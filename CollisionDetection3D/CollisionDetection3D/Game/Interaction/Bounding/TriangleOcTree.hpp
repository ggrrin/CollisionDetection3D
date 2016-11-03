#include "OcTree.hpp"

namespace CollisionDetection3D
{
	class TriangleOcTree : public OcTree<TriangleIterator, TriangleContainer>
	{
	public:
		TriangleOcTree() : OcTree(){}
		TriangleOcTree(const BoundingBox& bb, const TriangleContainer& tc, size_t maxDepth = 1) :
			OcTree(bb, 
			[&tc]( leafType* b)
		{
			vector<TriangleIterator> stuffs;
			
			for (auto it = tc.begin(); it != tc.end(); ++it)
			{
				if (b->box.intersects(*it))
					stuffs.push_back(it);
			}

			return move(stuffs);
		
		}
			, tc, maxDepth){}

		TriangleOcTree(const TriangleOcTree& t) = delete;
		TriangleOcTree operator=(const TriangleOcTree& t) = delete;

		TriangleOcTree(TriangleOcTree&& t)
		{
			this->root = move(t.root);
			this->position = t.position;
			this->triangleCont = t.triangleCont;
		}

		TriangleOcTree& operator=(TriangleOcTree&& t)
		{
			this->root = move(t.root);
			this->position = t.position;
			this->triangleCont = t.triangleCont;
			return *this;
		}

		TriIntersectInfo intersects(const TriangleOcTree& t) const 
		{
			comparisionCount = 0;
			if (root->triangleCount < 1 || t.root->triangleCount < 1)
				return TriIntersectInfo(false, State::notIntersect);
			else
			{
				cache.prepareCache(triangleCont.size(), t.triangleCont.size());

				auto res = a(root.get(), t.root.get(), t);
				//Debugln("comp:"+to_string(comparisionCount));
				return res;
			}
		}

		TriIntersectInfo a(OcElement* thisNode, OcElement* otherNode, const TriangleOcTree& otherTree) const 
		{
			typedef pair<OcElement*, OcElement*> descendants;
			vector<descendants> possibleDescendants;

			if (!thisNode->isLeave() && !otherNode->isLeave())
			{
				for (const auto& i : thisNode->sons)				
					for (const auto& j : otherNode->sons)
					{
						BoundingBox thisBBox(position + i->box.getMin(), position + i->box.getMax());
						BoundingBox otherBBox(otherTree.position + j->box.getMin(), otherTree.position + j->box.getMax());

						if (thisBBox.intersects(otherBBox))
							possibleDescendants.push_back(descendants(i.get(), j.get()));
					}				
			}
			else if (thisNode->isLeave() && otherNode->isLeave())
			{
				auto thisLeaf = dynamic_cast<OcLeaf<TriangleIterator, TriangleContainer>*>(thisNode);
				auto otherLeaf = dynamic_cast<OcLeaf<TriangleIterator, TriangleContainer>*>(otherNode);

				for (const auto& it0 : thisLeaf->getTriangles())
					for (const auto& it1 : otherLeaf->getTriangles())
					{
						Triangle t0 = *it0;
						Triangle t1 = *it1;
						auto cacheRes = cache.getValue(it0.getTriangleId(), it1.getTriangleId());
						//Debugln("tt " + to_string(it0.getTriangleId()) + "  " + to_string(it1.getTriangleId()));
						
						if (cacheRes.cached)
						{
							if (cacheRes.value)
								return TriIntersectInfo(true, State::intersect, { it0, it1 });
							else
								continue;
						}
						else
						{
							comparisionCount++;
							auto res = t0.intersects(t1);
							if (res.intersect)
								return TriIntersectInfo(true, State::intersect, { it0, it1 });
							else
								cache.setValue(it0.getTriangleId(), it1.getTriangleId(), res.intersect);
						}
					}
				return TriIntersectInfo(false, State::notIntersect);
			}
			else			
				assert(true);			

			if (!possibleDescendants.empty())
			{
				sort(possibleDescendants.begin(), possibleDescendants.end(), [](descendants x, descendants y)
				{ return x.first->triangleCount * x.second->triangleCount < y.first->triangleCount * y.second->triangleCount; });//heuristic

				for (const auto& k : possibleDescendants)
				{
					auto res = a(k.first, k.second, otherTree);
					if (res.intersect)
						return res;
				}

				return TriIntersectInfo(false, State::notIntersect);
			}
			else
			{
				return TriIntersectInfo(false, State::notIntersect);
			}
		}

	private:
		//static Cache<bool> cache;

	};

}