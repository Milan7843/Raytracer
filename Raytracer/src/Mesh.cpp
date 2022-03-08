#include "Mesh.h"

// Static variable
unsigned int Mesh::meshCount = 0;

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, unsigned int startIndex, unsigned int meshIndex)
    : shaderArraybeginIndex(startIndex), shaderMeshIndex(meshIndex)
{
    this->vertices = vertices;
    this->indices = indices;
    setupMesh();
    std::cout << indices.size() << std::endl;
}
Mesh::~Mesh()
{

}

int Mesh::getTriangleSize()
{
    return sizeof(Triangle);
}

void Mesh::writeToShader(Shader* shader, unsigned int ubo)
{
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, triangles.size() * sizeof(Triangle), &triangles[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    int j = 0;

    // Setting the models position
    shader->setVector3(("meshes[" + std::to_string(shaderMeshIndex) + "].position").c_str(), vec3ToGLSLVec3(position));
}

void Mesh::writePositionToShader(Shader* shader)
{
    shader->setVector3(("meshes[" + std::to_string(shaderMeshIndex) + "].position").c_str(), vec3ToGLSLVec3(position));
}

glm::vec3 Mesh::vec3ToGLSLVec3(glm::vec3 v)
{
    return glm::vec3(v.z, v.y, v.x);
}
glm::vec4 Mesh::vec3ToGLSLVec4(glm::vec3 v)
{
    return glm::vec4(v.z, v.y, v.x, 0.0f);
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Setting up the triangles in the UBO
    
    for (unsigned int i = 0; i < vertices.size(); i += 3)
    {
        Triangle tri{};
        tri.v1 = vec3ToGLSLVec4(vertices[i].position);
        tri.v2 = vec3ToGLSLVec4(vertices[i+1].position);
        tri.v3 = vec3ToGLSLVec4(vertices[i+2].position);
        tri.color = glm::vec3(1.0f);
        glm::vec3 ab = vertices[indices[static_cast<unsigned __int64>(i) + 2]].position - vertices[indices[i + 0]].position;
        glm::vec3 ac = vertices[indices[static_cast<unsigned __int64>(i) + 1]].position - vertices[indices[i + 0]].position;
        glm::vec3 normal = glm::normalize(glm::cross(ab, ac));
        tri.normal = glm::vec4(normal, 0.0f);
        tri.mesh = shaderMeshIndex;
        tri.reflectiveness = 0.9f;
        triangles.push_back(tri);
    }
}

void Mesh::draw(Shader* shader)
{
    glBindVertexArray(VAO);

    // Model matrix
    glm::mat4 model = glm::mat4(1.0f);
    // Translation of model matrix
    model = glm::translate(model, position);
    shader->setMat4("model", model);

    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}