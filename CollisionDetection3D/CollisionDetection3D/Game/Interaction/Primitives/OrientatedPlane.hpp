#pragma once
#include <glm/glm.hpp>
#include "../Support/GSElimination.hpp"
#include "../Support/IntersectInfo.hpp"
#include <assert.h>

namespace CollisionDetection3D
{
	using namespace glm;

	struct Ray;
	struct Segment;
	struct Triangle;

	struct OrientatedPlane final
	{
		//a*x + b*y + c*z + d = 0
		float get_A() const;
		float get_B() const;
		float get_C() const;
		float get_D() const;
		vec3 get_Normal() const;

		//righthanded order (normla is thumb of right hand)
		OrientatedPlane(vec3 x, vec3 y, vec3 z);
		OrientatedPlane(vec3 x, vec3 normal);
		~OrientatedPlane();

		void invertPlane();

		float distance(const vec3&  x) const;
		bool halfIntersect(const vec3& x) const;
		bool liesIn(const vec3&  x, float epsilon = GlobalEpsilon) const;

		IntersectInfo<vec3> intersect(const vec3& s) const;
		IntersectInfo<vec3> intersect(const Segment& s) const;
		IntersectInfo<vec3> intersect(const Ray& p) const;
		IntersectInfo<Ray> intersect(const OrientatedPlane& p) const;
		IntersectInfo<vec3> intersect(const Triangle& p) const;

	private:
		vec4 data;
		vec3 point;
	};
}

#include "Ray.hpp"


