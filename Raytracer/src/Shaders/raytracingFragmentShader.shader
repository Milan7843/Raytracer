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

vec3 skyboxColorHorizon = vec3(1., 0.7, 0.);
vec3 skyboxColorTop = vec3(0.45, 0.95, 0.85);

#define MAX_REFLECTIONS 50

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
    vec3 color;
    float reflectiveness;
    float fuzziness;
};
Sphere spheres[3] = Sphere[3](
    //     Pos                  Size    Color                   Reflectiveness      Fuzziness
    Sphere(vec3(0., 1., 0.),    1.2,    vec3(0.3, 0.6, 0.6),    4.0,                0.04),
    Sphere(vec3(0., 2., 2.),    1.0,    vec3(1.0, 0.3, 1.0),    0.001,              0.0),
    Sphere(vec3(1., 2., -2.),   0.5,    vec3(0.0, 0.8, 1.0),    0.0,                0.0)
);
float sphereDst(Sphere sph, vec3 pos);
vec3 getSphereNormal(Sphere sph, vec3 pos);

// LIGHTS
struct PointLight
{
    vec3 pos;
    vec3 color;
    float intensity;
};
PointLight pointLights[2] = PointLight[2](
    //         Pos                  Color                   Intensity  
    PointLight(vec3(0., -2., 2.),   vec3(1.0, 1.0, 0.0),    3f),
    PointLight(vec3(0., 2., -2.),   vec3(1.0, 0.0, 0.0),    1f)
);

struct DirLight
{
    vec3 dir;
    vec3 color;
    float intensity;
};
DirLight dirLights[1] = DirLight[1](
    //         Pos                  Color                   Intensity
    DirLight(vec3(1., -1., 0.),     vec3(0.0, 0.0, 1.0),    1.)
);

// Mesh
struct Mesh
{
    vec3 position;
};
#define NUM_MESHES $numMeshes
uniform Mesh meshes[NUM_MESHES];


struct Intersection
{
    bool intersected;
    float depth;
    vec3 pos;
    int closestTriHit;
};

Intersection triangleIntersection(Tri tri, Ray ray);

Intersection getAllIntersections(Ray ray, int skipTri);

Ray fireRay(vec3 pos, vec3 direction, bool reflect);

vec3 calculateLights(vec3 pos, Tri tri, int triHit);

float rand(float seed)
{
    return fract(sin(seed) * 10000.);
}



void main()
{
    vec2 pixelPos = vec2((pixelPos.x / 2. + 0.5) * screenSize.x - 0.5, (pixelPos.y / 2. + 0.5) * screenSize.y - 0.5);
    float d = 1 / tan(radians(fov) / 2);
    vec3 dir;
    float aspect_ratio = screenSize.x / screenSize.y;

    dir.x = aspect_ratio * (2 * pixelPos.x / screenSize.x) - aspect_ratio;
    dir.y = (2 * pixelPos.y / screenSize.y) - 1.;
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
        Intersection closestIntersection;
        closestIntersection.depth = 1000.;
        closestIntersection.intersected = false;
        int closestTriHit = -1;

        // Checking triangle ray hits 
        for (int j = 0; j < NUM_TRIANGLES; j++)
        {
            Intersection isec = triangleIntersection(triangles[j], ray);
            if (isec.intersected && isec.depth < closestIntersection.depth)
            {
                closestIntersection = isec;
                closestTriHit = j;
            }
        }


        // Check for hit
        if (closestTriHit == -1)
        {
            // Calculating air color
            vec3 up = vec3(0., 1., 0.);
            float t = dot(up, ray.dir) + 0.4;
            ray.finalColor += skyboxColorTop * (t)+skyboxColorHorizon * (1. - t);
            break;
        }
        else
        {
            if (triangles[closestTriHit].reflectiveness > 0.0 && reflect)
            {
                ray.finalColor += (1.0 - triangles[closestTriHit].reflectiveness) * triangles[closestTriHit].color;
                //ray.dir = reflect(ray.dir, triangles[closestTriHit].normal.xyz);
                ray.dir += triangles[closestTriHit].normal.xyz * -2. * dot(ray.dir, triangles[closestTriHit].normal.xyz);
                ray.pos = closestIntersection.pos + 0.0001f * ray.dir;
                continue; // reflecting
            }
            else
            {
                if (reflect)
                {
                    ray.finalColor = calculateLights(closestIntersection.pos, triangles[closestTriHit], closestTriHit);
                }
                else
                {
                    ray.finalColor = triangles[closestTriHit].color;
                }
                ray.hit = true;
                break;
            }
        }
    }

    return ray;
}

vec3 calculateLights(vec3 pos, Tri tri, int triHit)
{
    vec3 finalLight = vec3(0.);

    /* POINT LIGHTS */
    for (int i = 0; i < pointLights.length; i++)
    {
        vec3 dir = pointLights[i].pos - pos;
        dir = normalize(dir);

        // Doing ray trace light
        Ray ray;
        ray.pos = pos;
        ray.dir = dir;

        Intersection closestIntersection = getAllIntersections(ray, triHit);

        // Check for hit
        if (closestIntersection.closestTriHit == -1)
        {
            float intensity = min(
                (1. / (dir.x * dir.x + dir.y * dir.y + dir.z * dir.z)) 
                * dot(-dir, tri.normal),
                1.);

            finalLight += intensity * pointLights[i].color * pointLights[i].intensity;
        }
        else
        {
            // In shadow for this light
        }
    }

    /* DIRECTIONAL LIGHTS */
    for (int i = 0; i < dirLights.length; i++)
    {
        vec3 dir = -dirLights[i].dir;
        dir = normalize(dir);

        // Doing ray trace light
        Ray ray;
        ray.pos = pos;
        ray.dir = dir;


        Intersection closestIntersection = getAllIntersections(ray, triHit);


        // Check for hit
        if (closestIntersection.closestTriHit == -1)
        {
            float intensity = min(
                dot(-dir, tri.normal),
                1.);

            finalLight += intensity * dirLights[i].color * dirLights[i].intensity;
        }
        else
        {
            // In shadow for this light
        }
    }

    return tri.color * finalLight;
}

Intersection getAllIntersections(Ray ray, int skipTri)
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