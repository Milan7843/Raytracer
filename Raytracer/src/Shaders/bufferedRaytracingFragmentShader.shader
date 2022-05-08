#version 460 core
out vec4 FragColor;

in vec2 pixelPos;

#define NUM_TRIANGLES $numTriangles
bool trisReflected[NUM_TRIANGLES];

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

//vec3 skyboxColorHorizon = vec3(1., 0.7, 0.);
vec3 skyboxColorHorizon = vec3(0.1, 0.2, 0.4);
vec3 skyboxColorTop = vec3(0.45, 0.95, 0.85);

#define MAX_REFLECTIONS 15

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
#define NUM_SPHERES $numSpheres
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

#define NUM_POINT_LIGHTS $numPointLights
uniform PointLight pointLights[NUM_POINT_LIGHTS];

struct DirLight
{
    vec3 dir;
    vec3 color;
    float intensity;
};
#define NUM_DIR_LIGHTS 1
DirLight dirLights[NUM_DIR_LIGHTS] = DirLight[](
    //         Pos                  Color                   Intensity
    DirLight(vec3(.707, -.707, 0.), vec3(1.0, 1.0, 0.9), 0.8)
    );

struct AmbientLight
{
    vec3 color;
    float intensity;
};
#define NUM_AMBIENT_LIGHTS 1
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
#define NUM_MESHES $numMeshes
uniform Mesh meshes[NUM_MESHES];

struct Material
{
    vec3 color;
    float reflectiveness;
    float transparency;
};
#define NUM_MATERIALS $numMaterials
uniform Material materials[NUM_MATERIALS];



// Pixels: holds information about the rendered pixels
#define NUM_PIXELS $numPixels
layout(std140, binding = 3) buffer Pixels
{
    vec4 pixelColors[NUM_PIXELS];
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
};

Intersection triangleIntersection(Tri tri, Ray ray);

Intersection getAllIntersections(Ray ray, int skipTri, int skipSphere);

Ray fireRay(vec3 pos, vec3 direction, bool reflect);

vec3 calculateLights(vec3 pos, vec3 normal, int triHit, int sphereHit);

float rand(float seed)
{
    return fract(sin(seed) * 10000.);
}



void main()
{
    vec2 pixelPosIndex = vec2((pixelPos.x / 2. + 0.5) * screenSize.x, (pixelPos.y / 2. + 0.5) * screenSize.y);
    int pixelIndex = int(int(pixelPosIndex.x) + int(pixelPosIndex.y) * screenSize.x);
    /*
    if (pixelIndex >= 700*1200)
    {
        FragColor = vec4(1., 0., 0., 0.);

        return;
    }
    else
    {
        FragColor = vec4(0.);

        return;
    }*/

    // Skip raytracing if there is already a pixel defined in the buffer for this position
    if (pixelColors[pixelIndex] != vec4(0.))
    {
        FragColor = pixelColors[pixelIndex];

        return;
    }

    // Taking the middle of the pixel
    pixelPosIndex = pixelPosIndex - vec2(0.5);

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

    Ray ray = fireRay(cameraPosition, dir, true);

    pixelColors[pixelIndex] = vec4(ray.finalColor, 1.);
    FragColor = vec4(ray.finalColor, 1.);
}


Ray fireRay(vec3 pos, vec3 direction, bool reflect)
{
    Ray ray;
    ray.pos = pos;
    ray.dir = direction;

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
            for (int i = 0; i < NUM_DIR_LIGHTS; i++)
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
            if (closestIntersection.reflectiveness > 0.0 && reflect && i != MAX_REFLECTIONS - 1)
            {
                // Calculating the new ray direction for a reflection
                ray.finalColor += (1.0 - closestIntersection.reflectiveness) * closestIntersection.color;
                ray.dir = normalize(ray.dir + closestIntersection.normal * -2. * dot(ray.dir, closestIntersection.normal));
                ray.pos = closestIntersection.pos + 0.0001f * ray.dir;
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
    for (int i = 0; i < NUM_POINT_LIGHTS; i++)
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
    for (int i = 0; i < NUM_DIR_LIGHTS; i++)
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
    for (int i = 0; i < NUM_AMBIENT_LIGHTS; i++)
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
    for (int j = 0; j < NUM_TRIANGLES; j++)
    {
        // Skip already hit tri
        if (j == skipTri) continue;

        Intersection isec = triangleIntersection(triangles[j], ray);
        if (isec.intersected && isec.depth < closestIntersection.depth)
        {
            closestIntersection = isec;
            closestIntersection.closestTriHit = j;
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

        Intersection isec = Intersection(false, 0, vec3(.0), -1, -1, vec3(0.), vec3(0.), .0);

        if (b > 0.0 || det < 0.0)
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
            isec.color = materials[spheres[j].material].color;
            //isec.normal = normalize(isec.pos - spheres[j].pos);
            isec.normal = normalize(spheres[j].pos - isec.pos);

            if (c < 0)
            {
                isec.color = vec3(0.);
                isec.reflectiveness = 0.;
            }
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
        return i;
    }
    return i;
}










float sphereIntersection(Sphere sph, Ray ray)
{
    return 0.0;
}
vec3 getSphereNormal(Sphere sph, vec3 pos)
{
    return vec3(0.);
}