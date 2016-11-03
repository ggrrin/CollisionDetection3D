#include "Ray.hpp"

namespace CollisionDetection3D
{
	float Ray::epsilon = GlobalEpsilon;

	Ray::Ray(vec3 point, vec3 direction) : point(point), direction(direction){}

	Ray::~Ray(){}

	IntersectInfo<vec3> Ray::intersects(const Triangle& t) const
	{
		return t.intersects(*this);
	}

	IntersectInfo<vec3> Ray::intersects(const OrientatedPlane& p) const
	{
		return p.intersect(*this);
	}
	
	IntersectInfo<vec3> Ray::intersects(const Ray& r) const
	{
		if (linearDep(direction, r.direction))
		{
			if (intersects(r.point).intersect)
				return IntersectInfo<vec3>(true, State::identical);//identical
			else
				return IntersectInfo<vec3>(false, State::parallel);//parallel
		}
		else
		{
			OrientatedPlane plane(point, point + direction, point + r.direction);
			if (!plane.liesIn(r.point, epsilon))
				return IntersectInfo<vec3>(false, State::skewLines); //mimobezky
			else
			{
				mat3 invProj(r.direction, direction, cross(direction, r.direction));
				mat3 proj = inverse(invProj);

				//point -> posuneme do (0,0,0) -->
				//-->posun r.point
				vec3 a = r.point - point;

				vec3 invR2 = proj * a;
				vec3 invInter(0, invR2.y, 0);

				vec3 inter = (invProj * invInter) + point;
				return IntersectInfo<vec3>(true, State::intersect, inter);
			}
		}
	}

	IntersectInfo<vec3> Ray::intersects(const Segment& s) const
	{
		return s.intersects(*this);
	}

	IntersectInfo<vec3> Ray::intersects(const vec3& p) const
	{
		//X-Q=kp
		if (linearDep(p - point, direction))
			return IntersectInfo<vec3>(true, State::liesIn, p);
		else
			return IntersectInfo<vec3>(false, State::notIntersect);
	}

	vec3 Ray::get_v1() const
	{
		return point;
	}

	vec3 Ray::get_direction() const
	{
		return direction;
	}

	bool Ray::linearDep(const vec3& v1, const vec3& v2) const
	{
		return GSElimination<mat2x3>(mat2x3(v1, v2)).Elimine() < 2;
	}
}