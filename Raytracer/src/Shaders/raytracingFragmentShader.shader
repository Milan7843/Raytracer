#version 460 core
out vec4 FragColor;

in vec2 pixelPos;

uniform vec3 cameraPosition;
uniform vec3 cameraRotation;
uniform vec2 screenSize;

float i = 0.;
float dstThreshold = 0.005;
float fov = 70;

vec3 skyboxColorHorizon = vec3(1., 0.7, 0.);
vec3 skyboxColorTop = vec3(0.45, 0.95, 0.85);


struct Ray
{
    vec3 pos;
    vec3 dir;
    bool hit;
    float closestDst;
    vec3 endColor;
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
    Sphere(vec3(0., 1., 0.), 1.2, vec3(0.3, 0.6, 0.6), 4.0, 0.04),
    Sphere(vec3(0., 2., 2.), 1.0, vec3(1.0, 0.3, 1.0), 0.001, 0.0),
    Sphere(vec3(1., 2., -2.), 0.5, vec3(0.0, 0.8, 1.0), 0.0, 0.0)
);
float sphereDst(Sphere sph, vec3 pos);
vec3 getSphereNormal(Sphere sph, vec3 pos);


// Triangle
struct Tri
{
    vec3 v1;
    vec3 v2;
    vec3 v3;
    vec3 normal;
};
#DEFINE NUM_TRAINGLES $numTriangles
uniform Tri triangles[$numTriangles]



struct Intersection
{
    bool intersected;
    vec3 pos;
};

Intersection triangleIntersection(Tri tri, Ray ray);





Ray fireRay(vec3 pos, vec3 direction);

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


    Ray ray = fireRay(cameraPosition, dir);




    FragColor = vec4(ray.endColor, 1.);
}


Ray fireRay(vec3 pos, vec3 direction)
{
    Ray ray;
    ray.pos = pos;
    ray.dir = direction;


    vec3 up = vec3(0., 1., 0.);
    float t = dot(up, ray.dir) + 0.4;
    ray.endColor += skyboxColorTop * (t)+skyboxColorHorizon * (1. - t);
    

    return ray;
}

Intersection triangleIntersection(Tri tri, Ray ray)
{
    Intersection i;
    i.intersected = false;

    const float epsilon = 0.0000001;

    // Edges 1 and 2
    vec3 e1, e2;
    e1 = tri.v2 - tri.v1;
    e2 = tri.v3 - tri.v1;

    vec3 h = cross(ray.dir, e2);
    float a = dot(e1, h);

    if (a > -epsilon && a < epsilon)
    {
        // Ray is parallel to this tri
        Intersection i;
        i.intersected = false;
        return i;
    }
    float f = 1. / a;
    vec3 s = ray.pos - tri.v1;
    float u = f * dot(s, h);
    if (u < 0.0 || u > 1.)
    {
        return i;
    }
    vec3 q = cross(s, e1);
    float v = f * dot(ray.dir, q);
    if (v < 0.0 || v > 1.)
    {
        return i;
    }

    float t = f * dot(e2, q);
    if (t > epsilon)
    {
        // Ray intersection
        i.intersected = true;
        i.pos = ray.pos + ray.dir * t;
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