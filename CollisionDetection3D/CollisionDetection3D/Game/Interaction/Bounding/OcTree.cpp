#include "OcTree.hpp"

namespace CollisionDetection3D
{


	OcElement::OcElement(OcElem_ptr c1, OcElem_ptr c2, OcElem_ptr c3, OcElem_ptr c4, OcElem_ptr c5, OcElem_ptr c6, OcElem_ptr c7, OcElem_ptr c8, bool removeEmpty) :triangleCount(0), sons(8)
	{
		sons[0] = move(c1); sons[1] = move(c2);
		sons[2] = move(c3); sons[3] = move(c4);
		sons[4] = move(c5); sons[5] = move(c6);
		sons[6] = move(c7); sons[7] = move(c8);

		vec3 min, max;
		min = max = sons[0]->box.getMin();
		for (const auto& k : sons)
		{
			min = glm::min(min, k->box.getMin());
			max = glm::max(max, k->box.getMax());

			triangleCount += k->triangleCount;
		}

		box = BoundingBox(min, max);
		
		//Changed !!!!!!!!
		if (removeEmpty)
			sons.erase(remove_if(sons.begin(), sons.end(), [](const OcElem_ptr& x){ return  x->triangleCount < 1; }), sons.end());
	}

	




	

}