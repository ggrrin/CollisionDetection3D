#pragma once
#include <glm/glm.hpp>
#include <vector>

namespace CollisionDetection3D
{
	using namespace glm;
	using namespace std;

	enum State;

	template<typename T>
	struct IntersectInfo
	{
		IntersectInfo(bool i) : IntersectInfo(i, State::none){}

		IntersectInfo(bool i, State st) : intersect(i), state(st){}
		
		IntersectInfo(bool i, State st, const T& it) : intersectObj(1, it), intersect(i), state(st){}

		IntersectInfo(bool i, State st, initializer_list<T> it) : intersectObj(it), intersect(i), state(st){}

		~IntersectInfo(){}

		bool intersect;

		vector<T> intersectObj;

		State state;

		//intersection
		IntersectInfo<T>& operator+=(const IntersectInfo<T>& inf)
		{
			intersect = intersect && inf.intersect;
			if (intersect)
				intersectObj.insert(intersectObj.end(), inf.intersectObj.begin(), inf.intersectObj.end());
			else
				intersectObj.clear();
			return *this;
		}

		//union
		IntersectInfo<T>& operator*=(const IntersectInfo<T>& inf)
		{
			intersect = intersect || inf.intersect;
			if (intersect)
			{
				state = State::intersect;
				intersectObj.insert(intersectObj.end(), inf.intersectObj.begin(), inf.intersectObj.end());
			}
			else
				intersectObj.clear();
			return *this;
		}

	};

	enum State
	{
		identical, parallel, skewLines, none, liesIn, notIntersect, overLap, intersect
	};
	

}