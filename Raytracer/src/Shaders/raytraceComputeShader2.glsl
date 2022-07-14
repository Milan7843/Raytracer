#version 460 core
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 3) buffer Pixels
{
	vec4 colors[];
};

uniform int screenWidth;
uniform int sampleCount;
uniform int currentFrameSampleCount;

#define EPSILON 0.0001f

#define NUM_TRIANGLES $numTriangles
#define NUM_SPHERES $numSpheres
#define NUM_POINT_LIGHTS $numPointLights
#define NUM_DIR_LIGHTS 1
#define NUM_AMBIENT_LIGHTS 1
#define NUM_MESHES $numMeshes
#define NUM_MATERIALS $numMaterials

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

layout(std140, binding = 2) buffer Tris
{
    Tri triangles[NUM_TRIANGLES];
};

uniform vec3 cameraPosition;
uniform vec3 cameraRotation;
uniform vec2 screenSize;

float i = 0.;
float dstThreshold = 0.005;
float fov = 40;

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

float sphereDst(Sphere sph, vec3 pos);
vec3 getSphereNormal(Sphere sph, vec3 pos);


// LIGHTS
struct PointLight
{
    vec3 pos;
    vec3 color;
    float intensity;
};

uniform PointLight pointLights[NUM_POINT_LIGHTS];

struct DirLight
{
    vec3 dir;
    vec3 color;
    float intensity;
};
DirLight dirLights[NUM_DIR_LIGHTS] = DirLight[](
    //         Pos                  Color                   Intensity
    DirLight(vec3(.707, -.707, 0.), vec3(1.0, 1.0, 0.9), 0.8)
);

struct AmbientLight
{
    vec3 color;
    float intensity;
};
AmbientLight ambientLights[1] = AmbientLight[](
    //           Color                  Intensity
    AmbientLight(vec3(0.8, 0.8, 1.0), 0.3)
);

// Mesh
struct Mesh
{
    vec3 position;
    int material;
};
uniform Mesh meshes[NUM_MESHES];

struct Material
{
    vec3 color;
    float reflectiveness;
    float transparency;
    float refractiveness;
    float emmissiveness;
};
uniform Material materials[NUM_MATERIALS];


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
    float emmissiveness;
};

Intersection triangleIntersection(Tri tri, Ray ray);

Intersection getAllIntersections(Ray ray, int skipTri, int skipSphere);

Ray fireRay(vec3 pos, vec3 direction, bool reflect, float seed);
Ray fireSecondaryRay(vec3 pos, vec3 direction, bool reflect);

vec3 calculateLights(vec3 pos, vec3 normal, int triHit, int sphereHit, float seed);

Ray fireRayAtPixelPositionIndex(vec2 pixelPosIndex);

vec3 fireLightSampleRay(Ray ray, int skipTri, int skipSphere);

vec3 getGlobalIllumination(vec3 direction);


// Returns a random value between 0 and 1 [0, 1)
float rand(float seed)
{
    return fract(sin(seed) * 10000.);
}

void main()
{
	// 2 dimensional indices
	int cx = int(gl_GlobalInvocationID.x);
	int cy = int(gl_GlobalInvocationID.y);

	// Calculating the total index, used to map the 2D indices to a 1D array
	int pixelIndex = int(cx + screenWidth * cy);
    
    vec3 finalColor = vec3(0.);

    for (int s = 0; s < sampleCount; s++)
    {
        finalColor += fireRayAtPixelPositionIndex(vec2(cx, cy)).finalColor / float(sampleCount);
    }
    float currentFrameSampleCountFloat = float(currentFrameSampleCount);
    float newFramePart = 1.0 / currentFrameSampleCountFloat;
	colors[pixelIndex] = colors[pixelIndex] * (1. - newFramePart) + vec4(fireRayAtPixelPositionIndex(vec2(cx, cy)).finalColor, 1.0) * newFramePart;
}


Ray fireRayAtPixelPositionIndex(vec2 pixelPosIndex)
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

    Ray ray = fireRay(cameraPosition, dir, true, pixelPosIndex.x + pixelPosIndex.y*4000);
    return ray;
}


Ray fireRay(vec3 pos, vec3 direction, bool reflect, float seed)
{
    Ray ray = Ray(pos, direction, false, 10000., vec3(0.), 0., 0, -1);

    // Reflections loop
    for (int i = 0; i < MAX_REFLECTIONS; i++)
    {
        Intersection closestIntersection = getAllIntersections(ray, -1, -1);

        // Check for hit
        if (!closestIntersection.intersected)
        {
            ray.finalColor += getGlobalIllumination(ray.dir);
            break;
        }
        else
        {
            // Always have normal pointing with ray
            float normalRayDirDot = dot(closestIntersection.normal, ray.dir);
            vec3 normal = sign(normalRayDirDot) * closestIntersection.normal;

            // Reflect by chance
            if (rand(seed) < closestIntersection.reflectiveness && i != MAX_REFLECTIONS - 1)
            {
                vec3 reflectedRayDir = normalize(ray.dir + normal * -2. * dot(ray.dir, normal));
                ray.dir = reflectedRayDir;
                ray.pos = ray.pos + EPSILON * reflectedRayDir;
                continue;
            }

            // Go through (refract) by chance
            if (rand(seed) < closestIntersection.transparency && i != MAX_REFLECTIONS - 1)
            {
                vec3 refractedRayDir = normalize(normal * closestIntersection.refractiveness +
                    (1.0 - closestIntersection.refractiveness) * ray.dir);
                ray.dir = refractedRayDir;
                ray.pos = ray.pos + EPSILON * refractedRayDir;
                continue;
            }

            // Lastly just calculating lights if not reflecting and not passing through
            ray.finalColor += closestIntersection.color * calculateLights(closestIntersection.pos, closestIntersection.normal,
                closestIntersection.closestTriHit, closestIntersection.closestSphereHit, seed + i*15);
                
            ray.hit = true;
            break;
        }
    }

    return ray;
}







vec3 calculateLights(vec3 pos, vec3 normal, int triHit, int sphereHit, float seed)
{
    vec3 finalLight = vec3(0.);
    for (int i = 0; i < 10; i++) {
        // Calculating a random ray direction
        vec3 lightSampleRayDirection = vec3(rand(seed + i*100), rand(seed + i*125 + 1500), rand(seed + i*150 + 3000));
        float normalRayDirDot = dot(normal, lightSampleRayDirection);
        lightSampleRayDirection = sign(normalRayDirDot) * lightSampleRayDirection;

        // Creating the random ray
        Ray lightSampleRay = Ray(pos, lightSampleRayDirection, false, 10000., vec3(0.), 0., 0, -1);

        // Accumulating the light samples
        finalLight += fireLightSampleRay(lightSampleRay, triHit, sphereHit) / 10.;
    }

    return finalLight;
}





vec3 fireLightSampleRay(Ray ray, int skipTri, int skipSphere)
{
    vec3 finalColor = vec3(1.0);
    for (int i = 0; i < 10; i++) {
        Intersection intersection = getAllIntersections(ray, skipTri, skipSphere);

        // Hit the sky
        if (!intersection.intersected) {
            return finalColor * getGlobalIllumination(ray.dir);
        }
        
        // Hit an object with emmissive behaviour
        if (intersection.emmissiveness > 0.0) {
            return finalColor * intersection.emmissiveness * intersection.color;
        }

        // Hit a regular object
        finalColor *= intersection.color;

        // Continue reflecting
        float normalRayDirDot = dot(intersection.normal, ray.dir);
        vec3 normal = sign(normalRayDirDot) * intersection.normal;
        vec3 reflectedRayDir = normalize(ray.dir + normal * -2. * dot(ray.dir, normal));
        ray.pos = ray.pos + EPSILON * reflectedRayDir;
        ray.dir = reflectedRayDir;
    }
}









Intersection getAllIntersections(Ray ray, int skipTri, int skipSphere)
{
    Intersection closestIntersection;
    closestIntersection.depth = 1000.;
    closestIntersection.intersected = false;
    closestIntersection.closestTriHit = -1;

    // Checking triangle ray hits 
    for (int j = 0; j < NUM_TRIANGLES; j++)
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
    for (int j = 0; j < NUM_SPHERES; j++)
    {
        // Skip already hit tri
        if (j == skipSphere) continue;

        // Calculating this sphere's intersection
        vec3 o_c = ray.pos - spheres[j].pos; // (o-c)
        float half_b = dot(ray.dir, o_c);
        float b = 2.0 * half_b;
        float c = dot(o_c, o_c) - spheres[j].radius * spheres[j].radius;

        float det = b * b - 4 * c;

        Intersection isec = Intersection(false, 0, vec3(.0), -1, -1, vec3(0.), vec3(0.), .0, .0, .0, .0);

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
            isec.emmissiveness = materials[spheres[j].material].emmissiveness;
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







vec3 getGlobalIllumination(vec3 direction)
{
    // Calculating sky color
    vec3 up = vec3(0., 1., 0.);
    float t = dot(up, direction) + 0.6;
    vec3 skyColor = skyboxColorTop * (t)+skyboxColorHorizon * (1. - t);

    vec3 finalColor = skyColor;


    // Rendering each directional light as a sort of sun, by doing the final color dot the -direction, 
    // to calculate how much the ray is going into the sun
    for (int i = 0; i < NUM_DIR_LIGHTS; i++)
    {
        t = dot(direction, -dirLights[i].dir);
        float threshold = 0.98f;
        if (t > threshold)
        {
            // Normalize (threshold, 1.0] to (0.0, 1.0]
            t = (t - threshold) / (1. - threshold);

            //finalColor = dirLights[i].color * (t)+finalColor * (1. - t);
        }
    }
    return finalColor;
}





Intersection triangleIntersection(Tri tri, Ray ray)
{
    Intersection i;
    i.intersected = false;

    const float epsilon = 0.0000001;
    /*
    vec3 toVertex = normalize(tri.v1.xyz - ray.pos);
    if (dot(ray.dir, toVertex) < 0.9999f)
    {
        return i;
    }*/

    // Edges 1 and 2
    vec3 e1, e2;
    e1 = tri.v2.xyz - tri.v1.xyz;
    e2 = tri.v3.xyz - tri.v1.xyz;

    vec3 h = cross(ray.dir, e2);
    float a = dot(e1, h);

    if (a > -epsilon && a < epsilon)
    {
        return i;
    }
    float f = 1. / a;
    vec3 s = ray.pos - (tri.v1.xyz + meshes[tri.mesh].position);
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
        i.emmissiveness = materials[meshes[tri.mesh].material].emmissiveness;
        return i;
    }
    return i;
}