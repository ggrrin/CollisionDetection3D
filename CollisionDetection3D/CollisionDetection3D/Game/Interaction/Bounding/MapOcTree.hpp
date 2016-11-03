#pragma once
#include "OcTree.hpp"
#include "../../AppComponents/HeightMap.hpp"

namespace CollisionDetection3D
{
	class MapOcTree : public OcTree<shared_ptr<HeightMap>, vector<shared_ptr<HeightMap>>>
	{
	public:
		MapOcTree() : OcTree(){}

		MapOcTree(const BoundingBox& bb, const vector<shared_ptr<HeightMap>>& tc, size_t maxDepth, bool removeEmpty) :
			OcTree(bb,
			[&tc](leafType* b)
		{
			vector<shared_ptr<HeightMap>> stuffs;

			for (const auto& map : tc)			
				if (map->intersects(b->box).intersect)				
					stuffs.push_back(map);			

			return move(stuffs);

		}, tc, maxDepth, removeEmpty){}

		MapOcTree(const TriangleOcTree& t) = delete;
		MapOcTree operator=(const MapOcTree& t) = delete;

		MapOcTree(MapOcTree&& t)
		{
			this->root = move(t.root);
			this->position = t.position;
			this->triangleCont = t.triangleCont;
		}

		MapOcTree& operator=(MapOcTree&& t)
		{
			this->root = move(t.root);
			this->position = t.position;
			this->triangleCont = t.triangleCont;
			return *this;
		}

	private:


	};

}