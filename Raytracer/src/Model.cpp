#include "Model.h"

#include "Scene.h"

Model::Model(std::string& name, std::vector<unsigned int>& meshMaterialIndices,
	const std::string& path, unsigned int* meshCount, unsigned int* triangleCount,
	unsigned int MAX_MESH_COUNT)
	: path(path)
{
	this->name = name;

	loadModel(path, meshMaterialIndices, meshCount, triangleCount, MAX_MESH_COUNT);
}

Model::Model(unsigned int meshMaterialIndex,
	const std::string& path, unsigned int* meshCount, unsigned int* triangleCount,
	unsigned int MAX_MESH_COUNT)
	: path(path)
{
	// Automatically setting name based on model name
	this->name = path.substr(path.find_last_of('/') + 1, path.find_last_of('.') - (path.find_last_of('/') + 1));

	loadModel(path, meshMaterialIndex, meshCount, triangleCount, MAX_MESH_COUNT);
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
}

void Model::draw(AbstractShader* shader, Scene* scene)
{
	// Setting transformations
	shader->setMat4("model", getTransformationMatrix());
	shader->setMat4("rotation", getRotationMatrix());

	// Drawing each mesh
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].draw(shader, scene);
	}
}

void Model::drawInterface(Scene& scene)
{
	bool anyPropertiesChanged{ false };

	anyPropertiesChanged |= ImGui::InputText("Name", &getName());
	// Showing transformations
	anyPropertiesChanged |= ImGui::DragFloat3("Position", (float*)getPositionPointer(), 0.01f);
	anyPropertiesChanged |= ImGui::DragFloat3("Rotation", (float*)getRotationPointer(), 0.01f);
	anyPropertiesChanged |= ImGui::DragFloat3("Scale", (float*)getScalePointer(), 0.01f);

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
}

bool Model::writeToShader(AbstractShader* shader, unsigned int ssbo)
{
	if (hasWrittenToShader(shader))
	{
		return false;
	}

	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].writeToShader(shader, ssbo, getTransformationMatrix());
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

	processNode(scene->mRootNode, scene, meshMaterialIndices, meshCount, triangleCount, MAX_MESH_COUNT);
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

void Model::processNode(aiNode* node, const aiScene* scene, std::vector<unsigned int>& meshMaterialIndices, unsigned int* meshCount, unsigned int* triangleCount,
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
		meshes.push_back(processMesh(mesh, scene, meshMaterialIndices[i], *meshCount, triangleCount));

		(*meshCount)++;
	}

	// Reading all the data from all children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene, meshMaterialIndices, meshCount, triangleCount, MAX_MESH_COUNT);
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
	}

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

	return Mesh(meshName, vertices, indices, beginTriangleCount, meshCount, mesh->mMaterialIndex);
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene, unsigned int materialIndex, unsigned int meshCount, unsigned int* triangleCount)
{
	int beginTriangleCount = *triangleCount;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

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

	std::string meshName{ mesh->mName.C_Str() };

	return Mesh(meshName, vertices, indices, beginTriangleCount, meshCount, materialIndex);
}


glm::vec4 Model::aiVector3DToGLMVec4(aiVector3D v)
{
	return glm::vec4(v.x, v.y, v.z, 1.0f);
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
