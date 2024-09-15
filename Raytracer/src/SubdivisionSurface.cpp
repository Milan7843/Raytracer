#include "SubdivisionSurface.h"

#include "Mesh.h"

namespace SubdivisionSurface
{
	namespace
	{
		Vertex mix(Vertex& v1, Vertex& v2, float t = 0.5f)
		{
			return Vertex(
				v1.position * (1.0f - t) + v2.position * t,
				v1.normal * (1.0f - t) + v2.normal * t,
				v1.uv * (1.0f - t) + v2.uv * t,
				v1.tangent * (1.0f - t) + v2.tangent * t,
				v1.bitangent * (1.0f - t) + v2.bitangent * t
			);
		}

		// Try to find this vertex in the list of vertices.
		// Returns -1 if it is not found, otherwise the index
		int contains(const std::vector<Vertex>& vertices, Vertex& v, int from = 0)
		{
			for (int i = from; i < vertices.size(); i++)
			{
				// Compare the vertices
				if (vertices[i].position == v.position && vertices[i].normal == v.normal)
				{
					return i;
				}
			}

			return -1;
		}

		void addNonDuplicate(std::vector<unsigned int>& indices, unsigned int index)
		{
			for (unsigned int i = 0; i < indices.size(); i++)
			{
				if (indices[i] == index)
				{
					// Already found it
					return;
				}
			}

			indices.push_back(index);
		}

		void getNeighbours(const std::vector<unsigned int>& inIndices, std::vector<unsigned int>& outIndices, unsigned int index)
		{
			for (unsigned int i = 0; i < inIndices.size(); i += 3)
			{
				if (inIndices[i + 0] == index)
				{
					addNonDuplicate(outIndices, i + 1);
					addNonDuplicate(outIndices, i + 2);
				}
				else if (inIndices[i + 1] == index)
				{
					addNonDuplicate(outIndices, i + 0);
					addNonDuplicate(outIndices, i + 2);
				}
				else if (inIndices[i + 2] == index)
				{
					addNonDuplicate(outIndices, i + 0);
					addNonDuplicate(outIndices, i + 1);
				}
			}
		}
	}

	void subdivide(std::vector<Vertex>& inVertices, std::vector<unsigned int>& inIndices, std::vector<Vertex>& outVertices, std::vector<unsigned int>& outIndices)
	{
		// Adding the original indices
		for (unsigned int i = 0; i < inVertices.size(); i++)
		{
			outVertices.push_back(inVertices[i]);
		}

		// Creating an array of new vertices
		for (unsigned int i = 0; i < inIndices.size(); i += 3)
		{
			unsigned int i1 = inIndices[i + 0];
			unsigned int i2 = inIndices[i + 1];
			unsigned int i3 = inIndices[i + 2];

			// Adding 3 new vertices, one on each edge:
			unsigned int i4;
			unsigned int i5;
			unsigned int i6;

			Vertex v4 = mix(inVertices[i1], inVertices[i2]);
			Vertex v5 = mix(inVertices[i2], inVertices[i3]);
			Vertex v6 = mix(inVertices[i3], inVertices[i1]);


			int index;
			// This parts removes duplicate new vertices
			if ((index = contains(outVertices, v4, inVertices.size())) != -1)
			{
				i4 = index;
			}
			else
			{
				i4 = outVertices.size();
				outVertices.push_back(v4);
			}

			if ((index = contains(outVertices, v5, inVertices.size())) != -1)
			{
				i5 = index;
			}
			else
			{
				i5 = outVertices.size();
				outVertices.push_back(v5);
			}

			if ((index = contains(outVertices, v6, inVertices.size())) != -1)
			{
				i6 = index;
			}
			else
			{
				i6 = outVertices.size();
				outVertices.push_back(v6);
			}

			// 1, 4, 6
			outIndices.push_back(i1);
			outIndices.push_back(i4);
			outIndices.push_back(i6);
			// 2, 5, 4
			outIndices.push_back(i2);
			outIndices.push_back(i5);
			outIndices.push_back(i4);
			// 3, 6, 5
			outIndices.push_back(i3);
			outIndices.push_back(i6);
			outIndices.push_back(i5);
			// 4, 5, 6
			outIndices.push_back(i4);
			outIndices.push_back(i5);
			outIndices.push_back(i6);
		}

		// Updating original vertex positions
		std::vector<unsigned int> neighbours;
		for (unsigned int i = 0; i < inVertices.size(); i++)
		{
			// Finding the neighbours
			getNeighbours(inIndices, neighbours, i);

			float beta;
			float n = neighbours.size();
			if (n == 3)
			{
				beta = 3.0f / 16.0f;
			}
			else
			{
				beta = 3.0f / (8.0f * n);
			}

			glm::vec4 position = outVertices[i].position * (1.0f - beta * n);
			glm::vec4 normal = outVertices[i].normal * (1.0f - beta * n);
			glm::vec4 uv = outVertices[i].uv * (1.0f - beta * n);
			glm::vec4 tangent = outVertices[i].tangent * (1.0f - beta * n);
			glm::vec4 bitangent = outVertices[i].bitangent * (1.0f - beta * n);

			for (unsigned int j = 0; j < neighbours.size(); j++)
			{
				position += outVertices[neighbours[j]].position * beta;
				normal += outVertices[neighbours[j]].normal * beta;
				uv += outVertices[neighbours[j]].uv * beta;
				tangent += outVertices[neighbours[j]].tangent * beta;
				bitangent += outVertices[neighbours[j]].bitangent * beta;
			}


			outVertices[i] = Vertex{
				position,
				normal,
				uv,
				tangent,
				bitangent
			};
		}
	}
}
