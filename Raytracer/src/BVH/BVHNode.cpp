#include "BVHNode.h"

void BVHNode::deleteNode()
{
	// Deleting the left child, except if it is a mesh root
	if (leftChild != nullptr && !leftChild->isModelRoot)
	{
		// Making it delete its children
		leftChild->deleteNode();
		// Then deleting it
		delete leftChild;
		// And making sure we don't get an invalid pointer
		leftChild = nullptr;
	}

	// Deleting the right child, except if it is a mesh root
	if (rightChild != nullptr && !rightChild->isModelRoot)
	{
		// Making it delete its children
		rightChild->deleteNode();
		// Then deleting it
		delete rightChild;
		// And making sure we don't get an invalid pointer
		rightChild = nullptr;
	}
}
