#include "Model.h"

Model::Model(const std::string& path, unsigned int* meshCount, unsigned int* triangleCount, unsigned int materialIndex,
	unsigned int MAX_MESH_COUNT)
	: path(path)
{
	this->materialIndex = materialIndex;
	loadModel(path, meshCount, triangleCount, MAX_MESH_COUNT);
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
	filestream << materialIndex << "\n";
}

void Model::draw(AbstractShader* shader, Material* material)
{
	// Setting up the shader
	shader->setMat4("model", getTransformationMatrix());
	shader->setVector3("inputColor", material->color);

	// Drawing each mesh
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].draw(shader);
	}
}

void Model::writeToShader(AbstractShader* shader, unsigned int ssbo)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].writeToShader(shader, ssbo, materialIndex, getTransformationMatrix());
	}
}

void Model::loadModel(std::string path, unsigned int* meshCount, unsigned int* triangleCount, unsigned int MAX_MESH_COUNT)
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

	processNode(scene->mRootNode, scene, meshCount, triangleCount, MAX_MESH_COUNT);
}

void Model::processNode(aiNode* node, const aiScene* scene, unsigned int* meshCount, unsigned int* triangleCount,
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
		meshes.push_back(processMesh(mesh, scene, *meshCount, triangleCount));

		(*meshCount)++;
	}

	// Reading all the data from all children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene, meshCount, triangleCount, MAX_MESH_COUNT);
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
			
			// TEST REMOVE!
			vertex.normal = glm::vec4(glm::normalize(glm::vec3(vertex.position)), 1.0f);
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

	return Mesh(vertices, indices, beginTriangleCount, meshCount);
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
		<< "\nmaterial: " << model.materialIndex
		<< std::endl;

	return stream;
}
