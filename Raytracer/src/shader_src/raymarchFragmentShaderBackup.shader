#version 460 core
out vec4 FragColor;

in vec2 pixelPos;

uniform vec3 cameraPosition;
uniform vec3 cameraRotation;
uniform vec2 screenSize;

float i = 0.;
float dstThreshold = 0.005;
float fov = 70;



vec3 floorColor = vec3(0.4);
vec3 skyboxColorHorizon = vec3(1., 0.7, 0.);
vec3 skyboxColorTop = vec3(0.45, 0.95, 0.85);
vec3 floorColor1 = vec3(0.37, 0.55, 0.8);
vec3 floorColor2 = vec3(0.82, 0.4, 0.21);

// POINT LIGHT
struct PointLight
{
    vec3 pos;
};
PointLight pointLights[1] = PointLight[1](
    //         Pos
    PointLight(vec3(5., 5., 5.))
    );

// SPHERE
struct Sphere
{
    vec3 pos;
    float radius;
    vec3 color;
    float reflectiveness;
    float fuzziness;
    bool reflected;
};
Sphere spheres[3] = Sphere[3](
    //     Pos                  Size    Color                   Reflectiveness      Fuzziness
    Sphere(vec3(0., 1., 0.), 1.2, vec3(0.3, 0.6, 0.6), 4.0, 0.04, false),
    Sphere(vec3(0., 2., 2.), 1.0, vec3(1.0, 0.3, 1.0), 0.001, 0.0, false),
    Sphere(vec3(1., 2., -2.), 0.5, vec3(0.0, 0.8, 1.0), 0.0, 0.0, false)
    );
float sphereDst(Sphere sph, vec3 pos);
vec3 getSphereNormal(Sphere sph, vec3 pos);

// BOX
struct Box
{
    vec3 pos;
    vec3 size;
    vec3 color;
    float reflectiveness;
    bool reflected;
};
Box boxes[1] = Box[1](
    //  Pos                          Size                Color                   Reflectiveness
    Box(vec3(-1., 3., 2.), vec3(1., 0.5, 1.), vec3(0.3, 0.6, 0.6), 0.1, false)
    );
float boxDst(Box box, vec3 pos);
vec3 getBoxNormal(Box box, vec3 pos);

float planeDst(vec3 pos);


void setAllNonReflected();


struct RayResult
{
    bool hit;
    vec3 endPos;
    float closestDst;
    vec3 endColor;
    float timesReflected;
    int objectHitType;
    int closestObjIndex;
};



RayResult fireRay(vec3 pos, vec3 direction);

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


    RayResult ray = fireRay(cameraPosition, dir);



    if (ray.hit)
    {
        switch (ray.objectHitType)
        {
        case 0:
        ray.endColor += spheres[ray.closestObjIndex].color;
        break;
        case 1:
        ray.endColor += boxes[ray.closestObjIndex].color;
        break;
        }
        // Shadows
        float lightMultiplier = 0.3;
        for (int i = 0; i < pointLights.length(); i++)
        {
            vec3 shadowRayDir = normalize(pointLights[i].pos - ray.endPos);
            RayResult shadowRay = fireRay(ray.endPos + shadowRayDir * 0.1, shadowRayDir);
            if (!shadowRay.hit && shadowRay.timesReflected == 0.0)
            {
                lightMultiplier += 1;
            }
        }
        ray.endColor *= lightMultiplier;
    }
    else
    {
        vec3 up = vec3(0., 1., 0.);
        float t = dot(up, dir) + 0.4;
        ray.endColor += skyboxColorTop * (t)+skyboxColorHorizon * (1. - t);
    }

    ray.endColor = ray.endColor / (ray.timesReflected + 1.);


    FragColor = vec4(ray.endColor, 1.);
}


RayResult fireRay(vec3 pos, vec3 direction)
{
    int closestObjIndex = -1;

    int objectHitType = 0;

    bool hit = false;
    float timesReflected = 0;
    vec3 endColor;

    vec3 rayPos = pos;

    vec3 dir = direction;

    float totalClosestDst = 100;

    for (int i = 0; i < 1000; i++)
    {
        float closestDst = 100;

        for (int j = 0; j < spheres.length(); j++)
        {
            if (!spheres[j].reflected)
            {
                float dst = sphereDst(spheres[j], rayPos);
                if (dst < closestDst)
                {
                    closestDst = dst;
                    closestObjIndex = j;
                    if (dst < totalClosestDst)
                    {
                        totalClosestDst = dst;
                    }
                }
                if (dst < dstThreshold)
                {
                    if (spheres[j].reflectiveness > 0.)
                    {
                        vec3 normal = getSphereNormal(spheres[j], rayPos);
                        dir += normal * -2. * dot(dir, normal);
                        vec3 diff = vec3(1.);
                        if (spheres[j].fuzziness != 0.0)
                        {
                            diff =
                                normalize(vec3(spheres[j].fuzziness * rand(dir.x + pixelPos.x + dir.y + pixelPos.y * 14),
                                    spheres[j].fuzziness * rand(dir.x + pixelPos.x + dir.y + pixelPos.y * 27),
                                    spheres[j].fuzziness * rand(dir.x + pixelPos.x + dir.y + pixelPos.y * 45)
                                ));
                        }
                        float offset = 1. - dot(dir, diff);
                        dir = normalize(dir + spheres[j].fuzziness * diff);



                        // All object can reflect again except the one which just reflected
                        setAllNonReflected();
                        spheres[j].reflected = true;
                        timesReflected += spheres[j].reflectiveness;
                        endColor += spheres[j].color * spheres[j].reflectiveness;
                    }
                    else
                    {
                        hit = true;
                        objectHitType = 0;
                        break;
                    }
                }
            }
        }

        for (int j = 0; j < boxes.length(); j++)
        {
            if (!boxes[j].reflected)
            {
                float dst = boxDst(boxes[j], rayPos);
                if (dst < closestDst)
                {
                    closestDst = dst;
                    closestObjIndex = j;
                }
                if (dst < dstThreshold)
                {
                    if (boxes[j].reflectiveness > 0.)
                    {
                        vec3 normal = getBoxNormal(boxes[j], rayPos);
                        dir += normal * -2. * dot(dir, normal);
                        setAllNonReflected();
                        boxes[j].reflected = true;
                        timesReflected += boxes[j].reflectiveness;
                        endColor += boxes[j].color * boxes[j].reflectiveness;
                    }
                    else
                    {
                        hit = true;
                        objectHitType = 1;
                        break;
                    }
                }
            }
        }

        // Floor
        float dst = planeDst(rayPos);
        if (dst < closestDst)
        {
            closestDst = dst;
        }
        if (dst < dstThreshold)
        {
            if (floor(mod(rayPos.x, 2)) == floor(mod(rayPos.z, 2))) endColor += floorColor1;
            else endColor += floorColor2;
            objectHitType = -1;
            hit = true;
            break;
        }

        if (closestDst > 40)
        {
            break;
        }

        rayPos += dir * closestDst;
    }

    return RayResult(hit, rayPos, totalClosestDst, endColor, timesReflected, objectHitType, closestObjIndex);
}

float triangleDistance(vec3 p, Tri tri)
{
    vec3 ba = tri.v2 - tri.v1; vec3 pa = p - tri.v1;
    vec3 cb = tri.v3 - tri.v2; vec3 pb = p - tri.v2;
    vec3 ac = tri.v1 - tri.v3; vec3 pc = p - tri.v3;
    vec3 nor = cross(ba, ac);

    return sqrt(
        (sign(dot(cross(ba, nor), pa)) +
            sign(dot(cross(cb, nor), pb)) +
            sign(dot(cross(ac, nor), pc)) < 2.0)
        ?
        min(min(
            dot2(ba * clamp(dot(ba, pa) / dot2(ba), 0.0, 1.0) - pa),
            dot2(cb * clamp(dot(cb, pb) / dot2(cb), 0.0, 1.0) - pb)),
            dot2(ac * clamp(dot(ac, pc) / dot2(ac), 0.0, 1.0) - pc))
        :
        dot(nor, pa) * dot(nor, pa) / dot2(nor));
}


void setAllNonReflected()
{
    for (int i = 0; i < spheres.length(); i++)
    {
        spheres[i].reflected = false;
    }
}

float planeDst(vec3 pos)
{
    return pos.y;
}

//SPHERE FUNCTIONS
float sphereDst(Sphere sph, vec3 pos)
{
    return length(sph.pos - pos) - sph.radius;
}
vec3 getSphereNormal(Sphere sph, vec3 pos)
{
    return normalize(sph.pos - pos);
}

//BOX FUNCTIONS
float boxDst(Box box, vec3 pos) // Distance
{
    vec3 q = abs(pos - box.pos) - box.size;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}
vec3 getBoxNormal(Box box, vec3 pos) // Normal
{
    return normalize(box.pos - pos);
}