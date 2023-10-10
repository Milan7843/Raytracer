#version 460 core
layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(std140, binding = 3) buffer Pixels
{
    vec4 colors[];
};

uniform int screenWidth;
uniform int sampleCount;

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
#define NUM_MESHES 100
#define NUM_MATERIALS 40

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
    unsigned int bvhIndices[];
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
    mat4 transformation;
    int material;
    int padding[3];
};
layout(std430, binding = 8) buffer Meshes
{
    Mesh meshes[];
};




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

// Fire a ray through a pixel and get the final color
vec3 fireRayAtPixelPositionIndex(int blockLocalX, int blockLocalY, vec2 pixelPosIndex, int seed);

// Fire a ray, calculate the lighting and return the final color
vec3 fireRayAndGetFinalColor(int blockLocalX, int blockLocalY, vec3 pos, vec3 direction, int sampleIndex, int seed);

// Fire a ray, get the last intersection it makes
Intersection fireRay(vec3 pos, vec3 direction, bool reflect, int seed, bool ignoreRefraction, int skipTri, int skipSphere);

// Calculate the total lighting contribution at a position
vec3 calculateLights(Intersection intersection, int seed);

// Calculate the lighting contribution directly from light sources
vec3 calculateDirectLightingContribution(Intersection intersection, int seed);

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
    vec3 dir = vec3(rand(seed) * 2.0 - 1.0, rand(seed + 7) * 2.0 - 1.0, rand(seed + 13) * 2.0 - 1.0);
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
    // The pixel positions used for calculating the ray direction to be used
    int raydir_cx = 0;
    int raydir_cy = 0;

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

    raydir_cx = cx;
    raydir_cy = cy;

    if (pixelRenderSize != 1)
    {
        cx *= pixelRenderSize;
        cy *= pixelRenderSize;
        raydir_cx = cx + pixelRenderSize / 2;
        raydir_cy = cy + pixelRenderSize / 2;
    }

    // Calculating the total index, used to map the 2D indices to a 1D array
    int pixelIndex = int(cx + screenWidth * cy);

    vec3 finalColor1 = vec3(0.);

    /*
    for (int y = 0; y < multisamples; y++)
    {
        for (int x = 0; x < multisamples; x++)
        {
            finalColor += fireRayAtPixelPositionIndex(vec2(raydir_cx + 0.5, raydir_cy + 0.5) + vec2(x * d, -y * d),
                //pixelIndex * 13 * pixelIndex + pixelIndex + x * 3 + y * 17 + currentBlockRenderPassIndex * 2) / (multisamples * multisamples);
                pixelIndex * 1319 * pixelIndex + pixelIndex + pixelIndex * x * 107 * x * x + pixelIndex * y * 2549 * y + currentBlockRenderPassIndex * 89) / (multisamples * multisamples);
        }
    }*/

    vec3 finalColor = vec3(0.);
    finalColor += fireRayAtPixelPositionIndex(blockLocalX, blockLocalY, vec2(raydir_cx + 0.5, raydir_cy + 0.5),
        pixelIndex * 7 + currentBlockRenderPassIndex * 7);

    // TODO make buffer empty on begin of render!
    if (currentBlockRenderPassIndex == 0)
    {
        // Empty the buffer here
        colors[pixelIndex] = vec4(0.0);
    }

    float t = 1.0 / (float(currentBlockRenderPassIndex) + 1.0);
    // examples:
    /* old - new
    0: 0.0 - 1.0   t= 1.0
    1: 0.5 - 0.5   t= 0.5
    2: 0.66 - 0.33 t= 0.33
    3: 0.75 - 0.25 t= 0.25
    4: 0.8 - 0.2   t= 0.20
    5: 0.83 - 0.17 t= 0.17
    */
    colors[pixelIndex] = colors[pixelIndex] * (1.0 - t) + vec4(finalColor, 1.0) * t;

    // Writing the same pixel several times if we are rendering to multiple pixels
    if (pixelRenderSize != 1)
    {
        for (int i = 0; i < pixelRenderSize * pixelRenderSize; i++)
        {
            int x = i % pixelRenderSize;
            int y = i / pixelRenderSize;

            if (cx + x >= screenSize.x || cy + y >= screenSize.y)
            {
                continue;
            }
            int localPixelIndex = int((cx + x) + screenWidth * (cy + y));
            colors[localPixelIndex] = colors[pixelIndex];
        }
    }
}


vec3 fireRayAtPixelPositionIndex(int blockLocalX, int blockLocalY, vec2 pixelPosIndex, int seed)
{
    float d = 1 / tan(radians(fov) / 2);
    vec3 dir;
    float aspect_ratio = screenSize.x / screenSize.y;

    dir.x = aspect_ratio * (2 * pixelPosIndex.x / screenSize.x) - aspect_ratio;
    dir.y = (2 * pixelPosIndex.y / screenSize.y) - 1.;
    dir.z = d;
    dir = normalize(dir);

    float a = cameraRotation.x;
    float b = cameraRotation.y;
    float c = cameraRotation.z;

    // Precalculating sin
    float sa = sin(a);
    float sb = sin(b);
    float sc = sin(c);
    // Precalculating cos
    float ca = cos(a);
    float cb = cos(b);
    float cc = cos(c);

    // Calculating ray direction
    float   x = dir.x * ca * cb;
    x += dir.y * (ca * sb * sc - sa * cc);
    x += dir.z * (ca * sb * cc + sa * sc);

    float   y = dir.x * sa * cb;
    y += dir.y * (sa * sb * sc + ca * cc);
    y += dir.z * (sa * sb * cc - ca * sc);

    float   z = dir.x * -sb;
    z += dir.y * cb * sc + dir.z * cb * cc;

    dir = vec3(x, y, z);

    vec3 finalColor = vec3(0.);

    for (int i = 0; i < sampleCount; i++)
    {
        //Ray ray = fireRay(cameraPosition, dir, true, i + seed * 53);
        //Ray ray = fireRay(cameraPosition, dir, true, i * 67 * i + seed * 1471 * seed);
        //Ray ray = fireRay(cameraPosition, dir, true, i * 67 * i + seed * 17 * seed);
        finalColor += fireRayAndGetFinalColor(blockLocalX, blockLocalY, cameraPosition, dir, i, seed + i * 867);
    }
    finalColor = finalColor / float(sampleCount);
    return finalColor;
    //return vec3(0.0);
}





vec3 fireRayAndGetFinalColor(int blockLocalX, int blockLocalY, vec3 pos, vec3 direction, int sampleIndex, int seed)
{
    Ray ray = Ray(pos, direction, false, 10000., vec3(0.), 0., 0, -1);
    Intersection closestIntersection = fireRay(pos, direction, true, seed, false, -1, -1);

    ray.dir = closestIntersection.finalDirection;

    vec3 finalColor = vec3(0.0);

    // Check for hit
    if (!closestIntersection.intersected)
    {
        // Calculating sky color
        vec3 up = vec3(0., 1., 0.);
        float t = dot(up, ray.dir) + 0.6;
        /* ARTIFICIAL COLOR GRADIENT SKY
        vec3 skyColor = skyboxColorTop * (t)+skyboxColorHorizon * (1. - t);
        */
        vec3 skyColor = vec3(0.0);

        if (useHDRIAsBackground || i != 0)
        {
            // Calculating HDRI position
            float yaw = atan2(ray.dir.z, ray.dir.x);
            float pitch = (ray.dir.y / 2 + 0.5);

            skyColor = texture(hdri, vec2(yaw / (2 * PI), -pitch)).rgb;
        }
        finalColor = skyColor;


        // Rendering each directional light as a sort of sun, by doing the final color dot the -direction, 
        // to calculate how much the ray is going into the sun
        for (int lightIndex = 0; lightIndex < dirLightCount; lightIndex++)
        {
            t = dot(ray.dir, -normalize(dirLights[lightIndex].dir));
            float threshold = 0.98f;
            if (t > threshold)
            {
                // Normalize (threshold, 1.0] to (0.0, 1.0]
                t = (t - threshold) / (1. - threshold);

                finalColor = dirLights[lightIndex].color * (t)+finalColor * (1. - t);
            }
        }

        ray.finalColor = finalColor;
        finalColor *= closestIntersection.color;

        // Writing the data saying that no indirect lighting calculation should be performed at all
        if (renderUsingBlocks)
        {
            int index = blockLocalX + blockLocalY * blockSize + (blockSize * blockSize) * sampleIndex;

            IndirectLightingPixelData data = IndirectLightingPixelData(
                vec3(0.0),
                vec3(0.0),
                -2,
                vec3(0.0),
                -2
            );

            indirectLightingData[index] = data;
        }
    }
    else
    {
        finalColor =
            closestIntersection.color
            * 
            calculateLights(closestIntersection, seed);

        //finalColor =
        //    closestIntersection.normal;

        // Writing the data required for calculating the indirect lighting at this position later
        if (renderUsingBlocks)
        {
            int index = blockLocalX + blockLocalY * blockSize + (blockSize * blockSize) * sampleIndex;

            IndirectLightingPixelData data = IndirectLightingPixelData(
                closestIntersection.pos,
                closestIntersection.color,
                closestIntersection.closestTriHit,
                closestIntersection.normal,
                closestIntersection.closestSphereHit
            );

            indirectLightingData[index] = data;
        }
    }

    indirectLightingData[0].closestTriHit = 0;

    return finalColor;
    //return vec3(0.0);
}





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
            /*
            Tri tri = triangles[closestIntersection.closestTriHit];
            vec3 pos = closestIntersection.pos;
            vec3 v1 = (meshes[tri.mesh].transformation * vec4(tri.v1.xyz, 1.0)).zyx;
            vec3 v2 = (meshes[tri.mesh].transformation * vec4(tri.v2.xyz, 1.0)).zyx;
            vec3 v3 = (meshes[tri.mesh].transformation * vec4(tri.v3.xyz, 1.0)).zyx;
            ray.finalColor = vec3(distance(v1, pos), distance(v2, pos), distance(v3, pos));
            ray.hit = true;
            break;*/
            /*
            if (closestIntersection.transparency > 0.0 && closestIntersection.reflectiveness > 0.0 && reflect && i != MAX_REFLECTIONS - 1)
            {
                // Calculating the new ray direction for a reflection
                ray.finalColor += (1.0 - closestIntersection.transparency) * closestIntersection.color;

                // Making sure the normal always points with the ray
                float normalRayDirDot = dot(closestIntersection.normal, ray.dir);
                vec3 normal = sign(normalRayDirDot) * closestIntersection.normal;


                // Calculating two rays: one for reflection and one for transparency
                vec3 reflectedRayDir = normalize(ray.dir + normal * -2. * dot(ray.dir, normal));
                Ray reflectedRay =
                    fireSecondaryRay(
                        ray.pos + EPSILON * reflectedRayDir,
                        reflectedRayDir,
                        true
                    );

                // Refracted ray (going into the transparent object)
                vec3 refractedRayDir = normalize(normal * closestIntersection.refractiveness +
                    (1.0 - closestIntersection.refractiveness) * ray.dir);
                Ray refractedRay =
                    fireSecondaryRay(
                        ray.pos + EPSILON * refractedRayDir,
                        refractedRayDir,
                        true
                    );

                // This is the reflectiveness with view angle combined: [r, 1] with r = the original reflectiveness
                float usingReflectiveness = closestIntersection.reflectiveness + (1.0 - closestIntersection.reflectiveness) * (1.0 - abs(normalRayDirDot));

                // Combining the reflection and refraction colors
                ray.finalColor += usingReflectiveness * reflectedRay.finalColor
                    + (1.0 - usingReflectiveness) * refractedRay.finalColor;
                ray.hit = false;
                break;
            }*/


            float rayDirNormalDotProduct = dot(closestIntersection.normal, ray.dir);

            // Fresnel effect approximation reflection value
            // TODO turn fresnel back on at some point but i dont like it right now, it works too much on non-reflective objects like a couch
            float fresnelReflectionStrength = 0.;
            float fresnelReflectiveness = closestIntersection.reflectiveness + fresnelReflectionStrength * (1.0 - closestIntersection.reflectiveness) * pow(1.0 - max(0.0, rayDirNormalDotProduct), 5.0);

            if (rayDirNormalDotProduct < 0)
            {
                fresnelReflectiveness = 0;
            }

            if (fresnelReflectiveness > EPSILON && reflect && i != MAX_REFLECTIONS - 1 && rand(seed * 5 + i * 3) < fresnelReflectiveness)
            {
                // Calculating the new ray direction for a reflection
                //ray.finalColor += closestIntersection.color;

                ray.dir = normalize(ray.dir + closestIntersection.normal * -2. * dot(ray.dir, closestIntersection.normal));
                ray.pos = closestIntersection.pos + EPSILON * ray.dir;

                continue; // reflecting
            }
            else if (closestIntersection.transparency > EPSILON && i != MAX_REFLECTIONS - 1 && rand(seed * 13 + i * 47 + 5779) < closestIntersection.transparency)
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

                    //ray.finalColor = vec3(distanceFromTransparentMaterialEntry);
                    //ray.hit = true;
                    //break;
                }

                //ray.pos = vec3(5.0);




                totalClosestIntersection = closestIntersection;
                //return totalClosestIntersection;

                continue; // refracting
                /*
                Ray refractedRay =
                    fireSecondaryRay(
                        ray.pos + EPSILON * refractedRayDir,
                        refractedRayDir,
                        true,
                        seed*29 + i*577
                    );
                */

                //ray.finalColor = refractedRay.finalColor;
                //ray.hit = false;
                //break;
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





vec3 calculateLights(Intersection intersection, int seed)
{
    // Calculating the color from the lighting itself
    vec3 finalLightColor =
        calculateDirectLightingContribution(intersection, seed);

    // Factoring in emission
    finalLightColor +=
        materials[intersection.materialIndex].emission
        *
        materials[intersection.materialIndex].emissionStrength;

    // Making sure the light isn't out of bound
    finalLightColor = clamp(finalLightColor, vec3(0.0), vec3(1.0));

    return finalLightColor;
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
                fireRay(
                    intersection.pos,
                    dir,
                    false,
                    seed + i * 7,
                    false,
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

                finalLight += intensity * pointLights[i].color * pointLights[i].intensity * falloff;
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
                fireRay(
                    intersection.pos,
                    dir,
                    false,
                    seed + i * 7,
                    false,
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
        if (node.leftChild == -1)
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
            uint triangleCount = bvhIndices[node.rightChild];
            for (int i = 1; i <= triangleCount; i++)
            {
                uint j = bvhIndices[node.rightChild + i];

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
                    //closestIntersection.normal = vec3(0.0, 0.0, 1.0);
                }
            }
        }
        else
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
    vec3 s = ray.pos - vert1;
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