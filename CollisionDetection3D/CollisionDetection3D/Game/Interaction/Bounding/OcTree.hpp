#pragma once
#include "BoundingBox.hpp"
#include <memory>
#include <vector>
#include "../CornerTable.hpp"
#include "../../Cache.hpp"
#include <functional>

namespace CollisionDetection3D
{
	using namespace std;

	struct BoundingBox;

	struct OcElement;
	typedef unique_ptr<OcElement> OcElem_ptr;

	
	class OcElement
	{
	public:
		OcElement(OcElem_ptr c1, OcElem_ptr c2, OcElem_ptr c3, OcElem_ptr c4, OcElem_ptr c5, OcElem_ptr c6, OcElem_ptr c7, OcElem_ptr c8, bool removeEmpty);

		virtual bool isLeave() const
		{
			//return sons.empty();// there is never node with empty sons => node with 0 triangles =>
			//=> node has no sons => it is root
			return false;
		}

		OcElement(const BoundingBox& box) : triangleCount(0), box(box){};
		BoundingBox box;
		vector<OcElem_ptr> sons;
		size_t triangleCount;
	};

	template<typename Stuff, typename StuffContainer>
	class OcLeaf : public OcElement
	{
	public:		
		typedef typename OcLeaf<Stuff,StuffContainer>  me;
		OcLeaf(const BoundingBox& b, function<vector<Stuff>( me*)> getIntersectStuff, vec3 position) : OcElement(b), pos(position)
		{
			//for (auto it = cc.begin(); it != cc.end(); ++it)
			//{
			//	if (box.intersects(*it))
			//		stuffs.push_back(it);
			//}

			stuffs = std::move(getIntersectStuff(this));

			triangleCount = stuffs.size();
		}

		virtual bool isLeave() const override
		{
			return true;
		}

		const vector<Stuff>& getTriangles() const
		{
			return stuffs;
		}
		vector<Stuff> stuffs;

		vec3 pos;
	private:
	};

	template<typename Stuff, typename StuffContainer>
	class OcTree
	{
	public:
		typedef OcLeaf<typename Stuff, typename StuffContainer> leafType;
		typedef vector<OcElem_ptr> first_d;
		typedef vector<first_d> second_d;
		typedef vector<second_d> uniformArray;

		OcTree(){};
		OcTree(const BoundingBox& bb, function<vector<Stuff>( leafType*)> getIntersectStuff, const StuffContainer& tc, size_t maxDepth = 2, bool removeEmpty = false) : triangleCont(tc)
		{
			triangleCont = StuffContainer(tc);
			size_t count = pow(8, maxDepth);
			size_t side = pow(2, maxDepth);

			assert(side != 0);

			uniformArray prevLayer = initializeUniformArray(side);

			vec3  suBBDiagonal(bb.getXSize() / side, bb.getYSize() / side, bb.getZSize() / side);

			//lowest layer => tree leaves
			for (size_t x = 0; x < side; x++)
			{
				for (size_t y = 0; y < side; y++)
				{
					for (size_t z = 0; z < side; z++)
					{
						vec3 min, max;

						min = bb.getMin();
						min.x += (x / (float)side) * bb.getXSize();
						min.y += (y / (float)side) * bb.getYSize();
						min.z += (z / (float)side) * bb.getZSize();

						max = min + suBBDiagonal;

						prevLayer[x][y][z] = make_unique<OcLeaf<Stuff, StuffContainer>>(BoundingBox(min, max), getIntersectStuff, vec3(x,y,z));
					}
				}
			}

			//building tree
			for (size_t i = 1; i <= maxDepth; i++)
			{
				uniformArray layer = initializeUniformArray(pow(2, maxDepth - i));

				size_t prevSize = pow(2, maxDepth - i + 1);
				for (size_t x = 0; x < prevSize; x += 2)
				{
					for (size_t y = 0; y < prevSize; y += 2)
					{
						for (size_t z = 0; z < prevSize; z += 2)
						{
							OcElem_ptr node = make_unique<OcElement>(
								move(prevLayer[x][y][z]), move(prevLayer[x + 1][y][z]),
								move(prevLayer[x][y + 1][z]), move(prevLayer[x][y][z + 1]),
								move(prevLayer[x + 1][y][z + 1]), move(prevLayer[x + 1][y + 1][z + 1]),
								move(prevLayer[x][y + 1][z + 1]), move(prevLayer[x + 1][y + 1][z]), removeEmpty);
							layer[x / 2][y / 2][z / 2] = move(node);
						}
					}
				}

				prevLayer = move(layer);
			}

			root = move(prevLayer[0][0][0]);
		}

		OcTree(const OcTree& t) = delete;
		OcTree operator=(const OcTree& t) = delete;
				
		OcTree(OcTree&& t)
		{
			this->root = move(t.root);
			this->position = t.position;
			this->triangleCont = t.triangleCont;			
		}

		OcTree& operator=(OcTree&& t)
		{
			this->root = move(t.root);
			this->position = t.position;
			this->triangleCont = t.triangleCont;
			return *this;
		}

		typedef IntersectInfo<TriangleIterator> TriIntersectInfo;

		static size_t comparisionCount;


		void translate(const vec3& translation)
		{
			position += translation;
		}

		const OcElement* getRoot() const
		{
			return root.get();
		}

		const vec3& getPosition() const
		{
			return position;
		}

		void forEeachLeafIf(function<void(leafType*)> f, function<bool(OcElement*)> predicate)
		{
			stepInto(root.get(), f, predicate);
		}

		void forEeachLeaf(function<void(leafType*)> f)
		{
			stepInto(root.get(), f, [](OcElement*){return true; });
		}

	protected:

		void stepInto(OcElement* thisNode, function<void(leafType*)> f, function<bool(OcElement*)> predicate) const
		{
			if (!thisNode->isLeave())
			{
				for (const auto& i : thisNode->sons)
					if (predicate(i.get()))
						stepInto(i.get(), f, predicate);

			}
			else
			{
				f(dynamic_cast<leafType*>(thisNode));
			}
		}


		vec3 position;

		uniformArray OcTree::initializeUniformArray(size_t side) const
		{
			uniformArray q(side);
			for (size_t i = 0; i < side; i++)
			{
				q[i] = second_d(side);
				for (size_t j = 0; j < side; j++)
					q[i][j] = first_d(side);
			}

			return move(q);
		}

		OcElem_ptr root;
		StuffContainer triangleCont;

		static Cache<bool> cache;

	};

	template<typename Stuff, typename StuffContainer>
	size_t OcTree<Stuff, StuffContainer>::comparisionCount = 0;

	template<typename Stuff, typename StuffContainer>
	Cache<bool> OcTree<Stuff, StuffContainer>::cache = Cache<bool>();

}