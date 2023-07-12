#version 460 core
layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(std140, binding = 3) buffer Pixels
{
    vec4 colors[];
};

uniform int screenWidth;
uniform int sampleCount;
uniform int multisamples;

uniform bool renderUsingBlocks;
uniform bool useHDRIAsBackground;
uniform float hdriLightStrength;

uniform int indirectLightingQuality;

uniform vec2 currentBlockOrigin;
uniform int blockSize;
uniform int renderPassCount;
uniform int currentBlockRenderPassIndex;
uniform int pixelRenderSize;

#define EPSILON 0.0001f

#define NUM_SPHERES 10
#define NUM_POINT_LIGHTS 10
#define NUM_DIR_LIGHTS 10
#define NUM_AMBIENT_LIGHTS 10
#define NUM_MESHES 20
#define NUM_MATERIALS 10

//#define NUM_TRIANGLES 1
//#define NUM_SPHERES 1
//#define NUM_POINT_LIGHTS 1
//#define NUM_DIR_LIGHTS 1
//#define NUM_AMBIENT_LIGHTS 1
//#define NUM_MESHES 1
//#define NUM_MATERIALS 1

#define MAX_REFLECTIONS 15

// Triangle
struct Tri
{
    // Vertex positions
    vec4 v1;
    vec4 v2;
    vec4 v3;
    // Normals (padded to be 4N each by std140)
    vec4 n1;
    vec4 n2;
    vec4 n3;
    vec3 normal;
    float reflectiveness;
    vec3 color;
    int mesh;
};

// TODO: make this variable length
layout(std140, binding = 2) buffer Tris
{
    Tri triangles[];
};


struct BVHNode
{
    vec3 pos;
    int leftChild;
    vec3 size;
    int rightChild;
};

// The BVH data
layout(std430, binding = 4) buffer BVHData
{
    BVHNode bvhNodes[];
};

// The BVH leaf index data
layout(std430, binding = 5) buffer BVHIndices
{
    int bvhIndices[];
};


// The data required for subsequent indirect lighting calculation
struct IndirectLightingPixelData
{
    vec3 position;
    vec3 color;
    int closestTriHit;
    vec3 normal;
    int closestSphereHit;
};

layout(std430, binding = 6) buffer IndirectLightingData
{
    IndirectLightingPixelData indirectLightingData[];
};




uniform vec3 cameraPosition;
uniform vec3 cameraRotation;
uniform vec2 screenSize;

float i = 0.;
float dstThreshold = 0.005;
uniform float fov;

vec3 skyboxColorHorizon = vec3(0.1, 0.2, 0.4);
vec3 skyboxColorTop = vec3(0.45, 0.95, 0.85);


struct Ray
{
    vec3 pos;
    vec3 dir;
    bool hit;
    float closestDst;
    vec3 finalColor;
    float timesReflected;
    int objectHitType;
    int closestObjIndex;
};

// SPHERE
struct Sphere
{
    vec3 pos;
    float radius;
    int material;
};
uniform Sphere spheres[NUM_SPHERES];
uniform int sphereCount;

float sphereDst(Sphere sph, vec3 pos);
vec3 getSphereNormal(Sphere sph, vec3 pos);


// LIGHTS
// Skybox
uniform sampler2D hdri;

struct PointLight
{
    vec3 pos;
    vec3 color;
    float intensity;
    float shadowSoftness;
};
uniform PointLight pointLights[NUM_POINT_LIGHTS];
uniform int pointLightCount;

struct DirLight
{
    vec3 dir;
    vec3 color;
    float intensity;
    float shadowSoftness;
};
uniform DirLight dirLights[NUM_DIR_LIGHTS];
uniform int dirLightCount;

struct AmbientLight
{
    vec3 color;
    float intensity;
};
uniform AmbientLight ambientLights[NUM_AMBIENT_LIGHTS];
uniform int ambientLightCount;




// Mesh
struct Mesh
{
    vec3 position;
    int material;
    mat4 transformation;
};
uniform Mesh meshes[NUM_MESHES];




struct Material
{
    vec3 color;
    vec3 emission;
    float reflectiveness;
    float transparency;
    float refractiveness;
    float reflectionDiffusion;
    float emissionStrength;
    float fresnelReflectionStrength;
};
uniform Material materials[NUM_MATERIALS];
uniform int materialCount;




struct Intersection
{
    bool intersected;
    float depth;
    vec3 pos;
    int closestTriHit;
    int closestSphereHit;
    vec3 normal;
    vec3 color;
    float reflectiveness;
    float transparency;
    float refractiveness;
    int materialIndex;
    vec3 finalDirection;
};




uniform int stackSize;
layout(std430, binding = 7) buffer StackBuffer
{
    int stack[];
};

#define initializeStack(top) (top = -1)
#define isStackEmpty(top) (top == -1)
#define isStackFull(top) (top == stackSize - 1)
#define pushStack(value, top) {\
    if (isStackFull(top)) {\
        /* Stack overflow, handle error condition */\
    } else {\
        stack[(int(gl_GlobalInvocationID.x) + int(gl_GlobalInvocationID.y) * blockSize)*32 + (++top)] = value;\
    }\
}
#define popStack(top) stack[(int(gl_GlobalInvocationID.x) + int(gl_GlobalInvocationID.y) * blockSize)*32 + (top--)]



// Intersect a ray with a single triangle
Intersection triangleIntersection(Tri tri, Ray ray);

// Find the closest intersection of a ray
Intersection getAllIntersections(Ray ray, int skipTri, int skipSphere);

// Calculate the lighting contribution from light bounces
vec3 calculateIndirectLightingContribution(IndirectLightingPixelData data, int seed);
// Calculate the indirection lighting contribution at a single point
vec3 calculateIndirectLightingContributionAtPosition(IndirectLightingPixelData data, int iterations, int seed);

Intersection fireRay(vec3 pos, vec3 direction, bool reflect, int seed, bool ignoreRefraction, int skipTri, int skipSphere);

vec3 calculateDirectLightingContribution(Intersection intersection, int seed);

vec3 calculateHDRILightingContribution(Intersection intersection, int iterations, int seed);

// Get a random 3D unit vector
vec3 getRandomDirection(int seed);

// Find the area of a triangle described by three vertices (v1, v2, v3)
float triangleArea(vec3 v1, vec3 v2, vec3 v3)
{
    return 0.5 * length(cross(v2 - v1, v3 - v1));
}

// Get the normal of a triangle at the given position
vec3 getNormal(Tri tri, vec3 p)
{
    // Smooth normals
    /*
    if (false)
    {
        vec3 vert1 = (meshes[tri.mesh].transformation * vec4(tri.v1.xyz, 1.0)).zyx;
        vec3 vert2 = (meshes[tri.mesh].transformation * vec4(tri.v2.xyz, 1.0)).zyx;
        vec3 vert3 = (meshes[tri.mesh].transformation * vec4(tri.v3.xyz, 1.0)).zyx;

        float area1 = triangleArea(p, vert2, vert3);
        float area2 = triangleArea(p, vert1, vert3);
        float area3 = triangleArea(p, vert1, vert2);

        float totalArea = area1 + area2 + area3;

        //area1 = area1 / totalArea;
        //area2 = area2 / totalArea;
        //area3 = area3 / totalArea;

        //return tri.n1.xyz;
        return normalize(tri.n1 * area1 + tri.n2 * area2 + tri.n3 * area3).xyz;
    }
    */
    if (true)
    {
        vec3 v1 = (meshes[tri.mesh].transformation * vec4(tri.v1.xyz, 1.0)).zyx;
        vec3 v2 = (meshes[tri.mesh].transformation * vec4(tri.v2.xyz, 1.0)).zyx;
        vec3 v3 = (meshes[tri.mesh].transformation * vec4(tri.v3.xyz, 1.0)).zyx;

        vec3 a = v2 - v1;
        vec3 b = v3 - v1;
        vec3 c = p - v1;
        float daa = dot(a, a);
        float dab = dot(a, b);
        float dbb = dot(b, b);
        float dca = dot(c, a);
        float dcb = dot(c, b);
        float denom = daa * dbb - dab * dab;
        float y = (dbb * dca - dab * dcb) / denom;
        float z = (daa * dcb - dab * dca) / denom;
        float x = 1.0f - y - z;
        return -normalize(tri.n1.xyz * x + tri.n2.xyz * y + tri.n3.xyz * z).xyz;
    }
    // Default harsh normals
    return tri.normal;
}

#define PI 3.14159265359

// Returns a random value between 0 and 1 [0, 1)
float rand(float seed)
{
    float seedUsed = mod(seed, 2 * PI);
    return fract(sin(seedUsed) * 43758.5453);
}

float rand(int seed)
{
    float seedUsed = mod(float(seed), 2 * PI);
    return fract(sin(seedUsed) * 43758.5453);
}

float atan2(float x, float z)
{
    bool s = (abs(x) > abs(z));
    return mix(PI / 2.0 - atan(x, z), atan(z, x), s);
}





vec3 getRandomDirection(int seed)
{
    vec3 dir = vec3(rand(seed)*2.0-1.0, rand(seed + 7) * 2.0 - 1.0, rand(seed + 13) * 2.0 - 1.0);
    return normalize(dir);
}

vec3 getRandomDirectionFollowingNormal(vec3 normal, int seed)
{
    vec3 dir = getRandomDirection(seed);
    if (dot(normal, dir) >= 0)
    {
        return dir;
    }
    return -dir;
}















void main()
{
    // 2 dimensional indices
    int cx = int(gl_GlobalInvocationID.x);
    int cy = int(gl_GlobalInvocationID.y);
    int blockLocalX = cx;
    int blockLocalY = cy;

    if (renderUsingBlocks)
    {
        cx += int(currentBlockOrigin.x);
        cy += int(currentBlockOrigin.y);

        // Check for indices off-screen
        if (cx >= screenSize.x || cy >= screenSize.y)
        {
            return;
        }
    }

    vec3 finalColor = vec3(0.);

    // Calculating the total index, used to map the 2D indices to a 1D array
    int pixelIndex = int(cx + screenWidth * cy);

    int seed = pixelIndex*3 + currentBlockRenderPassIndex * 171;

    for (int i = 0; i < sampleCount; i++)
    {
        int index = blockLocalX + blockLocalY * blockSize + (blockSize * blockSize) * i;

        IndirectLightingPixelData data = indirectLightingData[index];

        if (data.closestTriHit == -2)
        {
            continue;
        }

        finalColor += calculateIndirectLightingContribution(data, i * 13 + seed);
        //finalColor += calculateIndirectLightingContributionAtPosition(data, 1, i * 13 + seed);
        //finalColor += vec3(1.0);

        //vec3 dataShowing = getRandomDirectionFollowingNormal(data.normal, i * 13 + seed);
        //dataShowing = data.normal;
        //dataShowing = data.position;
        
        //finalColor += dataShowing
        
        //Intersection intersection;
        //intersection.pos = data.position;
        //intersection.normal = data.normal;
        //intersection.closestTriHit = data.closestTriHit;
        //intersection.closestSphereHit = data.closestSphereHit;
        //finalColor += calculateDirectLightingContribution(intersection, i * 13 + seed);
        //finalColor += calculateDirectLightingContribution(intersection, i * 13 + seed);
    }

    colors[pixelIndex] += vec4(finalColor / float(renderPassCount * sampleCount), 1.0);
    //colors[pixelIndex] = vec4(1.0);
}


vec3 calculateIndirectLightingContribution(IndirectLightingPixelData data, int seed)
{
    // Do no indirect lighting calculation if the quality is set to 0
    if (indirectLightingQuality == 0)
    {
        return vec3(0.0);
    }

    int iterations = indirectLightingQuality-1;

    vec3 finalColor = vec3(0.0);

    // How the color of the light is affected by surface colors
    vec3 currentLightBounceAffectColor = vec3(1.0);

    // Calculating the indirection color at the first position
    finalColor += calculateIndirectLightingContributionAtPosition(data, max(6, indirectLightingQuality * 3), seed);

    
    // Do no further indirect lighting calculation if the quality is set to 1
    if (indirectLightingQuality == 1)
    {
        return finalColor * data.color;
    }

    // Then doing it again for a single bounce
    vec3 dir = getRandomDirectionFollowingNormal(data.normal, seed + 9);// +i * 31 + 10);

    Ray ray;
    ray.pos = data.position;
    ray.dir = dir;

    Intersection intersection =
        getAllIntersections(
            ray,
            data.closestTriHit,
            data.closestSphereHit
        );

    IndirectLightingPixelData newData = IndirectLightingPixelData(
        intersection.pos,
        intersection.color,
        intersection.closestTriHit,
        intersection.normal,
        intersection.closestSphereHit
    );
    finalColor += calculateIndirectLightingContributionAtPosition(newData, max(4, indirectLightingQuality * 2), seed + 27);// *intersection.color;
    
    //vec3 dir = getRandomDirectionFollowingNormal(intersection.normal, seed + 10);// +i * 31 + 10);
    //
    //Ray ray;
    //ray.pos = intersection.pos;
    //ray.dir = dir;
    //
    //Intersection secondIntersection =
    //    getAllIntersections(
    //        ray,
    //        intersection.closestTriHit,
    //        intersection.closestSphereHit
    //    );
    //
    //finalColor +=
    //    currentLightBounceAffectColor *
    //    calculateIndirectLightingContributionAtPosition(
    //        secondIntersection,
    //        max(5, indirectLightingQuality * 2),
    //        seed + 110
    //    );

    //for (int i = 0; i < iterations; i++)
    //{
    //    vec3 dir = getRandomDirectionFollowingNormal(currentIntersection.normal, seed + 10);// +i * 31 + 10);
    //
    //    Ray ray;
    //    ray.pos = currentIntersection.pos;
    //    ray.dir = dir;
    //
    //    currentIntersection =
    //        getAllIntersections(
    //            ray,
    //            currentIntersection.closestTriHit,
    //            currentIntersection.closestSphereHit
    //        );
    //
    //    currentLightBounceAffectColor *= currentIntersection.color;
    //
    //    finalColor +=
    //        currentLightBounceAffectColor *
    //        calculateIndirectLightingContributionAtPosition(
    //            currentIntersection,
    //            max(5, indirectLightingQuality * 2),
    //            seed + 110// * i + 23
    //        );
    //}

    finalColor = clamp(finalColor, vec3(0.0), vec3(1.0));
    return finalColor * data.color;
}

vec3 calculateIndirectLightingContributionAtPosition(IndirectLightingPixelData data, int iterations, int seed)
{
    vec3 finalColor = vec3(0.0);
    
    for (int i = 0; i < iterations; i++)
    {
        vec3 dir = getRandomDirectionFollowingNormal(data.normal, seed + i * 37 + 3);

        Ray ray;
        ray.pos = data.position + dir * 0.0001;
        ray.dir = dir;
        
        Intersection isec = getAllIntersections(ray, data.closestTriHit, data.closestSphereHit);

        if (isec.intersected)
        {
            // Calculating the light contribution at this position
            vec3 light = 
                calculateDirectLightingContribution(isec, seed + i * 3 + 59)

                // Multiplying by the surface color, because that's what it's bouncing off of
                * isec.color;

            // Also calculating the HDRI light contribution at this position
            light +=
                calculateHDRILightingContribution(isec, indirectLightingQuality * 2, seed + i * 3 + 791)
            
                // Multiplying by the surface color, because that's what it's bouncing off of
                * isec.color;

            // Factoring in emission
            light +=
                materials[isec.materialIndex].emission
                *
                materials[isec.materialIndex].emissionStrength;

            finalColor += light;
        }
        else
        {
            // Calculating HDRI position
            float yaw = atan2(ray.dir.z, ray.dir.x);
            float pitch = (ray.dir.y / 2 + 0.5);

            vec3 skyColor = texture(hdri, vec2(yaw / (2 * PI), -pitch)).rgb;

            finalColor += skyColor * hdriLightStrength;
        }
    }

    //return vec3(1.0);
    return finalColor / float(iterations);
}





/*
Intersection fireRay(vec3 pos, vec3 direction, bool reflect, int seed, bool ignoreRefraction, int skipTri, int skipSphere)
{
    Ray ray = Ray(pos, direction, false, 10000., vec3(0.), 0., 0, -1);
    bool inTransparentMaterial = false;
    vec3 transparencyColorMultiplier = vec3(1.0);

    Intersection totalClosestIntersection;

    // Start by skipping the given values
    int skipTriThisIteration = skipTri;
    int skipSphereThisIteration = skipSphere;

    // Reflections loop
    for (int i = 0; i < MAX_REFLECTIONS; i++)
    {
        Intersection closestIntersection = getAllIntersections(ray, skipTriThisIteration, skipSphereThisIteration);

        // After this point no more skipping
        skipTriThisIteration = -1;
        skipSphereThisIteration = -1;

        // Check for hit
        if (!closestIntersection.intersected)
        {
            closestIntersection.finalDirection = ray.dir;
            totalClosestIntersection = closestIntersection;
            break;
        }
        else
        {
            float rayDirNormalDotProduct = dot(closestIntersection.normal, ray.dir);

            if (closestIntersection.transparency > EPSILON && i != MAX_REFLECTIONS - 1 && rand(seed * 13 + i * 47 + 5779) < closestIntersection.transparency)
            {
                // If the dot product is negative, the ray direction is opposing the normal,
                // so we are entering glass, otherwise we are exiting
                bool isEnteringTransparentMaterial = rayDirNormalDotProduct <= 0.0;
                vec3 rayPositionBeforeUpdate = ray.pos;

                if (!ignoreRefraction)
                {
                    vec3 normal = sign(rayDirNormalDotProduct) * closestIntersection.normal;
                    vec3 refractedRayDir = normalize(normal * closestIntersection.refractiveness +
                        (1.0 - closestIntersection.refractiveness) * ray.dir);

                    ray.dir = refractedRayDir;
                }

                ray.pos = closestIntersection.pos + EPSILON * ray.dir;

                // Tell the next iterations that we are in glass right now

                if (isEnteringTransparentMaterial)
                    inTransparentMaterial = true;
                else
                {
                    float distanceFromTransparentMaterialEntry = distance(ray.pos, rayPositionBeforeUpdate);
                    transparencyColorMultiplier = transparencyColorMultiplier *
                        (vec3(1.0) - min(distanceFromTransparentMaterialEntry + 0.1, 1.0) * (vec3(1.0) - closestIntersection.color));
                    inTransparentMaterial = false;
                }


                totalClosestIntersection = closestIntersection;

                continue; // refracting
            }
            else
            {
                totalClosestIntersection = closestIntersection;
                break;
            }
        }
    }

    totalClosestIntersection.color *= transparencyColorMultiplier;

    return totalClosestIntersection;
}
*/


vec3 calculateHDRILightingContribution(Intersection intersection, int iterations, int seed)
{
    vec3 finalColor = vec3(0.0);

    for (int i = 0; i < iterations; i++)
    {
        vec3 dir = getRandomDirectionFollowingNormal(intersection.normal, seed + i * 13 + 3);

        Ray ray;
        ray.pos = intersection.pos + dir * 0.0001;
        ray.dir = dir;

        Intersection isec = getAllIntersections(ray, intersection.closestTriHit, intersection.closestSphereHit);

        // In view of the HDRI for this ray
        if (!isec.intersected)
        {
            // Calculating HDRI position
            float yaw = atan2(ray.dir.z, ray.dir.x);
            float pitch = (ray.dir.y / 2 + 0.5);

            vec3 skyColor = texture(hdri, vec2(yaw / (2 * PI), -pitch)).rgb;

            finalColor += skyColor * hdriLightStrength;
        }
    }

    //return vec3(1.0);
    return finalColor / float(iterations);
}

vec3 calculateDirectLightingContribution(Intersection intersection, int seed)
{
    vec3 finalLight = vec3(0.);

    int samplesPerLight = 10;

    for (int sampleIndex = 0; sampleIndex < samplesPerLight; sampleIndex++)
    {

        /* POINT LIGHTS */
        for (int i = 0; i < pointLightCount; i++)
        {
            vec3 posUsing = pointLights[i].pos;

            posUsing += pointLights[i].shadowSoftness * getRandomDirection(seed + i * 3 + sampleIndex * 17) * 0.1;

            vec3 dist = posUsing - intersection.pos;
            vec3 dir = normalize(dist);

            // Doing ray trace light
            Ray ray;
            ray.pos = intersection.pos;
            ray.dir = dir;

            Intersection closestIntersection =
                getAllIntersections(
                    ray,
                    intersection.closestTriHit,
                    intersection.closestSphereHit
                );

            // Check for shadow ray hits
            if (!closestIntersection.intersected
                || distance(closestIntersection.pos, intersection.pos) > distance(pointLights[i].pos, intersection.pos))
            {
                float intensity = min(
                    (1. / (dir.x * dir.x + dir.y * dir.y + dir.z * dir.z))
                    * dot(dir, intersection.normal),
                    1.);


                float falloff = 1.0 / (dist.x * dist.x + dist.y * dist.y + dist.z * dist.z);

                finalLight += intensity * pointLights[i].color * pointLights[i].intensity * falloff * closestIntersection.color;
            }
            else
            {
                // In shadow for this light
            }
        }

        /* DIRECTIONAL LIGHTS */
        for (int i = 0; i < dirLightCount; i++)
        {
            vec3 dir = -dirLights[i].dir;
            dir = normalize(dir);

            dir += dirLights[i].shadowSoftness * getRandomDirection(seed + i * 3 + 179 + sampleIndex * 17) * 0.1;
            dir = normalize(dir);

            // Doing ray trace light (actually for shadows)
            Ray ray;
            ray.pos = intersection.pos;
            ray.dir = dir;

            Intersection closestIntersection =
                getAllIntersections(
                    ray,
                    intersection.closestTriHit,
                    intersection.closestSphereHit
                );

            // Check for hit
            if (!closestIntersection.intersected)
            {
                float intensity = min(
                    dot(dir, intersection.normal),
                    1.);

                finalLight += (intensity * dirLights[i].color * dirLights[i].intensity * closestIntersection.color);
            }
            else
            {
                // In shadow for this light
            }
        }
    }
    finalLight = finalLight / float(samplesPerLight);

    /* AMBIENT LIGHTS */
    for (int i = 0; i < ambientLightCount; i++)
    {
        finalLight += ambientLights[i].intensity * ambientLights[i].color;
    }

    return finalLight;
}








bool intersectBoxRay(vec3 boxPos, vec3 boxSize, vec3 rayOrigin, vec3 rayDirection)
{
    vec3 boxMin = boxPos - boxSize * 0.5;
    vec3 boxMax = boxPos + boxSize * 0.5;

    vec3 invRayDir = 1.0 / rayDirection;
    //vec3 invRayDir;
    //invRayDir.x = (rayDirection.x != 0.0) ? (1.0 / rayDirection.x) : 0.0;
    //invRayDir.y = (rayDirection.y != 0.0) ? (1.0 / rayDirection.y) : 0.0;
    //invRayDir.z = (rayDirection.z != 0.0) ? (1.0 / rayDirection.z) : 0.0;
    vec3 t1 = (boxMin - rayOrigin) * invRayDir;
    vec3 t2 = (boxMax - rayOrigin) * invRayDir;

    vec3 tmin = min(t1, t2);
    vec3 tmax = max(t1, t2);

    float tNear = max(max(tmin.x, tmin.y), tmin.z);
    float tFar = min(min(tmax.x, tmax.y), tmax.z);

    //bool insideBox = (tmin.x > tmin.y) && (tmin.x > tmin.z);

    bool insideBox =
        (rayOrigin.x >= boxMin.x && rayOrigin.x <= boxMax.x) &&
        (rayOrigin.y >= boxMin.y && rayOrigin.y <= boxMax.y) &&
        (rayOrigin.z >= boxMin.z && rayOrigin.z <= boxMax.z);

    //return (tNear <= tFar && tNear >= 0) || insideBox;
    return (tNear <= tFar && tNear >= 0) || insideBox;
}

Intersection getAllIntersections(Ray ray, int skipTri, int skipSphere)
{
    /*
    bool intersected;
    float depth;
    vec3 pos;
    int closestTriHit;
    int closestSphereHit;
    vec3 normal;
    vec3 color;
    float reflectiveness;
    float transparency;
    float refractiveness;
    int materialIndex;
    vec3 finalDirection;
    */
    Intersection closestIntersection = Intersection(false, 1000., vec3(.0), -1, -1, vec3(0.), vec3(1.0), .0, .0, .0, 0, vec3(0.0));
    /*
    if (intersectBoxRay(bvhNodes[0].pos, bvhNodes[0].size, ray.pos, ray.dir))
    //if (intersectBoxRay(vec3(0.0), vec3(1.0), ray.pos, ray.dir))
    {
        closestIntersection.intersected = true;
        closestIntersection.color = vec3(0.0);
        closestIntersection.reflectiveness = 0.0;
        closestIntersection.transparency = 0.0;
        closestIntersection.refractiveness = 0.0;
        return closestIntersection;
    }*/


    // Performing BVH traversal
    int top = -1;
    initializeStack(top);

    pushStack(0, top);

    while (!isStackEmpty(top))
    {
        int current = popStack(top);

        BVHNode node = bvhNodes[current];

        //Intersection isec = Intersection(true, 0, vec3(.0), -1, -1, vec3(0.), vec3(0.), .0, .0, .0, 0);
        //closestIntersection = isec;


        // Check for leaf
        if (node.leftChild != -1)
        {
            // Is not leaf

            // Check for intersection. If we intersect: add its children to the stack

            BVHNode leftChildNode = bvhNodes[node.leftChild];
            BVHNode rightChildNode = bvhNodes[node.rightChild];

            if (intersectBoxRay(leftChildNode.pos, leftChildNode.size, ray.pos, ray.dir))
            {
                pushStack(node.leftChild, top);
            }
            if (intersectBoxRay(rightChildNode.pos, rightChildNode.size, ray.pos, ray.dir))
            {
                pushStack(node.rightChild, top);
            }
        }
        else
        {
            // Is leaf
            //if (intersectBoxRay(node.pos, node.size, ray.pos, ray.dir))
            //{
            //    closestIntersection.intersected = true;
            //    closestIntersection.color = vec3(0.0);
            //    closestIntersection.reflectiveness = 0.0;
            //    closestIntersection.transparency = 0.0;
            //    closestIntersection.refractiveness = 0.0;
            //    return closestIntersection;
            //}

            // Check all triangles inside it
            int triangleCount = bvhIndices[node.rightChild];

            for (int i = 1; i <= triangleCount; i++)
            {
                int j = bvhIndices[node.rightChild + i];

                // Check triangle intersection

                // Skip already hit tri
                if (j == skipTri) continue;

                Intersection isec = triangleIntersection(triangles[j], ray);
                if (isec.intersected && isec.depth < closestIntersection.depth)
                {
                    closestIntersection = isec;
                    closestIntersection.closestTriHit = int(j);
                    // TODO optimise this to to as few getNormal calls as possible (not on underlying faces)
                    closestIntersection.normal = getNormal(triangles[j], isec.pos);
                }
            }
        }
    }


    // Checking triangle ray hits
    /*
    for (int j = 0; j < triangles.length(); j++)
    {
        // Skip already hit tri
        if (j == skipTri) continue;

        Intersection isec = triangleIntersection(triangles[j], ray);
        if (isec.intersected && isec.depth < closestIntersection.depth)
        {
            closestIntersection = isec;
            closestIntersection.closestTriHit = j;
            // TODO optimise this to to as few getNormal calls as possible (not on underlying faces)
            closestIntersection.normal = getNormal(triangles[j], isec.pos);
        }
    }*/

    // Calculating ray-sphere intersections
    for (int j = 0; j < sphereCount; j++)
    {
        // Skip already hit sphere
        if (j == skipSphere) continue;

        // Calculating this sphere's intersection
        vec3 o_c = ray.pos - spheres[j].pos; // (o-c)
        float half_b = dot(ray.dir, o_c);
        float b = 2.0 * half_b;
        float c = dot(o_c, o_c) - spheres[j].radius * spheres[j].radius;

        float det = b * b - 4 * c;

        Intersection isec = Intersection(false, 0, vec3(.0), -1, -1, vec3(0.), vec3(0.), .0, .0, .0, 0, vec3(0.0));

        if (b > 0.0 || det < 0.0
            // Allows for single-sided spheres, necessary for sphere transparency and refraction
            || c < 0)
        {
            // No intersection
            continue;
        }
        // Two intersections
        else
        {
            isec.intersected = true;

            isec.depth = (-b - sqrt(det)) / 2.;
            isec.pos = ray.pos + ray.dir * isec.depth;
            isec.reflectiveness = materials[spheres[j].material].reflectiveness;
            isec.transparency = materials[spheres[j].material].transparency;
            isec.refractiveness = materials[spheres[j].material].refractiveness;
            isec.materialIndex = spheres[j].material;
            isec.color = materials[spheres[j].material].color;
            isec.normal = normalize(isec.pos - spheres[j].pos);

            if (isec.depth < closestIntersection.depth)
            {
                closestIntersection = isec;
                closestIntersection.closestTriHit = -1;
                closestIntersection.closestSphereHit = j;
            }
        }
    }
    return closestIntersection;
}





Intersection triangleIntersection(Tri tri, Ray ray)
{
    Intersection i = Intersection(false, 0, vec3(.0), -1, -1, vec3(0.), vec3(0.), .0, .0, .0, 0, vec3(0.0));
    i.intersected = false;

    const float epsilon = 0.0000001;

    vec3 vert1 = (meshes[tri.mesh].transformation * vec4(tri.v1.xyz, 1.0)).zyx;
    vec3 vert2 = (meshes[tri.mesh].transformation * vec4(tri.v2.xyz, 1.0)).zyx;
    vec3 vert3 = (meshes[tri.mesh].transformation * vec4(tri.v3.xyz, 1.0)).zyx;

    /* optimisation?
    vec3 toVertex = normalize(tri.v1.xyz - ray.pos);
    if (dot(ray.dir, toVertex) > 0.9999f)
    {
        return i;
    }*/

    // Edges 1 and 2
    vec3 e1, e2;
    e1 = vert2 - vert1;
    e2 = vert3 - vert1;

    vec3 h = cross(ray.dir, e2);
    float a = dot(e1, h);

    if (a > -epsilon && a < epsilon)
    {
        return i;
    }
    float f = 1. / a;
    vec3 s = ray.pos - (vert1 + meshes[tri.mesh].position);
    float u = f * dot(s, h);
    if (u < 0.0 || u > 1.)
    {
        return i;
    }
    vec3 q = cross(s, e1);
    float v = f * dot(ray.dir, q);
    if (v < 0.0 || u + v > 1.)
    {
        return i;
    }

    float t = f * dot(e2, q);
    if (t > epsilon)
    {
        // Ray intersection
        i.intersected = true;
        i.pos = ray.pos + ray.dir * t;
        i.depth = t;
        i.normal = tri.normal.xyz;
        i.color = materials[meshes[tri.mesh].material].color.rgb;
        i.reflectiveness = materials[meshes[tri.mesh].material].reflectiveness;
        i.transparency = materials[meshes[tri.mesh].material].transparency;
        i.refractiveness = materials[meshes[tri.mesh].material].refractiveness;
        i.materialIndex = meshes[tri.mesh].material;
        return i;
    }
    return i;
}