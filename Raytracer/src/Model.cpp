#include "Model.h"

#include "Scene.h"

Model::Model(std::string& name, std::vector<unsigned int>& meshMaterialIndices,
	const std::string& path, unsigned int* meshCount, unsigned int* triangleCount,
	unsigned int MAX_MESH_COUNT)
	: path(path)
	, Object()
	, ContextMenuSource()
{
	this->name = name;

	// Checking how many triangles this model adds
	unsigned int triangleCountBefore{ *triangleCount };
	loadModel(path, meshMaterialIndices, meshCount, triangleCount, MAX_MESH_COUNT);
	unsigned int triangleCountAfter{ *triangleCount };

	this->triangleCount = triangleCountAfter - triangleCountBefore;

	setType(MODEL);
}

Model::Model(unsigned int meshMaterialIndex,
	const std::string& path, unsigned int* meshCount, unsigned int* triangleCount,
	unsigned int MAX_MESH_COUNT)
	: path(path)
	, Object()
	, ContextMenuSource()
{
	// Automatically setting name based on model name
	this->name = path.substr(path.find_last_of('/') + 1, path.find_last_of('.') - (path.find_last_of('/') + 1));

	loadModel(path, meshMaterialIndex, meshCount, triangleCount, MAX_MESH_COUNT);
	setType(MODEL);
}

Model::~Model()
{
}

void Model::writeDataToStream(std::ofstream& filestream)
{
	// Writing basic object data
	Object::writeDataToStream(filestream);

	// Writing specific model data
	filestream << path << "\n";

	// Writing the number of submeshes, then all mesh's material indices to filestream
	filestream << meshes.size() << "\n";
	for (Mesh& mesh : meshes)
	{
		filestream << *mesh.getMaterialIndexPointer() << "\n";
	}

	filestream << meshes.size() << "\n";
	for (Mesh& mesh : meshes)
	{
		mesh.writeDataToStream(filestream);
	}
}

void Model::prepareForDraw(AbstractShader* shader)
{
	// Setting transformations
	shader->setMat4("model", getTransformationMatrix());
	shader->setMat4("rotation", getRotationMatrix());
}

void Model::draw(AbstractShader* shader, Scene* scene)
{
	prepareForDraw(shader);

	// Drawing each mesh
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		shader->setInt("objectID", meshes[i].getID());

		glm::mat4 transformation{ this->getTransformationMatrix() };
		meshes[i].draw(shader, scene, transformation);
	}
}

bool Model::drawInterface(Scene& scene)
{
	bool anyPropertiesChanged{ false };

	//anyPropertiesChanged |= ImGui::InputText("Name", &getName());
	ImGui::InputText("Name", &getName());
	// Showing transformations
	anyPropertiesChanged |= ImGui::DragFloat3("Position", (float*)getPositionPointer(), 0.01f);
	anyPropertiesChanged |= ImGui::DragFloat3("Rotation", (float*)getRotationPointer(), 0.01f);
	anyPropertiesChanged |= ImGui::DragFloat3("Scale", (float*)getScalePointer(), 0.01f);

	if (anyPropertiesChanged)
	{
		setVertexDataChanged(true);
	}

	int meshIndex = 0;
	// Drawing all the meshes of this model
	for (Mesh& mesh : getMeshes())
	{
		//drawMesh(mesh, scene, materialSlotsCharArray, meshIndex++);
		anyPropertiesChanged |= mesh.drawInterface(scene);
	}

	// If anything changed, no shader will have the updated data
	if (anyPropertiesChanged)
	{
		clearShaderWrittenTo();
	}

	return anyPropertiesChanged;
}

bool Model::writeToShader(AbstractShader* shader, unsigned int ssbo)
{
	if (hasWrittenToShader(shader))
	{
		return false;
	}

	std::cout << "m " << getPosition().x << ", " << getPosition().y << ", " << getPosition().z << std::endl;

	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].writeToShader(shader, ssbo, getTransformationMatrix() * meshes[i].getTransformationMatrix());
	}

	markShaderAsWrittenTo(shader);

	return true;
}

void Model::onDeleteMaterial(unsigned int index)
{
	// Verifying each submesh
	for (Mesh& mesh : meshes)
	{
		mesh.onDeleteMaterial(index);
	}
}

void Model::renderContextMenuItems(Scene& scene)
{
	// Draw some text
	ImGui::Text((getName()).c_str());

	// Add a separator line
	ImGui::Separator();

	if (ImGui::MenuItem("Delete"))
	{
		// TODO when delete is called, the click may still select one of its meshes, which causes an error
		scene.deleteModel(this->getID());
	}
	if (ImGui::MenuItem("Option 2"))
	{
		// Handle option 2 selection
	}
}

void Model::updateChildPointers()
{
	// Updating each submesh
	for (Mesh& mesh : meshes)
	{
		mesh.setModel(this);
	}
}

void Model::resetShaderIndices(unsigned int* triangleCount, unsigned int* meshCount)
{
	for (Mesh& mesh : meshes)
	{
		// Setting the shader index
		mesh.shaderArraybeginIndex = *triangleCount;
		mesh.setShaderMeshIndex(*meshCount);

		// Incrementing the triangle and mesh count to include the last mesh
		*triangleCount += mesh.getTriangleCount();
		(*meshCount)++;
	}
}

std::vector<Mesh>& Model::getMeshes()
{
	return meshes;
}

bool Model::isVertexDataChanged()
{
	return vertexDataChanged;
}

void Model::setVertexDataChanged(bool newValue, bool alsoSetMeshes)
{
	vertexDataChanged = newValue;

	if (!vertexDataChanged)
	{
		return;
	}

	if (!alsoSetMeshes)
	{
		return;
	}

	// If this model changed, all submeshes must have also changed
	for (Mesh& mesh : meshes)
	{
		mesh.setVertexDataChanged(true);
	}
}

void Model::loadModel(std::string path, std::vector<unsigned int>& meshMaterialIndices, unsigned int* meshCount, unsigned int* triangleCount, unsigned int MAX_MESH_COUNT)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);

	// Checking for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		Logger::logError(std::string("Assimp error: model loading failed:") + importer.GetErrorString());
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	unsigned int* meshIndex = new unsigned int(0);

	processNode(scene->mRootNode, scene, meshMaterialIndices, meshCount, meshIndex, triangleCount, MAX_MESH_COUNT);
}

void Model::loadModel(std::string path, unsigned int meshMaterialIndex, unsigned int* meshCount, unsigned int* triangleCount, unsigned int MAX_MESH_COUNT)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);

	// Checking for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		Logger::logError(std::string("Assimp error: model loading failed:") + importer.GetErrorString());
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene, meshMaterialIndex, meshCount, triangleCount, MAX_MESH_COUNT);
}

float Model::getAppropriateCameraFocusDistance()
{
	// TODO make this implementation more precise
	float maxDistance{ 0.0f };

	for (Mesh& mesh : meshes)
	{
		maxDistance = glm::max(maxDistance, mesh.getAppropriateCameraFocusDistance());
	}

	return maxDistance;
}

void Model::processNode(aiNode* node, const aiScene* scene, std::vector<unsigned int>& meshMaterialIndices, unsigned int* meshCount, unsigned int* meshIndex, unsigned int* triangleCount,
	unsigned int MAX_MESH_COUNT)
{
	// Reading mesh data for each mesh
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// Reached maximum number of meshes
		if (*meshCount >= MAX_MESH_COUNT)
		{
			// Stop loading more meshes
			return;
		}

		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene, meshMaterialIndices[*meshIndex], *meshCount, triangleCount));

		(*meshCount)++;
		(*meshIndex)++;
	}

	// Reading all the data from all children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene, meshMaterialIndices, meshCount, meshIndex, triangleCount, MAX_MESH_COUNT);
	}
}

void Model::processNode(aiNode* node, const aiScene* scene, unsigned int meshMaterialIndex, unsigned int* meshCount, unsigned int* triangleCount,
	unsigned int MAX_MESH_COUNT)
{
	// Reading mesh data for each mesh
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// Reached maximum number of meshes
		if (*meshCount >= MAX_MESH_COUNT)
		{
			// Stop loading more meshes
			return;
		}

		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene, meshMaterialIndex, *meshCount, triangleCount));

		(*meshCount)++;
	}

	// Reading all the data from all children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene, meshMaterialIndex, meshCount, triangleCount, MAX_MESH_COUNT);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene, unsigned int meshCount, unsigned int* triangleCount)
{
	int beginTriangleCount = *triangleCount;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	glm::vec4 meshPositionVec4{ glm::vec4(0.0f) };

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		// New vertex
		Vertex vertex;

		vertex.position = aiVector3DToGLMVec4(mesh->mVertices[i]);

		// Adding normals if possible
		if (mesh->HasNormals())
		{
			vertex.normal = aiVector3DToGLMVec4(mesh->mNormals[i]);
		}

		// Putting the new vertex into the vertices vector
		vertices.push_back(vertex);

		// Calculating the average vertex position
		meshPositionVec4 += vertex.position;
	}

	glm::vec3 meshPosition{ glm::vec3(meshPositionVec4.x, meshPositionVec4.y, meshPositionVec4.z) / (float)mesh->mNumVertices };

	// Getting indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// Pushing each index of the face
		if (face.mNumIndices < 3) continue;
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
		// Another triangle has been added

		// This variable is used to generate start indices for mesh's triangles
		(*triangleCount)++;
	}

	std::string meshName{ mesh->mName.C_Str() };

	return Mesh(meshName, vertices, indices, meshPosition, beginTriangleCount, meshCount, mesh->mMaterialIndex, this->getID(), this);
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene, unsigned int materialIndex, unsigned int meshCount, unsigned int* triangleCount)
{
	int beginTriangleCount = *triangleCount;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	glm::vec4 meshPositionVec4{ glm::vec4(0.0f) };

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		// New vertex
		Vertex vertex;

		vertex.position = aiVector3DToGLMVec4(mesh->mVertices[i]);

		// Adding normals if possible
		if (mesh->HasNormals())
		{
			vertex.normal = aiVector3DToGLMVec4(mesh->mNormals[i]);
		}

		if (mesh->HasTextureCoords(0))
		{
			vertex.uv = aiVector3DToGLMVec4(mesh->mTextureCoords[0][i]);
			//std::cout << "uv " << vertex.uv.x << ", " << vertex.uv.y << ", " << vertex.uv.z << std::endl;
		}
		else
		{
			vertex.uv = glm::vec4(0.0f);
		}

		// Putting the new vertex into the vertices vector
		vertices.push_back(vertex);

		// Calculating the average vertex position
		meshPositionVec4 += vertex.position;
	}

	glm::vec3 meshPosition{ glm::vec3(meshPositionVec4.x, meshPositionVec4.y, meshPositionVec4.z) / (float)mesh->mNumVertices };

	std::vector<glm::vec4> tangents;
	std::vector<glm::vec4> bitangents;
	std::vector<int> bi_ti_samplesPerVertex;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		tangents.push_back(glm::vec4(0.0f));
		bitangents.push_back(glm::vec4(0.0f));
		bi_ti_samplesPerVertex.push_back(0);
	}

	// Getting indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// Pushing each index of the face
		if (face.mNumIndices < 3) continue;

		// Loading a triangle
		if (face.mNumIndices == 3)
		{
			for (unsigned int j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}

			// Another triangle has been added

			// Calculating the tangent and bitangent vectors for each vertex
			calculateTangentBitangent(
				vertices,
				face.mIndices[0], face.mIndices[1], face.mIndices[2], 
				tangents, bitangents,
				bi_ti_samplesPerVertex
			);

			calculateTangentBitangent(
				vertices,
				face.mIndices[1], face.mIndices[2], face.mIndices[0],
				tangents, bitangents,
				bi_ti_samplesPerVertex
			);

			calculateTangentBitangent(
				vertices,
				face.mIndices[2], face.mIndices[0], face.mIndices[1],
				tangents, bitangents,
				bi_ti_samplesPerVertex
			);

			// This variable is used to generate start indices for mesh's triangles
			(*triangleCount)++;
		}
		/*
		// Loading a quad: it is treated like two triangles
		else if (face.mNumIndices == 4)
		{
			// Pushing the first of the two triangles
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);

			// Pushing the second of the two triangles
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[2]);
			indices.push_back(face.mIndices[3]);

			// Two triangles were added

			// This variable is used to generate start indices for mesh's triangles
			(*triangleCount) += 2;
		}*/
		// Too big; do not add
		else {
			Logger::logError("Found a face with more than 4 vertices. This is not supported. Please provide a model consisting of quads or triangles.");
		}
	}

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		if (bi_ti_samplesPerVertex[i] == 0)
		{
			continue;
		}

		// There is at least one tangent/bitangent: take the average
		vertices[i].tangent = tangents[i] / (float)bi_ti_samplesPerVertex[i];
		vertices[i].bitangent = bitangents[i] / (float)bi_ti_samplesPerVertex[i];
		//std::cout << "tangent vertex " << i << ": " << vertices[i].tangent.x << ", " << vertices[i].tangent.y << ", " << vertices[i].tangent.z << std::endl;
		//std::cout << "bitangent vertex " << i << ": " << vertices[i].bitangent.x << ", " << vertices[i].bitangent.y << ", " << vertices[i].bitangent.z << std::endl;
	}

	std::string meshName{ mesh->mName.C_Str() };

	return Mesh(meshName, vertices, indices, meshPosition, beginTriangleCount, meshCount, materialIndex, this->getID(), this);
}


glm::vec4 Model::aiVector3DToGLMVec4(aiVector3D v)
{
	return glm::vec4(v.x, v.y, v.z, 1.0f);
}

void Model::calculateTangentBitangent(std::vector<Vertex>& vertices,
	unsigned int index1, unsigned int index2, unsigned int index3,
	std::vector<glm::vec4>& tangents,
	std::vector<glm::vec4>& bitangents,
	std::vector<int>& bi_ti_samplesPerVertex)
{
	Vertex& v1 = vertices[index1];
	Vertex& v2 = vertices[index2];
	Vertex& v3 = vertices[index3];

	// Calculate the edge vectors for the triangle
	glm::vec3 edge1 = glm::vec3(v2.position) - glm::vec3(v1.position);
	glm::vec3 edge2 = glm::vec3(v3.position) - glm::vec3(v1.position);

	// Calculate the difference in UV coordinates
	glm::vec2 deltaUV1 = glm::vec2(v2.uv) - glm::vec2(v1.uv);
	glm::vec2 deltaUV2 = glm::vec2(v3.uv) - glm::vec2(v1.uv);

	// Calculate the tangent and bitangent
	glm::vec4 tangent, bitangent;

	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	tangent.w = 0.0f;
	tangent = glm::normalize(tangent);

	bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	bitangent.w = 0.0f;
	bitangent = glm::normalize(bitangent);

	tangents[index1] += tangent;
	bitangents[index1] += bitangent;
	bi_ti_samplesPerVertex[index1]++;
}

std::ostream& operator<<(std::ostream& stream, const Model& model)
{
	// Writing this object to the stream
	stream << "[Model]" << "path: " << model.path
		<< "\nposition: (" << model.position.x << ", " << model.position.y << ", " << model.position.z << ")"
		<< "\nrotation: (" << model.rotation.x << ", " << model.rotation.y << ", " << model.rotation.z << ")"
		<< "\nscale: (" << model.scaleVector.x << ", " << model.scaleVector.y << ", " << model.scaleVector.z << ")"
		<< "\nmaterials: ";

	/* TODO make this work
	for (Mesh& mesh : model.meshes)
	{
		stream << *mesh.getMaterialIndexPointer();
	}*/

	stream << std::endl;

	return stream;
}

BVHNode* Model::getRootNode()
{
	// TODO update on mesh update
	if (isVertexDataChanged())// || true)
	{
		// Creating a BVH from the model
		this->bvhRootNode = BVHHandler::generateFromModel(*this, this->bvhRootNode);
		setVertexDataChanged(false);
	}
	return bvhRootNode;
}

unsigned int Model::getTriangleCount()
{
	return triangleCount;
}
