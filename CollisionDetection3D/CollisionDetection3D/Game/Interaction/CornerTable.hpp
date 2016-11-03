#pragma once
#include <vector>
#include <exception>
#include <glm/glm.hpp>
#include <assert.h>  
#include <unordered_map>
#include "../../ApplicationBase/IDrawable.hpp"
#include "Primitives/Primitives.hpp"


namespace CollisionDetection3D
{
	using namespace std;
	using namespace glm;

	class CornerTable;

	class VertexTriangleIterator
	{
	public:
		VertexTriangleIterator(const CornerTable& cornerTable, int triangle_) :initialTriangle(triangle_), ct(cornerTable), triangle(triangle_){};
		~VertexTriangleIterator(){};
		typedef VertexTriangleIterator self_type;
		typedef Triangle value_type;
		typedef Triangle& reference;
		typedef Triangle* pointer;
		typedef std::forward_iterator_tag iterator_category;

		value_type operator*();
		value_type operator->();
		self_type operator++();
		self_type operator++(int);
		bool operator==(const self_type& it) const;
		bool operator !=(const self_type& it) const;
	private:
		int triangle;
		int initialTriangle;
		const CornerTable& ct;
	};

	class PlaneContainer;

	class PlaneIterator
	{
	public:
		PlaneIterator(PlaneContainer& cont, int triangle_) : ct(&cont), triangle(triangle_){};
		~PlaneIterator(){};
		typedef PlaneIterator self_type;
		typedef OrientatedPlane value_type;
		typedef OrientatedPlane& reference;
		typedef OrientatedPlane* pointer;
		typedef std::forward_iterator_tag iterator_category;

		value_type operator*();
		value_type operator->();
		self_type operator++();
		self_type operator++(int);
		bool operator==(const self_type& it) const;
		bool operator !=(const self_type& it) const;
	private:		
		PlaneContainer* ct;
		int triangle;
	};

	class PlaneContainer
	{
		friend PlaneIterator;
	public:
		typedef PlaneIterator::value_type value_type;

		PlaneContainer(const CornerTable& ct, bool transformed): modelTransformed(transformed),  ct(&ct){}
		~PlaneContainer(){}

		PlaneIterator begin();
		PlaneIterator end();
	private:
		const CornerTable* ct;
		//TODO do folowing things
		bool modelTransformed = false;
		bool caching = false;
		value_type getPlane(int triangleIndex);
	};

	class TriangleIterator
	{
	public:
		TriangleIterator(const CornerTable& cornerTable, int triangle_) : ct(&cornerTable), triangle(triangle_){};
		~TriangleIterator(){};
		typedef TriangleIterator self_type;
		typedef Triangle value_type;
		typedef Triangle& reference;
		typedef Triangle* pointer;
		typedef std::forward_iterator_tag iterator_category;

		value_type operator*() const;
		value_type operator->() const;
		value_type operator[](ptrdiff_t offset) const;
		self_type operator++();
		self_type operator++(int);
		bool operator==(const self_type& it) const;
		bool operator !=(const self_type& it) const;
		int getTriangleId()const
		{
			return triangle / 3;
		}

		vector <TriangleIterator> getTriangleNeigbours();
		
	private:
		const CornerTable* ct;
		int triangle;
	};

	class TriangleContainer
	{
	public:		
		TriangleContainer(){};
		TriangleContainer(const TriangleContainer& c) = default;
		TriangleContainer(const CornerTable& ct) : ct(&ct){}
		bool empty() const;
		size_t size() const;
		Triangle operator[](size_t index) const;
		TriangleIterator begin() const;
		TriangleIterator end() const;
	private:
		const CornerTable* ct;
	};

	class CornerTable : public IDrawable
	{
	public:
		friend class PlaneIterator;
		friend class PlaneContainer;
		friend class TriangleIterator;
		friend class TriangleContainer;

		CornerTable(ApplicationCore& app, vector<vec3> vertices, vector<unsigned int> indices) : vertexBuffer(vertices), indexBuffer(indices), IDrawable(&app) 
		{			
			BuildCornerTable();
			BuildNormals();
		}

		//CornerTable(ApplicationCore& app, vector<vec3>&& vertices, vector<unsigned int>&& indices) : vertexBuffer(vertices), indexBuffer(indices), IDrawable(&app)
		//{
		//	BuildCornerTable();
		//	BuildNormals();
		//}

		virtual ~CornerTable()
		{		}

		PlaneContainer getPlanes() const
		{
			return PlaneContainer(*this, false);
		}

		PlaneContainer getPlanesModelTransformed() const
		{
			return PlaneContainer(*this, true);
		}

		TriangleContainer getTriangles() const
		{
			return TriangleContainer(*this);
		}

		const int nil = -1;

		/// <summary>
		/// Returns triangle handle of the given corner
		/// </summary>
		/// <param name="c">Corner handle</param>
		/// <returns>Triangle handle</returns>
		static int cTriangle(int c)
		{
			return c / 3;
		}

		/// <summary>
		/// Returns handle of the 1st corner of the given triangle
		/// </summary>
		/// <param name="tr">Triangle handle</param>
		/// <returns>Corner handle</returns>
		static int tCorner(int tr)
		{
			return tr * 3;
		}

		/// <summary>
		/// Returns the next corner inside the same triangle
		/// </summary>
		/// <param name="c">Corner handle</param>
		/// <returns>Handle of the next corner</returns>
		static int cNext(int c) 
		{
			return (c % 3 == 2) ? c - 2 : c + 1;
		}

		/// <summary>
		/// Returns the previous corner inside the same triangle
		/// </summary>
		/// <param name="c">Corner handle</param>
		/// <returns>Handle of the previous corner</returns>
		static int cPrev(int c)  
		{
			return (c % 3 == 0) ? c + 2 : c - 1;
		}

		/// <summary>
		/// Returns vertex handle of the given corner
		/// </summary>
		/// <param name="c">Corner handle</param>
		/// <returns>Associated vertex's handle</returns>
		int cVertex(int c) const
		{
			if (c < 0) return nil;
			assert(c < indexBuffer.size());
			return indexBuffer[c];
		}

		const vec3& getVertex(int corrner) const
		{
			//Debugln(to_string(cVertex(corrner)));
			return vertexBuffer.at(cVertex(corrner));
		}

		/// <summary>
		/// Returns opposite corner to the given corner
		/// </summary>
		/// <param name="c">Corner handle</param>
		/// <returns>Handle of the opposite corner</returns>
		int cOpposite(int c) const
		{
			if (c < 0) return nil;
			assert(c < oppositeCornerBuffer.size());
			return oppositeCornerBuffer[c];
		}

		/// <summary>
		/// Returns the "right" corner from the given corner
		/// </summary>
		/// <param name="c">Corner handle</param>
		/// <returns>Corner handle of the "right" triangle</returns>
		int cRight(int c) const 
		{
			return cOpposite(cNext(c));
		}

		/// <summary>
		/// Returns the "left" corner from the given corner
		/// </summary>
		/// <param name="c">Corner handle</param>
		/// <returns>Corner handle of the "left" triangle</returns>
		int cLeft(int c) const 
		{
			return cOpposite(cPrev(c));
		}

		VertexTriangleIterator getVertexTriangleIterator(int cornerHandle)const 
		{
			return VertexTriangleIterator(*this, cornerHandle);
		}
		virtual const vector<unsigned int>& getIndexBuffer() const override
		{
			return indexBuffer;
		}

		virtual const vector<vec3>& getVertexBuffer() const override
		{
			return vertexBuffer;
		}

		virtual const vector<vec3>& getNormalBuffer() const override
		{
			return normalBuffer;
		}

	protected:
		vector<vec3> vertexBuffer;
		vector<unsigned int> indexBuffer;
		vector<vec3> normalBuffer;
		vector<int> oppositeCornerBuffer;

		int edgesCount = 0;
		int sharedEdgesCount = 0;

		void BuildNormals()
		{
			vector<bool> visited(vertexBuffer.size(), false);
			normalBuffer = vector<vec3>(vertexBuffer.size(), vec3(0, 1, 0));

			int normalProcessedCount = 0;
			int corner = 0;
			for (const auto vertexIndex : indexBuffer)
			{
				if (!visited[vertexIndex])
				{
					auto beIt = getVertexTriangleIterator(corner);
					auto cur = VertexTriangleIterator(beIt);
					vec3 normal(0);

					do
					{				
						//TODO predelat se cosinem to je divny
						auto t = *cur;
						auto koef = abs(dot(normalize(t.v2 - t.v1), normalize(t.v3 - t.v1)));
						if (koef == 0)
							koef = 0.1f;
						normal += koef * t.getNormal();
						++cur;
					} while (cur != beIt);

					normalBuffer[vertexIndex] = normalize(normal);
					visited[vertexIndex] = true;

					normalProcessedCount++;
					if (normalProcessedCount == normalBuffer.size())
						break;
				}
				corner++;
			}
		}


		typedef pair<int, int> Edge;

		struct EdgeHash
		{
		public:
			template <typename T, typename U>
			std::size_t operator()(const std::pair<T, U> &x) const
			{
				return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
			}
		};


		/// <summary>
		/// builds the mesh topology (corner-table should be consistent after this call).
		/// </summary>
		void BuildCornerTable()
		{
			if (vertexBuffer.size() < 1 || indexBuffer.size() < 1)
				throw logic_error("corrner table error");

			int n = indexBuffer.size();
			oppositeCornerBuffer = vector<int>();
			for (int i = 0; i < n; i++)
				oppositeCornerBuffer.push_back(nil);
			unordered_map<Edge, int, EdgeHash> edges;

			edgesCount = sharedEdgesCount = 0;
			for (int i = 0; i < n; i++) // process one corner
			{
				int cmin = cVertex(cPrev(i));
				int cmax = cVertex(cNext(i));
				if (cmin < 0 || cmax < 0) continue;

				if (cmin > cmax)
					swap(cmin, cmax);

				Edge edge(cmin, cmax);

				auto otherIt = edges.find(edge);
				if (otherIt != edges.end())
				{
					int other = otherIt->second;

					assert(oppositeCornerBuffer[other] == nil);

					oppositeCornerBuffer[other] = i;
					oppositeCornerBuffer[i] = other;
					edges.erase(edge);
					sharedEdgesCount++;
				}
				else
				{
					edges.insert(make_pair(edge, i));
					edgesCount++;
				}
			}
		}
	};



}