#include "Mesh.h"

#include "Scene.h"
#include "BVH/BVHHandler.h"
#include "Model.h"


Mesh::Mesh(std::string& name, std::vector<Vertex> vertices, std::vector<unsigned int> indices, glm::vec3 position
    , unsigned int startIndex, unsigned int meshIndex, unsigned int modelID, Model* model)
    : shaderArraybeginIndex(startIndex)
    , shaderMeshIndex(meshIndex)
    , materialIndex(materialIndex)
    , modelID(modelID)
    , averageVertexPosition(position)
    , model(model)
    , Object()
    , ContextMenuSource()
{
    this->name = name;
    this->vertices = vertices;

    glm::vec4 positionVec4{ glm::vec4(position, 0.0f) };

    // Finding the size of the object
    glm::vec4 minPosition{ glm::vec4(0.0f) };
    glm::vec4 maxPosition{ glm::vec4(0.0f) };

    // If there are any vertices we use the first to set the initial values
    if (this->vertices.size() > 0)
    {
        minPosition = this->vertices[0].position - positionVec4;
        maxPosition = this->vertices[1].position - positionVec4;
    }

    // Normalzing the vertex positions
    for (Vertex& vertex : this->vertices)
    {
        vertex.position -= positionVec4;
        //minPosition.x = glm::min(vertex.position.x, minPosition.x);
        //minPosition.y = glm::min(vertex.position.y, minPosition.y);
        //minPosition.z = glm::min(vertex.position.z, minPosition.z);
        //maxPosition.x = glm::max(vertex.position.x, maxPosition.x);
        //maxPosition.y = glm::max(vertex.position.y, maxPosition.y);
        //maxPosition.z = glm::max(vertex.position.z, maxPosition.z);
        minPosition = glm::min(vertex.position, minPosition);
        maxPosition = glm::max(vertex.position, maxPosition);
    }

    // Now taking the difference between the minimum and maximum position
    this->boundingBoxSize = glm::vec3(
        maxPosition.x - minPosition.x,
        maxPosition.y - minPosition.y,
        maxPosition.z - minPosition.z
    );

    this->indices = indices;
    this->position = position;
    setupMesh();
    setType(MESH);
}

Mesh::~Mesh()
{

}

bool Mesh::drawInterface(Scene& scene)
{
    unsigned int i = 0;

    bool anyPropertiesChanged{ false };

    //anyPropertiesChanged |= ImGui::InputText("Name", &getName());
    ImGui::InputText("Name", getNamePointer());
    // Showing transformations
    anyPropertiesChanged |= ImGui::DragFloat3("Position", (float*)getPositionPointer(), 0.01f);
    anyPropertiesChanged |= ImGui::DragFloat3("Rotation", (float*)getRotationPointer(), 0.01f);
    anyPropertiesChanged |= ImGui::DragFloat3("Scale", (float*)getScalePointer(), 0.01f);

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
                anyPropertiesChanged = true;
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


    // If anything changed, that means the vertex data may have been updated
    if (anyPropertiesChanged)
    {
        setVertexDataChanged(true);
    }

    if (anyPropertiesChanged)
    {
        markUnsavedChanges();
    }

    return anyPropertiesChanged;
}

int Mesh::getTriangleSize()
{
    return sizeof(Triangle);
}

void Mesh::writeToShader(AbstractShader* shader, unsigned int ssbo, const glm::mat4& transformation)
{
    std::cout << "Writing mesh data to buffer" << std::endl;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    // Copying this mesh's data into the buffer at the right position
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, shaderArraybeginIndex * sizeof(Triangle), triangles.size() * sizeof(Triangle), &triangles[0]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // Setting the models position
    writePositionToShader(shader);
    //shader->setVector3(("meshes[" + std::to_string(shaderMeshIndex) + "].position").c_str(), CoordinateUtility::vec3ToGLSLVec3(position));
    shader->setInt(("meshes[" + std::to_string(shaderMeshIndex) + "].material").c_str(), materialIndex);
    shader->setMat4(("meshes[" + std::to_string(shaderMeshIndex) + "].transformation").c_str(), transformation * this->getTransformationMatrix());
}

void Mesh::writePositionToShader(AbstractShader* shader)
{
}

float Mesh::getAppropriateCameraFocusDistance()
{
    //std::cout << "bb " << this->boundingBoxSize.x << ", " << this->boundingBoxSize.y << ", " << this->boundingBoxSize.z << std::endl;
    return glm::max(glm::max(
        this->boundingBoxSize.x * this->getScale().x,
        this->boundingBoxSize.y * this->getScale().y),
        this->boundingBoxSize.z * this->getScale().z
    ) * 2.0f;
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
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
    glEnableVertexAttribArray(4);

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

        tri.uv1 = v1.uv;
        tri.uv2 = v2.uv;
        tri.uv3 = v3.uv;

        tri.t1 = CoordinateUtility::vec4ToGLSLVec4(v1.tangent);
        tri.t2 = CoordinateUtility::vec4ToGLSLVec4(v2.tangent);
        tri.t3 = CoordinateUtility::vec4ToGLSLVec4(v3.tangent);
        tri.b1 = CoordinateUtility::vec4ToGLSLVec4(v1.bitangent);
        tri.b2 = CoordinateUtility::vec4ToGLSLVec4(v2.bitangent);
        tri.b3 = CoordinateUtility::vec4ToGLSLVec4(v3.bitangent);

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

void Mesh::setMaterialIndex(unsigned int index)
{
    this->materialIndex = index;
}

void Mesh::draw(AbstractShader* shader, Scene* scene, glm::mat4& modelTransformation)
{
    // Setting up the shader for the material used by this mesh
    shader->setVector3("inputColor", scene->getMaterials()[materialIndex].color);
    shader->setInt("materialIndex", materialIndex);
    shader->setMat4("model", this->getTransformationMatrix() * modelTransformation);

    glBindVertexArray(VAO);

    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

unsigned int* Mesh::getMaterialIndexPointer()
{
    return &materialIndex;
}

unsigned int Mesh::getMaterialIndex() const
{
    return materialIndex;
}

unsigned int Mesh::getModelID()
{
    return modelID;
}

void Mesh::renderContextMenuItems(Scene& scene)
{
    // Draw some text
    ImGui::Text((getName()).c_str());

    // Add a separator line
    ImGui::Separator();

    if (ImGui::MenuItem("Option 1"))
    {
        // Handle option 1 selection
    }

    if (ImGui::MenuItem("Option 2"))
    {
        // Handle option 2 selection
    }
}

void Mesh::setModel(Model* model)
{
    this->model = model;
}

Model* Mesh::getModel()
{
    return this->model;
}

glm::vec3 Mesh::getAverageVertexPosition() const
{
    return averageVertexPosition;
}

glm::vec3 Mesh::getRotationPoint() const
{
    return getAverageVertexPosition();
}

bool Mesh::isVertexDataChanged()
{
    return vertexDataChanged;
}

void Mesh::setVertexDataChanged(bool newValue)
{
    vertexDataChanged = newValue;

    // Mark the model as having vertex data changed, but only this 
    getModel()->setVertexDataChanged(true, false);
}

BVHNode* Mesh::getRootNode(Model& model)
{
    // TODO update on mesh update
    if (isVertexDataChanged())// || true)
    {
        // Creating a BVH from the model
        this->bvhRootNode = BVHHandler::generateFromMesh(model, *this, bvhRootNode);
        setVertexDataChanged(false);
    }
    return bvhRootNode;
}
