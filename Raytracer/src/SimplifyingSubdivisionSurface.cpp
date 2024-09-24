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
					addNonDuplicate(outIndices, inIndices[i + 1]);
					addNonDuplicate(outIndices, inIndices[i + 2]);
				}
				else if (inIndices[i + 1] == index)
				{
					addNonDuplicate(outIndices, inIndices[i + 0]);
					addNonDuplicate(outIndices, inIndices[i + 2]);
				}
				else if (inIndices[i + 2] == index)
				{
					addNonDuplicate(outIndices, inIndices[i + 0]);
					addNonDuplicate(outIndices, inIndices[i + 1]);
				}
			}
		}

		class VertexSet
		{
		public:
			VertexSet() {}
			~VertexSet() {}

			// Adds this vec4 to the set, returning if it was already contained
			void add(const Vertex& vertex, unsigned int index)
			{
				float minDistNotAccepted = 100.0f;
				for (unsigned int i = 0; i < set.size(); i++)
				{
					// Already contained
					if (close(vertex.position, set[i].position))
					{
						set[i].vertices.push_back(VertexAndIndex{ vertex, index });
						return;
					}
					float dist = glm::distance(vertex.position, set[i].position);
					if (dist < minDistNotAccepted)
					{
						minDistNotAccepted = dist;
					}
				}
				//std::cout << "min dist not accepted: " << minDistNotAccepted << std::endl;
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

			bool close(glm::vec4 a, glm::vec4 b)
			{
				glm::vec4 d = (a - b);
				return (d.x * d.x + d.y * d.y + d.z * d.z) < 0.00001f;
			}
		};
	}

	// Not finished
	void removeDuplicateVertices(const std::vector<Vertex>& inVertices,
		const std::vector<unsigned int>& inIndices,
		std::vector<Vertex>& outVertices,
		std::vector<unsigned int>& outIndices)
	{
		/*
		std::cout << inVertices.size() << " vertices." << std::endl;
		for (unsigned int i = 0; i < inVertices.size(); i++)
		{
			std::cout
				<< "Vertex " << i << ": "
				<< inVertices[i].position.x << ", " << inVertices[i].position.y << ", " << inVertices[i].position.z
				<< std::endl;
		}
		*/

		// Merge vertices with equal positions
		VertexSet set;

		for (unsigned int i = 0; i < inVertices.size(); i++)
		{
			set.add(inVertices[i], i);
		}

		//set.print();
		
		std::vector<unsigned int> lookupTable = set.generateLookupTableAndNewVertices(inIndices.size(), outVertices);
		for (unsigned int i = 0; i < inIndices.size(); i++)
		{
			outIndices.push_back(lookupTable[inIndices[i]]);
		}
		/*
		std::cout << outVertices.size() << " vertices." << std::endl;
		for (unsigned int i = 0; i < outVertices.size(); i++)
		{
			std::cout
				<< "Vertex " << i << ": "
				<< outVertices[i].position.x << ", " << outVertices[i].position.y << ", " << outVertices[i].position.z
				<< std::endl;
		}
		*/
	}

	int getLast(const std::vector<Vertex>& inVertices, const std::vector<unsigned int>& inIndices, unsigned int i1, unsigned int i2, unsigned int i_not)
	{
		for (unsigned int i = 0; i < inIndices.size(); i += 3)
		{
			unsigned int i1t = inIndices[i + 0];
			unsigned int i2t = inIndices[i + 1];
			unsigned int i3t = inIndices[i + 2];

			if ((i1t == i1 && i2t == i2 || i1t == i2 && i2t == i1) && i3t != i_not)
			{
				return inIndices[i + 2];
			}
			if ((i2t == i1 && i3t == i2 || i2t == i2 && i3t == i1) && i1t != i_not)
			{
				return inIndices[i + 0];
			}
			if ((i1t == i1 && i3t == i2 || i1t == i2 && i3t == i1) && i2t != i_not)
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
			return (inVertices[a] * 0.5f) + (inVertices[b] * 0.5f);
		}

		// Has vertices across
		return (inVertices[a] * 0.375f) + (inVertices[b] * 0.375f) + (inVertices[across1] * 0.125f) + (inVertices[across2] * 0.125f);
	}

	void subdivide(const std::vector<Vertex>& inVertices, const std::vector<unsigned int>& inIndices, std::vector<Vertex>& outVertices, std::vector<unsigned int>& outIndices)
	{
		//::vector<Vertex> inVertices;

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

			i4 = outVertices.size();
			outVertices.push_back(v4);
			i5 = outVertices.size();
			outVertices.push_back(v5);
			i6 = outVertices.size();
			outVertices.push_back(v6);

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

			Vertex result = inVertices[i] * (1.0f - beta * n);

			/*
			std::cout << "Neighbours of "
				<< inVertices[i].position.x << ", " << inVertices[i].position.y << ", " << inVertices[i].position.z << std::endl;
			for (unsigned int i = 0; i < neighbours.size(); i++)
			{
				unsigned int neighbourIndex{ neighbours[i] };

				std::cout
					<< "Vertex " << i << ": "
					<< inVertices[neighbourIndex].position.x << ", " << inVertices[neighbourIndex].position.y << ", " << inVertices[neighbourIndex].position.z
					<< std::endl;
			}*/

			for (unsigned int j = 0; j < neighbours.size(); j++)
			{
				result = result + inVertices[neighbours[j]] * beta;
			}


			outVertices[i] = result;
		}
	}

	void subdivide(const std::vector<Vertex>& inVertices,
		const std::vector<unsigned int>& inIndices,
		std::vector<Vertex>& outVertices,
		std::vector<unsigned int>& outIndices,
		unsigned int times)
	{
		// No subdivision applied
		if (times == 0)
		{
			outVertices = inVertices;
			outIndices = inIndices;
			return;
		}

		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		removeDuplicateVertices(inVertices, inIndices, vertices, indices);

		for (unsigned int i = 0; i < times; i++)
		{
			/*
			for (unsigned int i = 0; i < vertices.size(); i++)
			{
				std::cout
					<< "Vertex " << i << ": "
					<< vertices[i].position.x << ", " << vertices[i].position.y << ", " << vertices[i].position.z
					<< std::endl;
			}

			std::cout << "Triangles" << std::endl;
			for (unsigned int i = 0; i < indices.size(); i += 3)
			{
				std::cout
					<< "Tri " << (i / 3) << ": "
					<< indices[i + 0] << ", " << indices[i + 1] << ", " << indices[i + 2]
					<< std::endl;
			}
			*/

			subdivide(vertices, indices, outVertices, outIndices);

			// Not final iteration: will require new vertices and indices
			if (i < times - 1)
			{
				//indices = outIndices;
				//vertices = outVertices;
				indices.clear();
				vertices.clear();
				removeDuplicateVertices(outVertices, outIndices, vertices, indices);
				outVertices.clear();
				outIndices.clear();

			}
		}

	}
}
