#version 460 core
layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(std140, binding = 3) buffer Pixels
{
    vec4 colors[];
};

struct PixelData
{
    vec4 dir;
};
layout(std140, binding = 10) buffer PixelDataBuffer
{
    PixelData pixelData[];
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
    vec4 uv1;
    vec4 uv2;
    vec4 uv3;
    vec4 t1;
    vec4 t2;
    vec4 t3;
    vec4 b1;
    vec4 b2;
    vec4 b3;
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




struct Material
{
    vec4 color;
    vec4 emission;
    float reflectiveness;
    float transparency;
    float refractiveness;
    float reflectionDiffusion;
    float emissionStrength;
    float fresnelReflectionStrength;

    bool hasAlbedoTexture;
    float albedoTexture_xMin;
    float albedoTexture_xMax;
    float albedoTexture_yMin;
    float albedoTexture_yMax;

    bool hasNormalTexture;
    float normalTexture_xMin;
    float normalTexture_xMax;
    float normalTexture_yMin;
    float normalTexture_yMax;
    float normalMapStrength;
};

layout(std140, binding = 9) buffer Materials
{
    Material materials[];
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
uniform sampler2D textureAtlas;

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
    vec3 emission;
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
vec4 fireRayAtPixelPositionIndex(vec3 dir, int seed, int samples);

// Fire a ray, get the last intersection it makes
vec4 fireRay(bool reflect, int seed, Ray ray, Intersection closestIntersection);

vec3 getRayDirection(int pixelIndex, vec2 pixelPosIndex);

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
    if (materials[meshes[tri.mesh].material].hasNormalTexture)
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

        vec3 normal = -normalize(tri.n1.xyz * x + tri.n2.xyz * y + tri.n3.xyz * z).xyz;

        vec3 T = normalize(vec3(meshes[tri.mesh].transformation * tri.t1));
        vec3 B = normalize(vec3(meshes[tri.mesh].transformation * tri.b1));
        vec3 N = normalize(vec3(meshes[tri.mesh].transformation * -tri.n1));

        mat3 tbn1 = mat3(T, B, N);

        T = normalize(vec3(meshes[tri.mesh].transformation * tri.t2));
        B = normalize(vec3(meshes[tri.mesh].transformation * tri.b2));
        N = normalize(vec3(meshes[tri.mesh].transformation * -tri.n2));

        mat3 tbn2 = mat3(T, B, N);

        T = normalize(vec3(meshes[tri.mesh].transformation * tri.t3));
        B = normalize(vec3(meshes[tri.mesh].transformation * tri.b3));
        N = normalize(vec3(meshes[tri.mesh].transformation * -tri.n3));

        mat3 tbn3 = mat3(T, B, N);

        // If there is a normal texture we use it
        if (materials[meshes[tri.mesh].material].hasNormalTexture)
        {
            vec2 uv = tri.uv1.xy * x + tri.uv2.xy * y + tri.uv3.xy * z;
            mat3 tbn = tbn1 * x + tbn2 * y + tbn3 * z;

            // Normalzing uvs to [0, 1]
            uv.x = mod(uv.x + 1.0, 1.0);
            uv.y = mod(uv.y + 1.0, 1.0);

            float u = (uv.x * (materials[meshes[tri.mesh].material].normalTexture_xMax - materials[meshes[tri.mesh].material].normalTexture_xMin))
                + materials[meshes[tri.mesh].material].normalTexture_xMin;
            float v = (uv.y * (materials[meshes[tri.mesh].material].normalTexture_yMax - materials[meshes[tri.mesh].material].normalTexture_yMin))
                + materials[meshes[tri.mesh].material].normalTexture_yMin;

            vec3 normalFromMap = texture(textureAtlas, vec2(u, v)).xyz;

            normalFromMap = normalFromMap * 2.0 - 1.0;
            normalFromMap = normalize(tbn * normalFromMap);

            return normalize(materials[meshes[tri.mesh].material].normalMapStrength * normalFromMap + (1.0 - materials[meshes[tri.mesh].material].normalMapStrength) * normal);
        }

        return normal;
    }
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


// Get the normal of a triangle at the given position
vec3 sampleAlbedo(Tri tri, vec3 p)
{
    if (!materials[meshes[tri.mesh].material].hasAlbedoTexture)
    {
        return materials[meshes[tri.mesh].material].color.rgb;
    }

    // Finding the relative coordinates
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

    // Getting the albedo value from the texture
    vec2 uv = tri.uv1.xy * x + tri.uv2.xy * y + tri.uv3.xy * z;

    // Normalzing uvs to [0, 1]
    uv.x = mod(uv.x + 1.0, 1.0);
    uv.y = mod(uv.y + 1.0, 1.0);

    float u = (uv.x * (materials[meshes[tri.mesh].material].albedoTexture_xMax - materials[meshes[tri.mesh].material].albedoTexture_xMin))
        + materials[meshes[tri.mesh].material].albedoTexture_xMin;
    float v = (uv.y * (materials[meshes[tri.mesh].material].albedoTexture_yMax - materials[meshes[tri.mesh].material].albedoTexture_yMin))
        + materials[meshes[tri.mesh].material].albedoTexture_yMin;


    vec3 textureAlbedo = texture(textureAtlas, vec2(u, v)).xyz;

    // Default harsh normals
    return textureAlbedo * materials[meshes[tri.mesh].material].color.rgb;
}

#define PI 3.14159265359
#define RANDOM_TEXTURE_SIZE 8192

uniform sampler1D randomTexture;

float rand(int seed)
{
    float value = ((seed << 13) ^ seed) / 65536.4;
    return texture(randomTexture, fract(value)).r;
}

/*
// Returns a random value between 0 and 1 [0, 1)
float rand(float seed)
{
    float seedUsed = mod(seed, 2 * PI);
    return fract(sin(seedUsed) * 43758.5453);
}

float rand(int x, int y)
{
    int seed = x * 73856093 + y * 19349663;
    seed = (seed << 13) ^ seed;
    return fract((seed * (seed * seed * 15731 + 789221) + 1376312589) / 65536.0);
}

int randomInRange(int x, int y, float min, float max)
{
    return int(mix(min, max, rand(x, y)));
}

float rand(int seed)
{
    seed = (seed << 13) ^ seed;
    return fract((seed * (seed * seed * 15731 + 789221) + 1376312589) / 65536.0);
}*/
/*
float rand(int seed)
{
    float seedUsed = mod(float(seed), 2 * PI);
    return fract(sin(seedUsed) * 43758.5453);
}*/

float atan2(float x, float z)
{
    bool s = (abs(x) > abs(z));
    return mix(PI / 2.0 - atan(x, z), atan(z, x), s);
}


int toPixelIndex(int x, int y)
{
    return int(x + screenWidth * y);
}




vec3 getRandomDirection(int seed)
{
    vec3 dir = vec3(rand(seed) * 2.0 - 1.0, rand(seed*3 + seed % 31) * 2.0 - 1.0, rand(seed*7 + seed % 17) * 2.0 - 1.0);
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

    // If the first ray hit a light source directly, do not continue
    if (colors[pixelIndex].w == 1.0 && currentBlockRenderPassIndex > 0)
    {
        // for testing this feature
        //colors[pixelIndex] = vec4(1.0);
        return;
    }
    /* for testing this feature
    else if (currentBlockRenderPassIndex > 0)
    {
        colors[pixelIndex] = vec4(0.0);
        return;
    }*/

    
    //float neighbourSumBrightness = 0.0;
    float neighbourMaxBrightness = 0.0;
    float samples = 0;
    int neighbourhoodSize = 3;
    for (int ny = -neighbourhoodSize; ny <= neighbourhoodSize; ny++)
    {
        for (int nx = -neighbourhoodSize; nx <= neighbourhoodSize; nx++)
        {
            // Check for pixel out of bounds
            if (cx + nx < 0 || cx + nx >= screenSize.x || cy + ny < 0 || cy + ny >= screenSize.y)
            {
                continue;
            }

            int neighbourPixelIndex = toPixelIndex(cx + nx, cy + ny);
            vec4 neighbourColour = colors[neighbourPixelIndex];
            float brightness = (neighbourColour.x + neighbourColour.y + neighbourColour.z) / 3.0;
            //neighbourSumBrightness += brightness;
            //samples += 1.0;
            if (brightness > neighbourMaxBrightness)
            {
                neighbourMaxBrightness = brightness;
            }
        }
    }

    //float neighbourAvgBrightness = neighbourSumBrightness / samples;
    //neighbourAvgBrightness = 0.5;
    int samplesPerPixel = max(min(int(neighbourMaxBrightness * sampleCount), sampleCount), 1);
    //samplesPerPixel = sampleCount;

    // TODO make buffer empty on begin of render!
    if (currentBlockRenderPassIndex == 0)
    {
        // Empty the buffer here
        colors[pixelIndex] = vec4(0.0);
        pixelData[pixelIndex].dir = vec4(0.0);
        samplesPerPixel = sampleCount;
    }
    /*
    // for testing this feature
    else if (currentBlockRenderPassIndex == 1)
    {
        // Empty the buffer here
        colors[pixelIndex] = vec4((float(samplesPerPixel) / float(sampleCount)));
        return;
    }
    else
    {
        return;
    }*/
    


    vec4 finalColor = vec4(0.);
    vec3 rayDir = getRayDirection(pixelIndex, vec2(raydir_cx + 0.5, raydir_cy + 0.5));
    int seed = int(rand(cx) * 10000) * int(rand(cy * 2000) * 10000) + currentBlockRenderPassIndex * 171;
    finalColor += fireRayAtPixelPositionIndex(rayDir, seed, samplesPerPixel);

    //finalColor = materials[int(pixelIndex / 50) % 21].color.rgb;
    //finalColor = vec3(materials[int(pixelIndex / 50) % 21].hasAlbedoTexture ? 1. : 0., 0., 0.);

    float t = (1.0 / (float(currentBlockRenderPassIndex) + 1.0)) * (float(samplesPerPixel) / float(sampleCount));
    // examples:
    /* old - new
    0: 0.0 - 1.0   t= 1.0
    1: 0.5 - 0.5   t= 0.5
    2: 0.66 - 0.33 t= 0.33
    3: 0.75 - 0.25 t= 0.25
    4: 0.8 - 0.2   t= 0.20
    5: 0.83 - 0.17 t= 0.17
    */
    colors[pixelIndex] = colors[pixelIndex] * (1.0 - t) + finalColor * t;

    //colors[pixelIndex] = vec4(rand(seed));

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


vec3 getRayDirection(int pixelIndex, vec2 pixelPosIndex)
{
    // Already buffered, just return that
    if (pixelData[pixelIndex].dir.w != 0.0)
    {
        return pixelData[pixelIndex].dir.xyz;
    }

    vec3 dir;
    
    float d = 1 / tan(radians(fov) / 2);
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

    // Buffering the direction calculated
    pixelData[pixelIndex].dir = vec4(dir, 1.0);
    return dir;
}




vec4 fireRayAtPixelPositionIndex(vec3 dir, int seed, int samples)
{
    vec4 finalColor = vec4(0.);

    Ray startRay = Ray(cameraPosition, dir, false, 10000., vec3(0.), 0., 0, -1);
    Intersection startIntersection = getAllIntersections(startRay, -1, -1);

    for (int i = 0; i < samples; i++)
    {
        //Ray ray = fireRay(cameraPosition, dir, true, i + seed * 53);
        //Ray ray = fireRay(cameraPosition, dir, true, i * 67 * i + seed * 1471 * seed);
        //Ray ray = fireRay(cameraPosition, dir, true, i * 67 * i + seed * 17 * seed);

        vec4 color = fireRay(true, seed + i * 5, startRay, startIntersection);
        finalColor += color;
        //finalColor += fireRayAndGetFinalColor(blockLocalX, blockLocalY, cameraPosition, dir, i, seed + i * 5);
    }
    finalColor = finalColor / float(samples);
    return finalColor;
    //return vec3(0.0);
}




vec4 fireRay(bool reflect, int seed, Ray ray, Intersection closestIntersection)
{
    bool inTransparentMaterial = false;
    vec3 transparencyColorMultiplier = vec3(1.0);

    Intersection totalClosestIntersection;

    // Start by skipping the given values
    int skipTriThisIteration = -1;
    int skipSphereThisIteration = -1;

    vec3 color = vec3(1.0);
    bool hitLightSourceDirectly = false;

    // Reflections loop
    for (int i = 0; i < MAX_REFLECTIONS; i++)
    {
        // After this point no more skipping
        skipTriThisIteration = -1;
        skipSphereThisIteration = -1;

        // Check for hit
        if (!closestIntersection.intersected)
        {
            // Calculating sky color
            vec3 skyColor = vec3(0.0);
            
            if (true)
            {
                // Calculating HDRI position
                float yaw = atan2(ray.dir.z, ray.dir.x);
                float pitch = (-ray.dir.y / 2 + 0.5);

                skyColor = texture(hdri, vec2(yaw / (2 * PI), -pitch)).rgb * hdriLightStrength;
            }
            /*
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

                    finalColor = dirLights[lightIndex].color * (t)+skyColor * (1. - t);
                }
            }*/

            color *= skyColor;
            break;
        }
        else if (closestIntersection.emission != vec3(0.0))
        {
            color *= closestIntersection.emission;
            hitLightSourceDirectly = (i == 0);
            break;
        }
        else
        {
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

                //continue; // reflecting
            }
            else if (closestIntersection.transparency > EPSILON && i != MAX_REFLECTIONS - 1 && rand(seed * 13 + i * 47 + 5779) < closestIntersection.transparency)
            {
                // If the dot product is negative, the ray direction is opposing the normal,
                // so we are entering glass, otherwise we are exiting
                bool isEnteringTransparentMaterial = rayDirNormalDotProduct <= 0.0;
                vec3 rayPositionBeforeUpdate = ray.pos;

                // Refracting
                vec3 normal = sign(rayDirNormalDotProduct) * closestIntersection.normal;
                vec3 refractedRayDir = normalize(normal * closestIntersection.refractiveness +
                    (1.0 - closestIntersection.refractiveness) * ray.dir);
                ray.dir = refractedRayDir;

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

                    color *= closestIntersection.color;

                    //ray.finalColor = vec3(distanceFromTransparentMaterialEntry);
                    //ray.hit = true;
                    //break;
                }

                //ray.pos = vec3(5.0);




                totalClosestIntersection = closestIntersection;
                //return totalClosestIntersection;

                //continue; // refracting
                //Ray refractedRay =
                //    fireSecondaryRay(
                //        ray.pos + EPSILON * refractedRayDir,
                //        refractedRayDir,
                //        true,
                //        seed*29 + i*577
                //    );

                //ray.finalColor = refractedRay.finalColor;
                //ray.hit = false;
                //break;
            }
            else
            {
                // Hit a surface: scatter
                ray.dir = getRandomDirectionFollowingNormal(closestIntersection.normal, seed * 7 + i * 19 + 371);
                ray.pos = closestIntersection.pos + EPSILON * ray.dir;
                color *= closestIntersection.color;
                color *= dot(closestIntersection.normal, ray.dir);

                //return vec3(float(seed * 7 + i * 19 + 371) / 2147483647.0 * 7);

                // Use this to verify randomness!
                //return ray.dir;
                //return closestIntersection.color;
                //return materials[closestIntersection.materialIndex].color.rgb;

                //continue;
            }
        }

        // Updating the closest intersection after the first iteration
        closestIntersection = getAllIntersections(ray, skipTriThisIteration, skipSphereThisIteration);
    }

    return vec4(color * transparencyColorMultiplier, hitLightSourceDirectly ? 1.0 : 2.0);
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
    vec3 emission;
    */
    Intersection closestIntersection = Intersection(false, 1000., vec3(.0), -1, -1, vec3(0.), vec3(1.0), .0, .0, .0, 0, vec3(0.0), vec3(0.0));

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
                    closestIntersection.color = sampleAlbedo(triangles[j], isec.pos);
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
            Intersection isec = Intersection(false, 0, vec3(.0), -1, -1, vec3(0.), vec3(0.), .0, .0, .0, 0, vec3(0.0), vec3(0.0));

            isec.intersected = true;

            isec.depth = (-b - sqrt(det)) / 2.;
            isec.pos = ray.pos + ray.dir * isec.depth;
            isec.reflectiveness = materials[spheres[j].material].reflectiveness;
            isec.transparency = materials[spheres[j].material].transparency;
            isec.refractiveness = materials[spheres[j].material].refractiveness;
            isec.materialIndex = spheres[j].material;
            isec.color = materials[spheres[j].material].color.rgb;
            isec.normal = normalize(isec.pos - spheres[j].pos);
            isec.emission = materials[spheres[j].material].emission.rgb * materials[spheres[j].material].emissionStrength * 10.0;

            if (isec.depth < closestIntersection.depth)
            {
                closestIntersection = isec;
                closestIntersection.closestTriHit = -1;
                closestIntersection.closestSphereHit = j;
            }
        }
    }



    for (int j = 0; j < pointLightCount; j++)
    {
        // Calculating this sphere's intersection
        vec3 o_c = ray.pos - pointLights[j].pos; // (o-c)
        float half_b = dot(ray.dir, o_c);
        float b = 2.0 * half_b;
        float r = 0.2;
        float c = dot(o_c, o_c) - r * r;

        float det = b * b - 4 * c;


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
            float depth = (-b - sqrt(det)) / 2.;
            if (depth < closestIntersection.depth)
            {
                Intersection isec = Intersection(false, 0, vec3(.0), -1, -1, vec3(0.), vec3(0.), .0, .0, .0, 0, vec3(0.0), vec3(0.0));

                isec.intersected = true;

                isec.depth = depth;
                isec.pos = ray.pos + ray.dir * isec.depth;
                isec.color = pointLights[j].color;
                isec.normal = normalize(isec.pos - pointLights[j].pos);
                isec.emission = pointLights[j].color * pointLights[j].intensity * 10.0;


                closestIntersection = isec;
                closestIntersection.closestTriHit = -1;
                closestIntersection.closestSphereHit = -1;
            }
        }
    }

    return closestIntersection;
}





Intersection triangleIntersection(Tri tri, Ray ray)
{
    Intersection i = Intersection(false, 0, vec3(.0), -1, -1, vec3(0.), vec3(0.), .0, .0, .0, 0, vec3(0.0), vec3(0.0));
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
        //i.color = sampleAlbedo(tri, i.pos);
        i.color = materials[meshes[tri.mesh].material].color.rgb;
        i.reflectiveness = materials[meshes[tri.mesh].material].reflectiveness;
        i.transparency = materials[meshes[tri.mesh].material].transparency;
        i.refractiveness = materials[meshes[tri.mesh].material].refractiveness;
        i.materialIndex = meshes[tri.mesh].material;
        i.emission = materials[meshes[tri.mesh].material].emission.rgb * materials[meshes[tri.mesh].material].emissionStrength * 10.0;
        return i;
    }
    return i;
}