#pragma once

#include <vector>

struct Vertex;


namespace SubdivisionSurface
{
	void subdivide(const std::vector<Vertex>& inVertices,
		const std::vector<unsigned int>& inIndices,
		std::vector<Vertex>& outVertices,
		std::vector<unsigned int>& outIndices);

	void subdivide(const std::vector<Vertex>& inVertices,
		const std::vector<unsigned int>& inIndices,
		std::vector<Vertex>& outVertices,
		std::vector<unsigned int>& outIndices,
		unsigned int times);
};

