#pragma once

#include "Ray.hpp"

namespace CollisionDetection3D
{
	using namespace glm;

	struct Segment final
	{
		Segment(vec3 v1, vec3 v2);
		~Segment();

		vec3 get_v1() const;
		vec3 get_v2() const;

		IntersectInfo<vec3> intersects(const vec3& p) const;
		IntersectInfo<vec3> intersects(const Segment& segment) const;
		IntersectInfo<vec3> intersects(const Ray& r) const;
		IntersectInfo<vec3> intersects(const Triangle& t) const;

		const Ray& get_Ray() const;

	private:
		Ray ray;

		IntersectInfo<vec3> identicalTest(const Segment& s) const;
		IntersectInfo<vec3> segmentsCheck(const Segment& s) const;

		float dir(const vec3& v1, const vec3& v2) const
		{
			if (lq(v1, v2))
				return 1;
			else
				return -1;
		}

		bool lq(const vec3& v1, const vec3& v2) const
		{
			return v1.x <= v2.x && v1.y <= v2.y && v1.z <= v2.z;
		}

		//bool between(const vec3& x, const vec3& y, const vec3& b) const
		//{
		//	vec3 min = x;
		//	vec3 max = y;
		//	if (min.x >= max.x && min.y >= max.y && min.z >= max.z)
		//		swap(min, max);
		//
		//	if (min.x <= b.x && b.x <= max.x)//a b a			
		//		if (min.y <= b.y && b.y <= max.y)//a b a				
		//			if (min.z <= b.z && b.z <= max.z)//a b a					
		//				return true;
		//	return false;
		//}

	};
}