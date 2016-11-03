#include "OcTree.hpp"
#include "../../Body.hpp"


namespace CollisionDetection3D
{

	class ObjOcTree : public OcTree<shared_ptr<Body>, vector<shared_ptr<Body>>>
	{
	public:
		ObjOcTree() : OcTree(){}

		ObjOcTree(const BoundingBox& bb, const vector<shared_ptr<Body>>& tc, size_t maxDepth) :
			OcTree(bb,
			[&tc]( leafType* b)
		{
			vector<shared_ptr<Body>> stuffs;

			for (const auto& co : tc)
			{
				if (b->box.intersects(co->getBBox()))
				{					
					stuffs.push_back(co);
					co->nodes.push_back(b);
				}
			}

			return move(stuffs);

		}  , tc, maxDepth){}

		ObjOcTree(const TriangleOcTree& t) = delete;
		ObjOcTree operator=(const ObjOcTree& t) = delete;

		ObjOcTree(ObjOcTree&& t)
		{
			this->root = move(t.root);
			this->position = t.position;
			this->triangleCont = t.triangleCont;
		}

		ObjOcTree& operator=(ObjOcTree&& t)
		{
			this->root = move(t.root);
			this->position = t.position;
			this->triangleCont = t.triangleCont;
			return *this;
		}

		vector<leafType*> getCollidingLeaves(const BoundingBox& bb)
		{
			vector<leafType*> dat;
			ac(root.get(), bb, dat);

			return move(dat);
		}

		void ac(OcElement* node, const BoundingBox& bb, vector<leafType*>& dat)
		{
			if (node->box.intersects(bb))
			{
				if (node->isLeave())
				{
					dat.push_back(dynamic_cast<leafType*>(node));
				}
				else
				{
					for (auto& a : node->sons)					
						ac(a.get(),bb, dat);					
				}
			}
		}


	private:


	};

}