#include "Model.h"

// Static variable
unsigned int Model::triangleCount = 0;

Model::Model(const std::string& path)
{
	loadModel(path);
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
		meshes[i].writeToShader(shader, ssbo);
	}
}

void Model::loadModel(std::string path)
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

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	// Reading mesh data for each mesh
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	// Reading all the data from all children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	int beginTriangleCount = triangleCount;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		// New vertex
		Vertex vertex;

		vertex.position = aiVector3DToGLMVec3(mesh->mVertices[i]);

		// Adding normals if possible
		if (mesh->HasNormals())
		{
			vertex.normal = aiVector3DToGLMVec3(mesh->mNormals[i]);
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
		triangleCount++;
	}

	return Mesh(vertices, indices, beginTriangleCount, Mesh::meshCount++);
}

glm::vec3 Model::aiVector3DToGLMVec3(aiVector3D v)
{
	return glm::vec3(v.x, v.y, v.z);
}