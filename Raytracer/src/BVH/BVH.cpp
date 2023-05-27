#include "BVH.h"

BVH::BVH(const char* vertexShaderPath, const char* geometryShaderPath, const char* fragmentShaderPath)
	: bvhRenderShader(vertexShaderPath, fragmentShaderPath, geometryShaderPath)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
}

BVH::~BVH()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void BVH::draw(Scene& scene)
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

	flattenBVHTreeData(root, data, true);

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

void BVH::generateFromModel(Model& model)
{
	// Deleting any old data we have already
	deleteCurrentBVH();

	std::vector<Mesh*> meshes;

	// Taking all meshes from the model and putting them together in one vector
	for (Mesh& mesh : model.getMeshes())
	{
		meshes.push_back(&mesh);
	}

	std::vector<BVHNode*> meshRootNodes;

	// Going over all meshes and calculating a BVH for each
	for (Mesh* mesh : meshes)
	{
		// Copying the triangles so that they aren't affected by the in-place sorting
		//std::vector<Triangle> copy(mesh->triangles);
		//BVHNode* meshRootNode = generateBVHRecursively(mesh->triangles, 0);
		//root = meshRootNode;
	}
	BVHNode* meshRootNode = generateBVHRecursively(meshes[1]->triangles, 0);
	root = meshRootNode;
}

BVHNode* BVH::generateBVHRecursively(std::vector<Triangle> triangles, unsigned int depth)
{
	BVHNode* parent = new BVHNode;

	// Calculating the bounding box of the parent
	parent->data = getBoundingBox(triangles);

	//std::cout << "pos " << parent->data.pos.x << ", " << parent->data.pos.y << ", " << parent->data.pos.z << std::endl;
	//std::cout << "size " << parent->data.size.x << ", " << parent->data.size.y << ", " << parent->data.size.z << std::endl;

	for (Triangle& a : triangles)
	{
		glm::vec3 avg = (a.v1 + a.v2 + a.v3) / 3.0f;
		//std::cout << "input " << avg.x << ", " << avg.y << ", " << avg.z << std::endl;
	}

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

	/*
	// Sorting on the correct axis
	switch (axis)
	{
		case Axis::x:
			std::sort(triangles.begin(), triangles.end(), &BVH::compareTrianglesX);
			break;
		case Axis::y:
			std::sort(triangles.begin(), triangles.end(), &BVH::compareTrianglesY);
			break;
		case Axis::z:
			std::sort(triangles.begin(), triangles.end(), &BVH::compareTrianglesZ);
			break;
	}*/

	unsigned int medianIndex = (unsigned int)(triangles.size() / 2);

	// Dividing up the data
	std::vector<Triangle> left(&triangles[0], &triangles[medianIndex]);
	std::vector<Triangle> right(&triangles[medianIndex], &triangles[triangles.size()]);

	//std::cout << "total size: " << triangles.size() << ", left size: " << left.size() << ", right size: " << right.size() << std::endl;

	for (Triangle& a : left)
	{
		glm::vec3 avg = (a.v1 + a.v2 + a.v3) / 3.0f;
		//std::cout << "left " << avg.x << ", " << avg.y << ", " << avg.z << std::endl;
		//std::cout << "tri v1: " << a.v1.x << ", " << a.v1.y << ", " << a.v1.z << std::endl;
		//std::cout << "tri v2: " << a.v2.x << ", " << a.v2.y << ", " << a.v2.z << std::endl;
		//std::cout << "tri v3: " << a.v3.x << ", " << a.v3.y << ", " << a.v3.z << std::endl;
	}

	for (Triangle& a : right)
	{
		glm::vec3 avg = (a.v1 + a.v2 + a.v3) / 3.0f;
		//std::cout << "right " << avg.x << ", " << avg.y << ", " << avg.z << std::endl;
		//std::cout << "tri v1: " << a.v1.x << ", " << a.v1.y << ", " << a.v1.z << std::endl;
		//std::cout << "tri v2: " << a.v2.x << ", " << a.v2.y << ", " << a.v2.z << std::endl;
		//std::cout << "tri v3: " << a.v3.x << ", " << a.v3.y << ", " << a.v3.z << std::endl;
	}

	// Then recursively creating the children
	parent->leftChild = generateBVHRecursively(left, depth + 1);
	parent->rightChild = generateBVHRecursively(right, depth + 1);
	
	return parent;
}

void BVH::flattenBVHTreeData(BVHNode* rootNode, std::vector<BVHData>& data, bool onlyLeaves)
{
	// Do not continue on no root
	if (rootNode == nullptr) return;

	if (!onlyLeaves || (rootNode->leftChild == nullptr && rootNode->rightChild == nullptr))
		data.push_back(rootNode->data);

	flattenBVHTreeData(rootNode->leftChild, data, onlyLeaves);
	flattenBVHTreeData(rootNode->rightChild, data, onlyLeaves);
}

void BVH::deleteCurrentBVH()
{
	// Deleting the root node
	if (root != nullptr)
	{
		// Making it delete its children
		root->deleteNode();
		// Then deleting it
		delete root;
		// And making sure we don't get an invalid pointer
		root = nullptr;
	}
}


BVHData BVH::getBoundingBox(std::vector<Triangle>& triangles)
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

void BVH::updateMinMax(glm::vec4& min, glm::vec4& max, glm::vec4& val)
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
