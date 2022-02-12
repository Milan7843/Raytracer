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

void Mesh::writeToShader(Shader* shader)
{
    int j = 0;

    // Setting the models position
    shader->setVector3(("meshes[" + std::to_string(shaderMeshIndex) + "].position").c_str(), vec3ToGLSLVec3(position));

    for (unsigned int i = 0; i < indices.size(); i += 3)
    {
        // Setting reference to mesh
        shader->setInt(("triangles[" + std::to_string(j) + "].mesh").c_str(), shaderMeshIndex);

        // Setting the vertex positions
        shader->setVector3(("triangles[" + std::to_string(j) + "].v1").c_str(), vec3ToGLSLVec3(vertices[indices[i]].position));
        shader->setVector3(("triangles[" + std::to_string(j) + "].v2").c_str(), vec3ToGLSLVec3(vertices[indices[static_cast<unsigned __int64>(i) + 1]].position));
        shader->setVector3(("triangles[" + std::to_string(j) + "].v3").c_str(), vec3ToGLSLVec3(vertices[indices[static_cast<unsigned __int64>(i) + 2]].position));

        // Calculating the normal and setting it
        glm::vec3 ab = vertices[indices[static_cast<unsigned __int64>(i) + 2]].position - vertices[indices[i + 0]].position;
        glm::vec3 ac = vertices[indices[static_cast<unsigned __int64>(i) + 1]].position - vertices[indices[i + 0]].position;
        glm::vec3 normal = glm::normalize(glm::cross(ab, ac));
        shader->setVector3(("triangles[" + std::to_string(j) + "].normal").c_str(), vec3ToGLSLVec3(normal));

        // Setting reflectiveness
        shader->setFloat(("triangles[" + std::to_string(j) + "].reflectiveness").c_str(), 0.99f);
        srand(i);

        //shader->setVector3(("triangles[" + std::to_string(j) + "].color").c_str(), glm::vec3((std::rand() % 100) /100.0f));
        shader->setVector3(("triangles[" + std::to_string(j) + "].color").c_str(), glm::vec3(1.0f));
        j++;
    }
}

void Mesh::writePositionToShader(Shader* shader)
{
    shader->setVector3(("meshes[" + std::to_string(shaderMeshIndex) + "].position").c_str(), vec3ToGLSLVec3(position));
}

glm::vec3 Mesh::vec3ToGLSLVec3(glm::vec3 v)
{
    return glm::vec3(v.z, v.y, v.x);
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