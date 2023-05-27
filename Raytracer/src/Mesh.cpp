#include "Mesh.h"

#include "Scene.h"

Mesh::Mesh(std::string& name, std::vector<Vertex> vertices, std::vector<unsigned int> indices
    , unsigned int startIndex, unsigned int meshIndex, unsigned int materialIndex)
    : name(name)
    , shaderArraybeginIndex(startIndex)
    , shaderMeshIndex(meshIndex)
    , materialIndex(materialIndex)
{
    this->vertices = vertices;
    this->indices = indices;
    setupMesh();
}

Mesh::~Mesh()
{

}

bool Mesh::drawInterface(Scene& scene)
{
    unsigned int i = 0;

    bool materialUpdated{ false };

    // Preview the currently selected name
    if (ImGui::BeginCombo((getName() + "##combo").c_str(), (*(scene.getMaterials()[*getMaterialIndexPointer()].getNamePointer())).c_str()))
    {
        // Looping over each material to check whether it was clicked;
        // If it was: select the index of the material as the material index for this mesh
        for (Material& material : scene.getMaterials())
        {
            bool thisMaterialSelected = (i == *getMaterialIndexPointer());

            // Button for selecting material
            if (ImGui::Selectable((*(scene.getMaterials()[i].getNamePointer())).c_str()))
            {
                *getMaterialIndexPointer() = i;
                materialUpdated = true;
            }

            if (thisMaterialSelected)
            {
                ImGui::SetItemDefaultFocus();
            }

            // Increment material counter
            i++;
        }

        // End this combo selector
        ImGui::EndCombo();
    }

    return materialUpdated;
}

int Mesh::getTriangleSize()
{
    return sizeof(Triangle);
}

void Mesh::writeToShader(AbstractShader* shader, unsigned int ssbo, const glm::mat4& transformation)
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    // Copying this mesh's data into the buffer at the right position
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, shaderArraybeginIndex * sizeof(Triangle), triangles.size() * sizeof(Triangle), &triangles[0]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // Setting the models position
    writePositionToShader(shader);
    //shader->setVector3(("meshes[" + std::to_string(shaderMeshIndex) + "].position").c_str(), CoordinateUtility::vec3ToGLSLVec3(position));
    shader->setInt(("meshes[" + std::to_string(shaderMeshIndex) + "].material").c_str(), materialIndex);
    shader->setMat4(("meshes[" + std::to_string(shaderMeshIndex) + "].transformation").c_str(), transformation);
}

void Mesh::writePositionToShader(AbstractShader* shader)
{
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
    for (unsigned int i = 0; i < indices.size(); i += 3)
    {
        Vertex& v1 = vertices[indices[i]];
        Vertex& v2 = vertices[indices[i + 1]];
        Vertex& v3 = vertices[indices[i + 2]];
        // static_cast<unsigned __int64>(i)
        Triangle tri{};
        tri.v1 = v1.position;
        tri.v2 = v2.position;
        tri.v3 = v3.position;

        tri.n1 = -CoordinateUtility::vec4ToGLSLVec4(v1.normal);
        tri.n2 = -CoordinateUtility::vec4ToGLSLVec4(v2.normal);
        tri.n3 = -CoordinateUtility::vec4ToGLSLVec4(v3.normal);

        tri.color = glm::vec3(1.0f);
        glm::vec3 ab = v2.position - v1.position;
        glm::vec3 ac = v3.position - v1.position;
        glm::vec3 normal = -glm::normalize(glm::cross(ab, ac));
        tri.normal = CoordinateUtility::vec3ToGLSLVec3(normal);
        tri.mesh = shaderMeshIndex;
        tri.reflectiveness = 1.0f;
        triangles.push_back(tri);
    }
}

unsigned int Mesh::getTriangleCount()
{
    return triangles.size();
}

void Mesh::setShaderMeshIndex(unsigned int shaderMeshIndex)
{
    this->shaderMeshIndex = shaderMeshIndex;

    // Setting this shader mesh index in all triangle data
    for (Triangle& tri : triangles)
    {
        tri.mesh = shaderMeshIndex;
    }
}

void Mesh::onDeleteMaterial(unsigned int index)
{
    // Deleted this mesh' material
    if (this->materialIndex == index)
    {
        this->materialIndex = 0;
        return;
    }

    // Deleted a material with an index below this mesh'
    if (index < this->materialIndex)
    {
        this->materialIndex--;
    }
}

void Mesh::draw(AbstractShader* shader, Scene* scene)
{
    // Setting up the shader for the material used by this mesh
    shader->setVector3("inputColor", scene->getMaterials()[materialIndex].color);
    shader->setInt("materialIndex", materialIndex);

    glBindVertexArray(VAO);

    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

unsigned int* Mesh::getMaterialIndexPointer()
{
    return &materialIndex;
}

std::string& Mesh::getName()
{
    return name;
}

unsigned int Mesh::getMaterialIndex() const
{
    return materialIndex;
}
