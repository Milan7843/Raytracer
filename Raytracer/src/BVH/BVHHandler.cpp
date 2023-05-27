#include "BVHHandler.h"

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

	flattenBVHTreeData(scene.getBVHRoot(), data, false);

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
		for (Mesh& mesh : model.getMeshes())
		{
			meshRootNodes.push_back(mesh.getRootNode());
		}
	}

	// Generating a new BVH using the root nodes
	BVHNode* rootNode = generateBVHRecursively(meshRootNodes);
	return rootNode;
}

BVHNode* BVHHandler::generateFromMesh(Mesh& mesh, BVHNode* oldBVHRoot)
{
	// Deleting any old data we have already
	deleteBVH(oldBVHRoot);

	// Generating a new BVH
	BVHNode* meshRootNode = generateBVHRecursively(mesh.triangles, 0);
	return meshRootNode;
}

BVHNode* BVHHandler::generateBVHRecursively(std::vector<Triangle> triangles, unsigned int depth)
{
	BVHNode* parent = new BVHNode;

	// Calculating the bounding box of the parent
	parent->data = getBoundingBox(triangles);

	// Base case: stop on too few triangles
	if (triangles.size() <= 8)
	{
		//std::cout << "triangle count: " << triangles.size() << std::endl;
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
	
	// Sorting on the correct axis
	switch (axis)
	{
		case Axis::x:
			std::sort(triangles.begin(), triangles.end(), [](const Triangle& a, const Triangle& b) {
				float middleA = (a.v1.x + a.v2.x + a.v3.x) / 3.0f;
				float middleB = (b.v1.x + b.v2.x + b.v3.x) / 3.0f;
				return middleA < middleB;
				});
			break;
		case Axis::y:
			std::sort(triangles.begin(), triangles.end(), [](const Triangle& a, const Triangle& b) {
				float middleA = (a.v1.y + a.v2.y + a.v3.y) / 3.0f;
				float middleB = (b.v1.y + b.v2.y + b.v3.y) / 3.0f;
				return middleA < middleB;
				});
			break;
		case Axis::z:
			std::sort(triangles.begin(), triangles.end(), [](const Triangle& a, const Triangle& b) {
				float middleA = (a.v1.z + a.v2.z + a.v3.z) / 3.0f;
				float middleB = (b.v1.z + b.v2.z + b.v3.z) / 3.0f;
				return middleA < middleB;
				});
			break;
	}

	unsigned int medianIndex = (unsigned int)(triangles.size() / 2);

	// Dividing up the data
	std::vector<Triangle> left(&triangles[0], &triangles[medianIndex]);
	std::vector<Triangle> right(&triangles[medianIndex], &triangles[triangles.size()]);

	// Then recursively creating the children
	parent->leftChild = generateBVHRecursively(left, depth + 1);
	parent->rightChild = generateBVHRecursively(right, depth + 1);
	
	return parent;
}

void BVHHandler::flattenBVHTreeData(BVHNode* rootNode, std::vector<BVHData>& data, bool onlyLeaves)
{
	// Do not continue on no root
	if (rootNode == nullptr) return;

	if (!onlyLeaves || (rootNode->leftChild == nullptr && rootNode->rightChild == nullptr))
		data.push_back(rootNode->data);

	flattenBVHTreeData(rootNode->leftChild, data, onlyLeaves);
	flattenBVHTreeData(rootNode->rightChild, data, onlyLeaves);
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
	// Base case: stop when less than 2 nodes: must not happen!
	if (nodes.size() < 2)
	{
		Logger::logError("Ended up with fewer than 2 nodes!");
		return parent;
	}

	// If we haven't hit a base case, split up the nodes and recurse
	unsigned int medianIndex = (unsigned int)(nodes.size() / 2);

	// Dividing up the data
	std::vector<BVHNode*> left(&nodes[0], &nodes[medianIndex]);
	std::vector<BVHNode*> right(&nodes[medianIndex], &nodes[nodes.size()]);

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


BVHData BVHHandler::getBoundingBox(std::vector<Triangle>& triangles)
{
	glm::vec4 min = glm::vec4(0.0f);
	glm::vec4 max = glm::vec4(0.0f);

	// Setting the minimum and maximum to an arbitrary vertex
	if (triangles.size() >= 1)
	{
		min = triangles[0].v1;
		max = triangles[0].v1;
	}

	// Updating the minimum and maximum for every vertex
	for (Triangle& tri : triangles)
	{
		updateMinMax(min, max, tri.v1);
		updateMinMax(min, max, tri.v2);
		updateMinMax(min, max, tri.v3);

		/*
		std::cout << "tri v1: " << tri.v1.x << ", " << tri.v1.y << ", " << tri.v1.z << std::endl;
		std::cout << "tri v2: " << tri.v2.x << ", " << tri.v2.y << ", " << tri.v2.z << std::endl;
		std::cout << "tri v3: " << tri.v3.x << ", " << tri.v3.y << ", " << tri.v3.z << std::endl;
		std::cout << "new min: " << min.x << ", " << min.y << ", " << min.z << std::endl;
		std::cout << "new max: " << max.x << ", " << max.y << ", " << max.z << std::endl;
		*/
	}

	/*
	std::cout << "min: " << min.x << ", " << min.y << ", " << min.z << std::endl;
	std::cout << "max: " << max.x << ", " << max.y << ", " << max.z << std::endl;
	*/

	// Calculating the required values:
	// center: avg. of min and max
	glm::vec3 center = (glm::vec3(min) + glm::vec3(max)) / 2.0f;
	// size: offset between min and max
	glm::vec3 size = (glm::vec3(max) - glm::vec3(min)) * 0.96f;

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

	/*
	std::cout << "min: " << min.x << ", " << min.y << ", " << min.z << std::endl;
	std::cout << "max: " << max.x << ", " << max.y << ", " << max.z << std::endl;
	*/

	// Calculating the required values:
	// center: avg. of min and max
	glm::vec3 center = (glm::vec3(min) + glm::vec3(max)) / 2.0f;
	// size: offset between min and max
	glm::vec3 size = (glm::vec3(max) - glm::vec3(min)) * 0.96f;

	return BVHData{ center, size };
}
