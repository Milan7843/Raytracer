#include "SubdivisionSurface.h"

#include "Mesh.h"

namespace SubdivisionSurface
{
	namespace
	{
		Vertex mix(const Vertex& v1, const Vertex& v2, float t = 0.5f)
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

		void addNonDuplicate(const std::vector<Vertex>& inVertices, std::vector<unsigned int>& indices, unsigned int index)
		{
			for (unsigned int i = 0; i < indices.size(); i++)
			{
				if (inVertices[indices[i]].position == inVertices[index].position)
				{
					// Already found it
					return;
				}
			}

			indices.push_back(index);
		}

		void getNeighbours(const std::vector<unsigned int>& inIndices, const std::vector<Vertex>& inVertices, std::vector<unsigned int>& outIndices, unsigned int index)
		{
			for (unsigned int i = 0; i < inIndices.size(); i += 3)
			{
				if (inIndices[i + 0] == index)
				{
					addNonDuplicate(inVertices, outIndices, inIndices[i + 1]);
					addNonDuplicate(inVertices, outIndices, inIndices[i + 2]);
				}
				else if (inIndices[i + 1] == index)
				{
					addNonDuplicate(inVertices, outIndices, inIndices[i + 0]);
					addNonDuplicate(inVertices, outIndices, inIndices[i + 2]);
				}
				else if (inIndices[i + 2] == index)
				{
					addNonDuplicate(inVertices, outIndices, inIndices[i + 0]);
					addNonDuplicate(inVertices, outIndices, inIndices[i + 1]);
				}
			}
		}

		void getPositionsDuplicates(unsigned int index, const std::vector<Vertex>& vertices, std::vector<unsigned int>& outIndices)
		{
			Vertex vertex = vertices[index];

			for (unsigned int i = 0; i < vertices.size(); i++)
			{
				if (i == index) continue;

				if (vertices[i].position == vertex.position)
				{
					outIndices.push_back(i);
				}
			}
		}

		// Get the neighbours of a vertex + all neighbours of all position duplicates
		void getNeighboursWithDuplicates(unsigned int index,
			const std::vector<Vertex>& inVertices,
			const std::vector<unsigned int>& inIndices,
			std::vector<unsigned int>& outIndices)
		{
			std::vector<unsigned int> duplicateIndices;
			getPositionsDuplicates(index, inVertices, duplicateIndices);

			for (unsigned int i = 0; i < duplicateIndices.size(); i++)
			{
				unsigned int index = duplicateIndices[i];

				getNeighbours(inIndices, inVertices, outIndices, index);
			}
		}

		class Vec4HashSet
		{
		public: 
			Vec4HashSet() {}
			~Vec4HashSet() {}

			// Adds this vec4 to the set, returning if it was already contained
			bool add(glm::vec4 v)
			{
				for (unsigned int i = 0; i < set.size(); i++)
				{
					// Already contained
					if (v == set[i]) { return true; }
				}
				// Not yet contained, add it
				set.push_back(v);
				return false;
			}

		private:
			std::vector<glm::vec4> set;
		};
	}

	void replaceAll(std::vector<unsigned int>& indices, unsigned int replace, unsigned int with)
	{
		for (unsigned int i = 0; i < indices.size(); i++)
		{
			if (indices[i] == replace)
			{
				indices[i] = with;
			}
		}
	}

	// Not finished
	void removeDuplicateVertices(const std::vector<Vertex>& inVertices, std::vector<unsigned int>& inIndices, std::vector<Vertex>& outVertices)
	{
		std::cout << inVertices.size() << " vertices." << std::endl;
		for (unsigned int i = 0; i < inVertices.size(); i++)
		{
			std::cout
				<< "Vertex " << i << ": "
				<< inVertices[i].position.x << ", " << inVertices[i].position.y << ", " << inVertices[i].position.z
				<< std::endl;
		}

		// Merge vertices with equal positions
		Vec4HashSet set;

		for (unsigned int i = 0; i < inVertices.size(); i++)
		{
			if (set.add(inVertices[i].position))
			{
				// Already contained
				//replaceAll(inIndices, )
			}
			else
			{
				// New vertex
				outVertices.push_back(inVertices[i]);
			}
		}

		std::cout << inVertices.size() << " vertices." << std::endl;
		for (unsigned int i = 0; i < inVertices.size(); i++)
		{
			std::cout
				<< "Vertex " << i << ": "
				<< inVertices[i].position.x << ", " << inVertices[i].position.y << ", " << inVertices[i].position.z
				<< std::endl;
		}
	}

	int getLast(const std::vector<Vertex>& inVertices, const std::vector<unsigned int>& inIndices, unsigned int i1, unsigned int i2, unsigned int i_not)
	{
		Vertex v1 = inVertices[i1];
		Vertex v2 = inVertices[i2];
		Vertex v_not = inVertices[i_not];

		for (unsigned int i = 0; i < inIndices.size(); i += 3)
		{
			Vertex i1t = inVertices[inIndices[i + 0]];
			Vertex i2t = inVertices[inIndices[i + 1]];
			Vertex i3t = inVertices[inIndices[i + 2]];

			if (i1t.position == v1.position && i2t.position == v2.position && i3t.position != v_not.position)
			{
				return inIndices[i + 2];
			}
			if (i2t.position == v1.position && i3t.position == v2.position && i1t.position != v_not.position)
			{
				return inIndices[i + 0];
			}
			if (i1t.position == v1.position && i3t.position == v2.position && i2t.position != v_not.position)
			{
				return inIndices[i + 1];
			}
		}
		return -1;
	}

	Vertex createVertexBetween(const std::vector<Vertex>& inVertices, unsigned int a, unsigned int b, unsigned int across1, unsigned int across2)
	{
		if (across1 == -1 || across2 == -1)
		{
			std::cout << "Doesn't have vertices across" << std::endl;
			return mix(inVertices[a], inVertices[b]);
		}

		// Has vertices across
		std::cout << "Has vertices across" << std::endl;
		return (inVertices[a] * 0.375f) + (inVertices[b] * 0.375f) + (inVertices[across1] * 0.125f) + (inVertices[across2] * 0.125f);
	}

	void subdivide(const std::vector<Vertex>& inVertices, const std::vector<unsigned int>& inIndices, std::vector<Vertex>& outVertices, std::vector<unsigned int>& outIndices)
	{
		//::vector<Vertex> inVertices;

		//removeDuplicateVertices(inVerticesWithDupl, inIndices, inVertices);

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

			Vertex v4 = createVertexBetween(inVertices, i1, i2, i3, getLast(inVertices, inIndices, i1, i2, i3));
			Vertex v5 = createVertexBetween(inVertices, i2, i3, i1, getLast(inVertices, inIndices, i2, i3, i1));
			Vertex v6 = createVertexBetween(inVertices, i1, i3, i2, getLast(inVertices, inIndices, i1, i3, i2));


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
			neighbours.clear();
			getNeighboursWithDuplicates(i, inVertices, inIndices, neighbours);

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

			glm::vec4 position = inVertices[i].position * (1.0f - beta * n);
			glm::vec4 normal = inVertices[i].normal * (1.0f - beta * n);
			glm::vec4 uv = inVertices[i].uv * (1.0f - beta * n);
			glm::vec4 tangent = inVertices[i].tangent * (1.0f - beta * n);
			glm::vec4 bitangent = inVertices[i].bitangent * (1.0f - beta * n);


			std::cout << "Neighbours of "
				<< inVertices[i].position.x << ", " << inVertices[i].position.y << ", " << inVertices[i].position.z << std::endl;
			for (unsigned int i = 0; i < neighbours.size(); i++)
			{
				unsigned int neighbourIndex{ neighbours[i] };

				std::cout
					<< "Vertex " << i << ": "
					<< inVertices[neighbourIndex].position.x << ", " << inVertices[neighbourIndex].position.y << ", " << inVertices[neighbourIndex].position.z
					<< std::endl;
			}

			for (unsigned int j = 0; j < neighbours.size(); j++)
			{
				position += inVertices[neighbours[j]].position * beta;
				normal += inVertices[neighbours[j]].normal * beta;
				uv += inVertices[neighbours[j]].uv * beta;
				tangent += inVertices[neighbours[j]].tangent * beta;
				bitangent += inVertices[neighbours[j]].bitangent * beta;
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

	void subdivide(const std::vector<Vertex>& inVertices,
		const std::vector<unsigned int>& inIndices,
		std::vector<Vertex>& outVertices,
		std::vector<unsigned int>& outIndices,
		unsigned int times)
	{
		std::vector<Vertex> vertices = inVertices;
		std::vector<unsigned int> indices = inIndices;

		for (unsigned int i = 0; i < times; i++)
		{
			subdivide(vertices, indices, outVertices, outIndices);

			// Not final iteration: will require new vertices and indices
			if (i < times - 1)
			{
				indices = outIndices;
				vertices = outVertices;
				outVertices.clear();
				outIndices.clear();
			}
		}

	}
}
