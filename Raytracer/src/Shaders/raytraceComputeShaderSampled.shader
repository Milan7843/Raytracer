#version 460 core
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std140, binding = 3) buffer Pixels
{
    vec4 colors[];
};

uniform int screenWidth;
uniform int sampleCount;
uniform int multisamples;

uniform bool renderUsingBlocks;
uniform vec2 currentBlockOrigin;
uniform int blockSize;
uniform int renderPassCount;
uniform int currentBlockRenderPassIndex;

#define EPSILON 0.0001f

#define NUM_SPHERES 10
#define NUM_POINT_LIGHTS 10
#define NUM_DIR_LIGHTS 10
#define NUM_AMBIENT_LIGHTS 10
#define NUM_MESHES 10
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
    vec4 v1;
    vec4 v2;
    vec4 v3;
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
};
uniform PointLight pointLights[NUM_POINT_LIGHTS];
uniform int pointLightCount;

struct DirLight
{
    vec3 dir;
    vec3 color;
    float intensity;
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
    float reflectiveness;
    float transparency;
    float refractiveness;
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
};

Intersection triangleIntersection(Tri tri, Ray ray);

Intersection getAllIntersections(Ray ray, int skipTri, int skipSphere);

Ray fireRay(vec3 pos, vec3 direction, bool reflect, int seed);
Ray fireSecondaryRay(vec3 pos, vec3 direction, bool reflect, int seed);

vec3 calculateLights(vec3 pos, vec3 normal, int triHit, int sphereHit);

vec3 fireRayAtPixelPositionIndex(vec2 pixelPosIndex, int seed);

// Returns a random value between 0 and 1 [0, 1)
float rand(float seed)
{
    return fract(sin(seed) * 43758.5453);
}

float rand(int seed)
{
    return fract(sin(float(seed)) * 43758.5453);
}

#define PI 3.14159265359
float atan2(float x, float z)
{
    bool s = (abs(x) > abs(z));
    return mix(PI/2.0 - atan(x, z), atan(z, x), s);
}


void main()
{
    // 2 dimensional indices
    int cx = int(gl_GlobalInvocationID.x);
    int cy = int(gl_GlobalInvocationID.y);

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

    // Calculating the total index, used to map the 2D indices to a 1D array
    int pixelIndex = int(cx + screenWidth * cy);

    vec3 finalColor1 = vec3(0.);

    float d = 1. / (float(multisamples + 1));
    vec3 finalColor = vec3(0.);

    for (int y = 0; y < multisamples; y++)
    {
        for (int x = 0; x < multisamples; x++)
        {
            finalColor += fireRayAtPixelPositionIndex(vec2(cx, cy) + vec2(x * d, -y * d), pixelIndex*1319* pixelIndex + pixelIndex*x*107*x*x + pixelIndex*y*2549*y + currentBlockRenderPassIndex*89) / (multisamples * multisamples);
        }
    }

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
}


vec3 fireRayAtPixelPositionIndex(vec2 pixelPosIndex, int seed)
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
    float   x = dir.x * cos(a) * cos(b);
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
        Ray ray = fireRay(cameraPosition, dir, true, i * 67 * i + seed * 1471 * seed);
        finalColor += ray.finalColor;
    }
    finalColor = finalColor / float(sampleCount);
    return finalColor;
}


Ray fireRay(vec3 pos, vec3 direction, bool reflect, int seed)
{
    Ray ray = Ray(pos, direction, false, 10000., vec3(0.), 0., 0, -1);

    // Reflections loop
    for (int i = 0; i < MAX_REFLECTIONS; i++)
    {
        Intersection closestIntersection = getAllIntersections(ray, -1, -1);

        // Check for hit
        if (!closestIntersection.intersected)
        {
            // Calculating sky color
            vec3 up = vec3(0., 1., 0.);
            float t = dot(up, ray.dir) + 0.6;
            vec3 skyColor = skyboxColorTop * (t)+skyboxColorHorizon * (1. - t);

            // Calculating HDRI position
            float yaw = atan2(ray.dir.x, ray.dir.z);
            float pitch = (ray.dir.y / 2 + 0.5);

            skyColor = texture(hdri, vec2(yaw/(2*PI), -pitch)).rgb;

            vec3 finalColor = skyColor;


            // Rendering each directional light as a sort of sun, by doing the final color dot the -direction, 
            // to calculate how much the ray is going into the sun
            for (int i = 0; i < dirLightCount; i++)
            {
                t = dot(ray.dir, -dirLights[i].dir);
                float threshold = 0.98f;
                if (t > threshold)
                {
                    // Normalize (threshold, 1.0] to (0.0, 1.0]
                    t = (t - threshold) / (1. - threshold);

                    finalColor = dirLights[i].color * (t)+finalColor * (1. - t);
                }
            }

            ray.finalColor += finalColor;
            break;
        }
        else
        {
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

            if (closestIntersection.reflectiveness > 0.0 && reflect && i != MAX_REFLECTIONS - 1 && rand(seed*17 + i * 1559) < closestIntersection.reflectiveness)
            {
                // Calculating the new ray direction for a reflection
                ray.finalColor += (1.0 - closestIntersection.reflectiveness) * closestIntersection.color;

                ray.dir = normalize(ray.dir + closestIntersection.normal * -2. * dot(ray.dir, closestIntersection.normal));
                ray.pos = closestIntersection.pos + EPSILON * ray.dir;
                continue; // reflecting
            }
            else if (closestIntersection.transparency > 0.0 && reflect && i != MAX_REFLECTIONS - 1 && rand(seed*13 + i * 47 + 5779) < closestIntersection.transparency)
            {
                vec3 normal = sign(dot(closestIntersection.normal, ray.dir)) * closestIntersection.normal;
                vec3 refractedRayDir = normalize(normal * closestIntersection.refractiveness +
                    (1.0 - closestIntersection.refractiveness) * ray.dir);
                Ray refractedRay =
                    fireSecondaryRay(
                        ray.pos + EPSILON * refractedRayDir,
                        refractedRayDir,
                        true,
                        seed*29 + i*577
                    );

                ray.finalColor = refractedRay.finalColor;
                ray.hit = false;
                break;
            }
            else
            {
                if (reflect)
                {
                    ray.finalColor += closestIntersection.color * calculateLights(closestIntersection.pos, closestIntersection.normal,
                        closestIntersection.closestTriHit, closestIntersection.closestSphereHit);
                }
                else
                {
                    ray.finalColor = closestIntersection.color;
                }
                ray.hit = true;
                break;
            }
        }
    }

    return ray;
}


Ray fireSecondaryRay(vec3 pos, vec3 direction, bool reflect, int seed)
{
    Ray ray = Ray(pos, direction, false, 10000., vec3(0.), 0., 0, -1);

    // Reflections loop
    for (int i = 0; i < MAX_REFLECTIONS; i++)
    {
        Intersection closestIntersection = getAllIntersections(ray, -1, -1);

        // Check for hit
        if (!closestIntersection.intersected)
        {
            // Calculating sky color
            vec3 up = vec3(0., 1., 0.);
            float t = dot(up, ray.dir) + 0.6;
            vec3 skyColor = skyboxColorTop * (t)+skyboxColorHorizon * (1. - t);

            vec3 finalColor = skyColor;


            // Rendering each directional light as a sort of sun, by doing the final color dot the -direction, 
            // to calculate how much the ray is going into the sun
            for (int i = 0; i < dirLightCount; i++)
            {
                t = dot(ray.dir, -dirLights[i].dir);
                float threshold = 0.98f;
                if (t > threshold)
                {
                    // Normalize (threshold, 1.0] to (0.0, 1.0]
                    t = (t - threshold) / (1. - threshold);

                    finalColor = dirLights[i].color * (t)+finalColor * (1. - t);
                }
            }

            ray.finalColor += finalColor;
            break;
        }
        else
        {
            // Do transparency first, as the ray otherwise could not escape from a transparent object (it would just reflect)
            if (closestIntersection.transparency > 0.0 && reflect && i != MAX_REFLECTIONS - 1)
            {
                // Calculating the new ray direction for a reflection
                ray.finalColor += (1.0 - closestIntersection.transparency) * closestIntersection.color;

                // Making the normal always face the right way
                vec3 normal = sign(dot(closestIntersection.normal, ray.dir)) * closestIntersection.normal;

                // Refracting the light
                ray.dir = normalize(normal * closestIntersection.refractiveness + (1.0 - closestIntersection.refractiveness) * ray.dir);
                ray.pos = closestIntersection.pos + EPSILON * ray.dir;
                continue; // reflecting
            }
            else if (closestIntersection.reflectiveness > 0.0 && reflect && i != MAX_REFLECTIONS - 1 && rand(seed*3 + i * 97) < closestIntersection.reflectiveness)
            {
                // Calculating the new ray direction for a reflection
                ray.finalColor += (1.0 - closestIntersection.reflectiveness) * closestIntersection.color;
                ray.dir = normalize(ray.dir + closestIntersection.normal * -2. * dot(ray.dir, closestIntersection.normal));
                ray.pos = closestIntersection.pos + EPSILON * ray.dir;
                continue; // reflecting
            }
            else
            {
                if (reflect)
                {
                    ray.finalColor = closestIntersection.color * calculateLights(closestIntersection.pos, closestIntersection.normal,
                        closestIntersection.closestTriHit, closestIntersection.closestSphereHit);
                }
                else
                {
                    ray.finalColor = closestIntersection.color;
                }
                ray.hit = true;
                break;
            }
        }
    }

    return ray;
}





vec3 calculateLights(vec3 pos, vec3 normal, int triHit, int sphereHit)
{
    vec3 finalLight = vec3(0.);

    /* POINT LIGHTS */
    for (int i = 0; i < pointLightCount; i++)
    {
        vec3 dist = pointLights[i].pos - pos;
        vec3 dir = normalize(dist);

        // Doing ray trace light
        Ray ray;
        ray.pos = pos;
        ray.dir = dir;

        Intersection closestIntersection = getAllIntersections(ray, triHit, sphereHit);

        // Check for shadow ray hits
        if (!closestIntersection.intersected
            || distance(closestIntersection.pos, pos) > distance(pointLights[i].pos, pos))
        {
            float intensity = min(
                (1. / (dir.x * dir.x + dir.y * dir.y + dir.z * dir.z))
                * dot(-dir, normal),
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

        // Doing ray trace light (actually for shadows)
        Ray ray;
        ray.pos = pos;
        ray.dir = dir;

        Intersection closestIntersection = getAllIntersections(ray, triHit, sphereHit);

        // Check for hit
        if (!closestIntersection.intersected)
        {
            // Works somehow??
            float intensity = min(
                dot(-dir, normal),
                1.);

            finalLight += intensity * dirLights[i].color * dirLights[i].intensity;
        }
        else
        {
            // In shadow for this light
        }
    }

    /* AMBIENT LIGHTS */
    for (int i = 0; i < ambientLightCount; i++)
    {
        finalLight += ambientLights[i].intensity * ambientLights[i].color;
    }

    return finalLight;
}












Intersection getAllIntersections(Ray ray, int skipTri, int skipSphere)
{
    Intersection closestIntersection;
    closestIntersection.depth = 1000.;
    closestIntersection.intersected = false;
    closestIntersection.closestTriHit = -1;

    // Checking triangle ray hits 
    for (int j = 0; j < triangles.length(); j++)
    {
        // Skip already hit tri
        if (j == skipTri) continue;

        Intersection isec = triangleIntersection(triangles[j], ray);
        if (isec.intersected && isec.depth < closestIntersection.depth)
        {
            closestIntersection = isec;
            closestIntersection.closestTriHit = j;
            isec.normal = triangles[j].normal;
        }
    }

    // Calculating ray-sphere intersections
    for (int j = 0; j < sphereCount; j++)
    {
        // Skip already hit tri
        if (j == skipSphere) continue;

        // Calculating this sphere's intersection
        vec3 o_c = ray.pos - spheres[j].pos; // (o-c)
        float half_b = dot(ray.dir, o_c);
        float b = 2.0 * half_b;
        float c = dot(o_c, o_c) - spheres[j].radius * spheres[j].radius;

        float det = b * b - 4 * c;

        Intersection isec = Intersection(false, 0, vec3(.0), -1, -1, vec3(0.), vec3(0.), .0, .0, .0);

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
            isec.color = materials[spheres[j].material].color;
            isec.normal = normalize(spheres[j].pos - isec.pos);
        }

        if (isec.intersected && isec.depth < closestIntersection.depth)
        {
            closestIntersection = isec;
            closestIntersection.closestTriHit = -1;
            closestIntersection.closestSphereHit = j;
        }
    }
    return closestIntersection;
}








Intersection triangleIntersection(Tri tri, Ray ray)
{
    Intersection i;
    i.intersected = false;

    const float epsilon = 0.0000001;

    vec3 vert1 = (meshes[tri.mesh].transformation * vec4(tri.v1.xyz, 1.0)).zyx;
    vec3 vert2 = (meshes[tri.mesh].transformation * vec4(tri.v2.xyz, 1.0)).zyx;
    vec3 vert3 = (meshes[tri.mesh].transformation * vec4(tri.v3.xyz, 1.0)).zyx;

    /*
    vec3 toVertex = normalize(tri.v1.xyz - ray.pos);
    if (dot(ray.dir, toVertex) < 0.9999f)
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
        return i;
    }
    return i;
}