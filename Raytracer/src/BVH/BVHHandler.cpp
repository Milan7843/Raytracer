#include "BVHHandler.h"

#include "../Scene.h"

BVHHandler::BVHHandler(const char* vertexShaderPath, const char* geometryShaderPath, const char* fragmentShaderPath)
	: bvhRenderShader(vertexShaderPath, fragmentShaderPath, geometryShaderPath)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
}

BVHHandler::~BVHHandler()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void BVHHandler::draw(Scene& scene)
{
	bvhRenderShader.use();

	// View matrix
	glm::mat4 view = glm::mat4(1.0f);
	view = scene.getActiveCamera().getViewMatrix();
	bvhRenderShader.setMat4("view", view);

	// Projection matrix
	glm::mat4 projection;
	projection = scene.getActiveCamera().getProjectionMatrix();
	bvhRenderShader.setMat4("projection", projection);

	bvhRenderShader.setVector3("inputColor", glm::vec3(0.15, 0.92, 0.3));

	std::vector<BVHData> data;

	scene.getBVH().flatten(data, true);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(BVHData) * data.size(), data.data(), GL_STATIC_DRAW);

	// Specifying the vertex attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BVHData), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(BVHData), (void*)sizeof(glm::vec3));

	glDrawArrays(GL_POINTS, 0, data.size());

	glBindVertexArray(0);
}

BVHNode* BVHHandler::updateByScene(Scene& scene, BVHNode* oldBVHRoot)
{
	// Deleting any old data we have already
	deleteBVH(oldBVHRoot);

	std::vector<BVHNode*> meshRootNodes;

	// Adding all meshes' BVH root nodes
	for (Model& model : scene.getModels())
	{
		meshRootNodes.push_back(model.getRootNode());
	}

	// Generating a new BVH using the root nodes
	BVHNode* rootNode = generateBVHRecursively(meshRootNodes);
	return rootNode;
}

BVHNode* BVHHandler::generateFromModel(Model& model, BVHNode* oldBVHRoot)
{
	// Deleting any old data we have already
	deleteBVH(oldBVHRoot);

	std::vector<BVHNode*> meshRootNodes;

	// Adding all meshes' BVH root nodes
	for (Mesh& mesh : model.getMeshes())
	{
		// Generating a new tree from the mesh
		meshRootNodes.push_back(generateFromMesh(mesh, nullptr));
	}

	// Generating a new BVH using the root nodes
	BVHNode* rootNode = generateBVHRecursively(meshRootNodes);
	// Mark the root as a model root node
	rootNode->isModelRoot = true;
	return rootNode;
}

BVHNode* BVHHandler::generateFromMesh(Mesh& mesh, BVHNode* oldBVHRoot)
{
	// Deleting any old data we have already
	deleteBVH(oldBVHRoot);

	std::cout << "generating from mesh " << mesh.triangles.size() << std::endl;

	std::vector<unsigned int> indices(mesh.triangles.size());

	for (unsigned int i{ 0 }; i < mesh.triangles.size(); i++)
	{
		indices[i] = i;
	}

	// Generating a new BVH
	BVHNode* meshRootNode = generateBVHRecursively(mesh.triangles, indices, 0, mesh.shaderArraybeginIndex);
	return meshRootNode;
}

void BVHHandler::writeIntoSSBOs(BVHNode* root, unsigned int dataSSBO, unsigned int triangleSSBO)
{
	std::vector<FlattenedBVHNode> structure;
	std::vector<unsigned int> indices;
	

	/*
	BVHNode* D = new BVHNode{
		nullptr,
		nullptr,
		BVHData{},
		std::vector<unsigned int> { 0, 1, 2 }
	};
	BVHNode* E = new BVHNode{
		nullptr,
		nullptr,
		BVHData{},
		std::vector<unsigned int> { 3, 4, 5 }
	};

	BVHNode* F = new BVHNode{
		nullptr,
		nullptr,
		BVHData{},
		std::vector<unsigned int> { 6, 7, 8 }
	};
	BVHNode* G = new BVHNode{
		nullptr,
		nullptr,
		BVHData{},
		std::vector<unsigned int> { 9, 10, 11 }
	};

	BVHNode* B = new BVHNode{
		D,
		E,
		BVHData{},
		std::vector<unsigned int> { }
	};

	BVHNode* C = new BVHNode{
		F,
		G,
		BVHData{},
		std::vector<unsigned int> { }
	};


	BVHNode root{
		B,
		C,
		BVHData{},
		std::vector<unsigned int> {}
	};
	// A
	BVHNode root{
		// B
		new BVHNode {
			// D
			new BVHNode {
				nullptr,
				nullptr,
				BVHData{},
				std::vector<unsigned int> { 0, 1, 2 }
			},
			// E
			new BVHNode {
				nullptr,
				nullptr,
				BVHData{},
				std::vector<unsigned int> { 3, 4, 5 }
			},
			BVHData{},
			std::vector<unsigned int> {}
		},
		// C
		new BVHNode {
			// F
			new BVHNode {
				nullptr,
				nullptr,
				BVHData{},
				std::vector<unsigned int> { 6, 7, 8 }
			},
			// G
			new BVHNode {
				nullptr,
				nullptr,
				BVHData{},
				std::vector<unsigned int> { 9, 10, 11 }
			},
			BVHData{},
			std::vector<unsigned int> {}
		},
		BVHData{},
		std::vector<unsigned int> {}
	};

	*/
	
	// Creating the index data:
	flattenBVHTreeIndices(root, structure, indices);

	if (structure.size() == 0 || indices.size() == 0)
	{
		Logger::logError("No data in BVH");
		return;
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, dataSSBO);

	std::cout << "Writing " << structure.size() << " nodes into ssbo " << dataSSBO << std::endl;

	// Loading zero-data into the data buffer
	glBufferData(GL_SHADER_STORAGE_BUFFER, structure.size() * sizeof(FlattenedBVHNode), &structure[0], GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, dataSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangleSSBO);

	std::cout << "Writing " << indices.size() << " indices into ssbo " << triangleSSBO << std::endl;

	// Loading zero-data into the triangle buffer
	glBufferData(GL_SHADER_STORAGE_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, triangleSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	/*
	std::cout << "Structure: \n";
	unsigned int index{ 0 };
	for (FlattenedBVHNode& flatNode : structure)
	{
		std::cout << index++ << " - l: " << flatNode.leftChild << ", r: " << flatNode.rightChild << "\n";
	}
	std::cout << std::endl;

	std::cout << "Indices: \n";
	for (unsigned int i : indices)
	{
		std::cout << i << " ";
	}
	std::cout << std::endl;
	*/
}

BVHNode* BVHHandler::generateBVHRecursively(std::vector<Triangle>& triangles, std::vector<unsigned int> indices, unsigned int depth, unsigned int shaderArrayBeginIndex)
{
	//std::cout << indices.size() << " triangles" << std::endl;
	BVHNode* parent = new BVHNode;

	// Calculating the bounding box of the parent
	parent->data = getBoundingBox(triangles, indices);

	//std::cout << "bounding box: pos (" << parent->data.pos.x << ", " << parent->data.pos.y << ", " << parent->data.pos.z
	//	<< "), size ("
	//	<< parent->data.size.x << ", " << parent->data.size.y << ", " << parent->data.size.z << ")" << std::endl;

	// Base case: stop on too few triangles
	if (indices.size() <= 20 || depth >= 40)
	{
		//std::cout << "triangle count: " << triangles.size() << std::endl;
		parent->triangleIndices = indices;
		for (unsigned int i{ 0 }; i < indices.size(); i++)
		{
			parent->triangleIndices[i] += shaderArrayBeginIndex;
		}
		return parent;
	}

	// Dividing up the triangles
	// 1. Choosing an axis: simply the longest axis
	// x by default
	Axis axis = Axis::x;
	// Checking if y is bigger
	if (parent->data.size.y > parent->data.size.x && parent->data.size.y > parent->data.size.z)
		axis = Axis::y;
	// Checking if z is bigger
	if (parent->data.size.z > parent->data.size.x && parent->data.size.z > parent->data.size.y)
		axis = Axis::z;
	//axis = Axis::z;
	
	// Sorting on the correct axis
	switch (axis)
	{
		case Axis::x:
			std::sort(indices.begin(), indices.end(), [&triangles](const unsigned int& a, const unsigned int& b) {
				Triangle& tri1 = triangles[a];
				Triangle& tri2 = triangles[b];
				float middleA = (tri1.v1.x + tri1.v2.x + tri1.v3.x) / 3.0f;
				float middleB = (tri2.v1.x + tri2.v2.x + tri2.v3.x) / 3.0f;
				return middleA < middleB;
				});
			break;
		case Axis::y:
			std::sort(indices.begin(), indices.end(), [&triangles](const unsigned int& a, const unsigned int& b) {
				Triangle& tri1 = triangles[a];
				Triangle& tri2 = triangles[b];
				float middleA = (tri1.v1.y + tri1.v2.y + tri1.v3.y) / 3.0f;
				float middleB = (tri2.v1.y + tri2.v2.y + tri2.v3.y) / 3.0f;
				return middleA < middleB;
				});
			break;
		case Axis::z:
			std::sort(indices.begin(), indices.end(), [&triangles](const unsigned int& a, const unsigned int& b) {
				Triangle& tri1 = triangles[a];
				Triangle& tri2 = triangles[b];
				float middleA = (tri1.v1.z + tri1.v2.z + tri1.v3.z) / 3.0f;
				float middleB = (tri2.v1.z + tri2.v2.z + tri2.v3.z) / 3.0f;
				return middleA < middleB;
				});
			break;
	}

	unsigned int medianIndex = (unsigned int)(indices.size() / 2);

	// Dividing up the data
	//std::vector<unsigned int> left(&indices[0], &indices[medianIndex]);
	//std::vector<unsigned int> right(&indices[medianIndex], &indices.end());
	std::vector<unsigned int> left(indices.begin(), indices.begin() + medianIndex);
	std::vector<unsigned int> right(indices.begin() + medianIndex, indices.end());

	// Then recursively creating the children
	parent->leftChild = generateBVHRecursively(triangles, left, depth + 1, shaderArrayBeginIndex);
	parent->rightChild = generateBVHRecursively(triangles, right, depth + 1, shaderArrayBeginIndex);
	
	return parent;
}

void BVHHandler::flattenBVHTreeData(BVHNode* rootNode, std::vector<BVHData>& data, bool onlyLeaves)
{
	// Do not continue on no root
	if (rootNode == nullptr) return;

	// Leaf only / is leaf check
	if (!onlyLeaves || (rootNode->leftChild == nullptr && rootNode->rightChild == nullptr))
	{
		data.push_back(rootNode->data);
		/*
		std::cout << "flattened node: pos (" << rootNode->data.pos.x << ", " << rootNode->data.pos.y << ", " << rootNode->data.pos.z
			<< "), size ("
			<< rootNode->data.size.x << ", " << rootNode->data.size.y << ", " << rootNode->data.size.z << ")" << std::endl;
		*/
	}

	flattenBVHTreeData(rootNode->leftChild, data, onlyLeaves);
	flattenBVHTreeData(rootNode->rightChild, data, onlyLeaves);
}

void BVHHandler::flattenBVHTreeIndices(BVHNode* rootNode, std::vector<FlattenedBVHNode>& treeStructureData, std::vector<unsigned int>& indices)
{
	// There must be a root
	if (rootNode == nullptr) return;
	
	FlattenedBVHNode flattenedNode;

	//flattenedNode.data = rootNode->data;
	flattenedNode.pos = CoordinateUtility::vec3ToGLSLVec3(rootNode->data.pos);
	flattenedNode.size = CoordinateUtility::vec3ToGLSLVec3(rootNode->data.size);

	unsigned int currentIndex = treeStructureData.size();

	treeStructureData.push_back(flattenedNode);

	// Check for leaf node
	if (rootNode->leftChild == nullptr && rootNode->rightChild == nullptr)
	{
		// Setting a leaf marker
		treeStructureData[currentIndex].leftChild = -1;
		treeStructureData[currentIndex].rightChild = indices.size();

		// Pushing the number of following indices
		indices.push_back(rootNode->triangleIndices.size());

		// And pushing the indices
		for (unsigned int index : rootNode->triangleIndices)
		{
			indices.push_back(index);
		}
	}
	// Internal node
	else
	{
		// Pushing the indices of the two children
		treeStructureData[currentIndex].leftChild = treeStructureData.size();
		flattenBVHTreeIndices(rootNode->leftChild, treeStructureData, indices);
		treeStructureData[currentIndex].rightChild = treeStructureData.size();
		flattenBVHTreeIndices(rootNode->rightChild, treeStructureData, indices);
	}
}

BVHNode* BVHHandler::generateBVHRecursively(std::vector<BVHNode*> nodes)
{
	BVHNode* parent = new BVHNode;

	// Calculating the bounding box of the parent
	parent->data = getBoundingBox(nodes);

	// Base case: stop on three nodes left, as we cannot recurse with 1 node (cannot fill 2 children)
	if (nodes.size() == 3)
	{
		// Making a vector with only the first two
		std::vector<BVHNode*> left{ nodes.begin(), nodes.begin() + 2 };

		// Making one last recursion to split up the two
		parent->leftChild = generateBVHRecursively(left);

		// The right child will just be the left over one
		parent->rightChild = nodes[nodes.size() - 1];

		return parent;
	}
	// Base case: stop on 2 nodes left: exactly 2 children
	if (nodes.size() == 2)
	{
		parent->leftChild = nodes[0];
		parent->rightChild = nodes[1];
		return parent;
	}
	// Base case: stop on a single node left: should only occur when the input size is just one, not during recursion
	if (nodes.size() == 1)
	{
		//Logger::logError("Ended up with fewer than 2 nodes!");
		parent->leftChild = nodes[0];
		parent->rightChild = nullptr;
		return parent;
	}
	// Base case: stop when less than 1 nodes: must not happen!
	if (nodes.size() < 1)
	{
		Logger::logError("Ended up with no nodes!");
		return parent;
	}

	// If we haven't hit a base case, split up the nodes and recurse
	unsigned int medianIndex = (unsigned int)(nodes.size() / 2);

	// Dividing up the data
	//std::vector<BVHNode*> left(&nodes[0], &nodes[medianIndex]);
	//std::vector<BVHNode*> right(&nodes[medianIndex], &nodes[nodes.size()-1]);
	std::vector<BVHNode*> left(nodes.begin(), nodes.begin() + medianIndex);
	std::vector<BVHNode*> right(nodes.begin() + medianIndex, nodes.end());

	// Then recursively creating the children
	parent->leftChild = generateBVHRecursively(left);
	parent->rightChild = generateBVHRecursively(right);

	return parent;
}

void BVHHandler::deleteBVH(BVHNode* node)
{
	// Deleting the node
	if (node != nullptr)
	{
		// Making it delete its children
		node->deleteNode();
		// Then deleting it
		delete node;
		// And making sure we don't get an invalid pointer
		node = nullptr;
	}
}


BVHData BVHHandler::getBoundingBox(std::vector<Triangle>& triangles, std::vector<unsigned int>& indices)
{
	glm::vec4 min = glm::vec4(0.0f);
	glm::vec4 max = glm::vec4(0.0f);

	// Setting the minimum and maximum to an arbitrary vertex
	if (indices.size() >= 1)
	{
		min = triangles[indices[0]].v1;
		max = triangles[indices[0]].v1;
	}

	// Updating the minimum and maximum for every vertex
	for (unsigned int& i : indices)
	{
		Triangle& tri = triangles[i];
		updateMinMax(min, max, tri.v1);
		updateMinMax(min, max, tri.v2);
		updateMinMax(min, max, tri.v3);
	}

	// Calculating the required values:
	// center: avg. of min and max
	glm::vec3 center = (glm::vec3(min) + glm::vec3(max)) / 2.0f;
	// size: offset between min and max
	glm::vec3 size = (glm::vec3(max) - glm::vec3(min));// *1.05f;

	return BVHData{ center, size };
}

void BVHHandler::updateMinMax(glm::vec4& min, glm::vec4& max, const glm::vec4& val)
{
	if (val.x < min.x)
		min.x = val.x;
	if (val.y < min.y)
		min.y = val.y;
	if (val.z < min.z)
		min.z = val.z;
	if (val.x > max.x)
		max.x = val.x;
	if (val.y > max.y)
		max.y = val.y;
	if (val.z > max.z)
		max.z = val.z;
}

void BVHHandler::updateMinMax(glm::vec3& min, glm::vec3& max, const glm::vec3& val)
{
	if (val.x < min.x)
		min.x = val.x;
	if (val.y < min.y)
		min.y = val.y;
	if (val.z < min.z)
		min.z = val.z;
	if (val.x > max.x)
		max.x = val.x;
	if (val.y > max.y)
		max.y = val.y;
	if (val.z > max.z)
		max.z = val.z;
}

BVHData BVHHandler::getBoundingBox(std::vector<BVHNode*>& nodes)
{
	glm::vec3 min = glm::vec3(0.0f);
	glm::vec3 max = glm::vec3(0.0f);

	// Setting the minimum and maximum to an arbitrary vertex
	if (nodes.size() >= 1)
	{
		min = nodes[0]->data.pos;
		max = nodes[0]->data.pos;
	}

	// Updating the minimum and maximum for every vertex
	for (BVHNode* node : nodes)
	{
		updateMinMax(min, max, glm::vec3(node->data.pos - node->data.size / 2.0f));
		updateMinMax(min, max, glm::vec3(node->data.pos + node->data.size / 2.0f));
	}

	// Calculating the required values:
	// center: avg. of min and max
	glm::vec3 center = (glm::vec3(min) + glm::vec3(max)) / 2.0f;
	// size: offset between min and max
	glm::vec3 size = (glm::vec3(max) - glm::vec3(min));// *0.96f;

	return BVHData{ center, size };
}
