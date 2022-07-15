#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace CoordinateUtility
{
    inline glm::vec3 vec3ToGLSLVec3(glm::vec3 v)
    {
        return glm::vec3(v.x, v.y, v.z);
    }
    inline glm::vec4 vec4ToGLSLVec4(glm::vec3 v)
    {
        return glm::vec4(v.x, v.y, v.z, 1.0f);
    }
}