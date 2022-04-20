#include "Model.h"

Model::Model(const std::string& path, unsigned int* meshCount, unsigned int* triangleCount, unsigned int materialIndex)
	: materialIndex(materialIndex)
{
	loadModel(path, meshCount, triangleCount);
}

Model::~Model()
{
}

void Model::draw(Shader* shader)
{
	// Drawing each mesh
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].draw(shader);
	}
}

void Model::writeToShader(Shader* shader, unsigned int ssbo)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].writeToShader(shader, ssbo, materialIndex);
	}
}

void Model::rotate(glm::vec3 rotationAxis, float degrees)
{
	this->transformation = glm::rotate(transformation, degrees, rotationAxis);
}

void Model::move(glm::vec3 move)
{
	this->transformation = glm::translate(transformation, move);
}

void Model::scale(glm::vec3 scale)
{
	this->transformation = glm::scale(transformation, scale);
}

void Model::scale(float scale)
{
	this->scale(glm::vec3(1.0f) * scale);
}

void Model::applyTransformations()
{
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].applyTransformations(transformation);
	}
}

void Model::loadModel(std::string path, unsigned int* meshCount, unsigned int* triangleCount)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);

	// Checking for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "Assimp error: model loading failed:" << importer.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene, meshCount, triangleCount);
}

void Model::processNode(aiNode* node, const aiScene* scene, unsigned int* meshCount, unsigned int* triangleCount)
{
	// Reading mesh data for each mesh
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene, *meshCount, triangleCount));

		(*meshCount)++;
	}
	// Reading all the data from all children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene, meshCount, triangleCount);
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

	return Mesh(vertices, indices, beginTriangleCount, meshCount);
}

glm::vec4 Model::aiVector3DToGLMVec4(aiVector3D v)
{
	return glm::vec4(v.x, v.y, v.z, 1.0f);
}