#pragma once
//#include "StoredTriangle.hpp"
#include "Triangle.hpp" //included via StoredTriangle
//#include "Segment.hpp" included at EOF

namespace CollisionDetection3D
{
	using namespace glm;	

	struct Segment;
	struct OrientatedPlane;
	struct Triangle;

	struct Ray final
	{
		static float epsilon;

		Ray(vec3 point, vec3 direction);
		~Ray();
	
		IntersectInfo<vec3> intersects(const vec3& p) const;
		IntersectInfo<vec3> intersects(const Segment& s) const;
		IntersectInfo<vec3> intersects(const Ray& r) const;
		IntersectInfo<vec3> intersects(const OrientatedPlane& r) const;
		IntersectInfo<vec3> intersects(const Triangle& r) const;

		vec3 get_v1() const;
		vec3 get_direction() const;

	private:
		vec3 point, direction;
		bool linearDep(const vec3& v1, const vec3& v2) const;
	};
}

#include "Segment.hpp"

