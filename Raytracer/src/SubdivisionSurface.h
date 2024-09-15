#pragma once

#include <vector>

struct Vertex;


namespace SubdivisionSurface
{
	void subdivide(std::vector<Vertex>& inVertices,
		std::vector<unsigned int>& inIndices,
		std::vector<Vertex>& outVertices,
		std::vector<unsigned int>& outIndices);
};

