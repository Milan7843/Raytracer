#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, unsigned int startIndex)
    : shaderArraybeginIndex(startIndex)
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
    for (unsigned int i = 0; i < indices.size(); i += 3)
    {
        shader->setVector3(("triangles[" + std::to_string(j) + "].v1").c_str(), vertices[indices[i]].position);
        shader->setVector3(("triangles[" + std::to_string(j) + "].v2").c_str(), vertices[indices[i+1]].position);
        shader->setVector3(("triangles[" + std::to_string(j) + "].v3").c_str(), vertices[indices[i+2]].position);
        glm::vec3 ab = vertices[indices[i + 2]].position - vertices[indices[i + 0]].position;
        glm::vec3 ac = vertices[indices[i + 1]].position - vertices[indices[i + 0]].position;
        glm::vec3 normal = glm::normalize(glm::cross(ab, ac));
        shader->setVector3(("triangles[" + std::to_string(j) + "].v1").c_str(), normal);
        shader->setFloat(("triangles[" + std::to_string(j) + "].reflectiveness").c_str(), 1.0f);
        shader->setVector3(("triangles[" + std::to_string(j) + "].color").c_str(), glm::vec3(1.0f));
        j++;
    }
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

void Mesh::draw()
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}