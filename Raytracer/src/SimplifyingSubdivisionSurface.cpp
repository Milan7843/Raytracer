#include "SubdivisionSurface.h"

#include "Mesh.h"

namespace SubdivisionSurface
{
	namespace
	{
		struct VertexAndIndex
		{
			Vertex v;
			unsigned int i;
		};
		struct PositionAndVertices
		{
			glm::vec4 position;
			std::vector<VertexAndIndex> vertices;

			Vertex average()
			{
				if (vertices.size() == 0)
				{
					return Vertex{};
				}
				float t = 1.0f / (float)vertices.size();

				Vertex avg = Vertex{};

				for (unsigned int i = 0; i < vertices.size(); i++)
				{
					avg = avg + (vertices[i].v * t);
				}
				return avg;
			}
		};

		class VertexSet
		{
		public:
			VertexSet() {}
			~VertexSet() {}

			// Adds this vec4 to the set, returning if it was already contained
			void add(const Vertex& vertex, unsigned int index)
			{
				for (unsigned int i = 0; i < set.size(); i++)
				{
					// Already contained
					if (vertex.position == set[i].position)
					{
						set[i].vertices.push_back(VertexAndIndex{ vertex, index });
						return;
					}
				}
				set.push_back(PositionAndVertices{ vertex.position });
				set[set.size()-1].vertices.push_back(VertexAndIndex{ vertex, index });
			}

			void print()
			{
				std::cout << "VertexSet" << std::endl;
				for (unsigned int i = 0; i < set.size(); i++)
				{
					std::cout << "Collection " << i << " at " << set[i].position.x << ", " << set[i].position.y << ", " << set[i].position.z << std::endl;
					for (unsigned int j = 0; j < set[i].vertices.size(); j++)
					{
						std::cout << "  " 
							<< set[i].vertices[j].i << ": "
							<< set[i].vertices[j].v.position.x << ", " << set[i].vertices[j].v.position.y << ", " << set[i].vertices[j].v.position.z << std::endl;
					}
				}
			}

			std::vector<unsigned int> generateLookupTableAndNewVertices(unsigned int size, std::vector<Vertex>& vertices)
			{
				std::vector<unsigned int> lookupTable = std::vector<unsigned int>(size);

				for (unsigned int i = 0; i < set.size(); i++)
				{
					for (unsigned int j = 0; j < set[i].vertices.size(); j++)
					{
						lookupTable[set[i].vertices[j].i] = i;
					}

					Vertex avg = set[i].average();

					vertices.push_back(avg);
				}

				return lookupTable;
			}

		private:
			std::vector<PositionAndVertices> set;
		};
	}

	void replaceAll(const std::vector<Vertex>& inVertices, std::vector<unsigned int>& indices, unsigned int replace, unsigned int with)
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
	void removeDuplicateVertices(const std::vector<Vertex>& inVertices,
		const std::vector<unsigned int>& inIndices,
		std::vector<Vertex>& outVertices,
		std::vector<unsigned int>& outIndices)
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
		VertexSet set;

		for (unsigned int i = 0; i < inVertices.size(); i++)
		{
			set.add(inVertices[i], i);
		}

		set.print();
		
		std::vector<unsigned int> lookupTable = set.generateLookupTableAndNewVertices(inIndices.size(), outVertices);
		for (unsigned int i = 0; i < inIndices.size(); i++)
		{
			outIndices.push_back(lookupTable[inIndices[i]]);
		}

		std::cout << outVertices.size() << " vertices." << std::endl;
		for (unsigned int i = 0; i < outVertices.size(); i++)
		{
			std::cout
				<< "Vertex " << i << ": "
				<< outVertices[i].position.x << ", " << outVertices[i].position.y << ", " << outVertices[i].position.z
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
			//return mix(inVertices[a], inVertices[b]);
		}

		// Has vertices across
		std::cout << "Has vertices across" << std::endl;
		return (inVertices[a] * 0.375f) + (inVertices[b] * 0.375f) + (inVertices[across1] * 0.125f) + (inVertices[across2] * 0.125f);
	}

	void subdivide(const std::vector<Vertex>& inVertices, const std::vector<unsigned int>& inIndices, std::vector<Vertex>& outVertices, std::vector<unsigned int>& outIndices)
	{
		//::vector<Vertex> inVertices;

		removeDuplicateVertices(inVertices, inIndices, outVertices, outIndices);

		return;

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
		/*
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
		}*/
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
