#pragma once
#include "OrientatedPlane.hpp"

namespace CollisionDetection3D
{
	using namespace glm;

	struct Segment;
	struct Ray;
	struct OrientatedPlane;

	inline vec3 trans(const mat4& t, const vec3& c)
	{
		return vec3(t* vec4(c, 1));
	}


	struct Triangle 
	{
		Triangle(const vec3& v1, const vec3& v2, const vec3& v3) : Triangle(v1, v2, v3, mat4(1)){}
		Triangle(const vec3& v1, const vec3& v2, const vec3& v3, const mat4& transform);
		
		IntersectInfo<vec3> intersects(const vec3& v) const;
		IntersectInfo<vec3> intersects(const Segment& t) const;
		IntersectInfo<vec3> intersects(const Ray& t) const;
		IntersectInfo<vec3> intersects(const OrientatedPlane& t) const;
		IntersectInfo<vec3> intersects(const Triangle& t) const;
		
		vec3 getNormal()const
		{
			OrientatedPlane p(v1, v2, v3);
			auto res = p.get_Normal();
			return res;
		}

		vec3 getRawNormal()
		{
			return cross(v3 - v1, v3 - v2);
		}

		const vec3& v1, v2, v3;
		const mat4& transform;


		vec3 v1Cache, v2Cache, v3Cache;

		virtual vec3 getTv1() const
		{	
			return v1Cache;
		}

		virtual vec3 getTv2() const
		{
			return v2Cache;
		}

		virtual vec3 getTv3() const
		{
			return v3Cache;
		}

	private:

		bool triangleCross(const OrientatedPlane& plane) const;

		IntersectInfo<vec3> triangleIntersection(const Triangle& t) const;

		template<typename T>
		IntersectInfo<vec3> edgeIntersection(const  T& r) const;
	};
}


#include "Ray.hpp"