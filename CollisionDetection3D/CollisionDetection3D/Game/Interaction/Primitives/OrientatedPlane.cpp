#include "OrientatedPlane.hpp"

namespace CollisionDetection3D
{
	OrientatedPlane::OrientatedPlane(vec3 x, vec3 normal) :point(x), data(normalize(normal), -dot(normalize(normal), x)){}

	OrientatedPlane::OrientatedPlane(vec3 x, vec3 y, vec3 z) : OrientatedPlane(x, cross(x - y, x - z))
	{}

	OrientatedPlane::~OrientatedPlane(){};


	IntersectInfo<vec3> OrientatedPlane::intersect(const Triangle& p) const
	{
		return p.intersects(*this);
	}

	IntersectInfo<vec3> OrientatedPlane::intersect(const vec3& v) const
	{		
		if (liesIn(v))
			return IntersectInfo<vec3>(true, State::liesIn, v);
		else
			return IntersectInfo<vec3>(false, State::notIntersect);
	}

	IntersectInfo<Ray> OrientatedPlane::intersect(const OrientatedPlane& p) const
	{
		IntersectInfo<Ray> r(true,State::intersect);

		mat2x4 a(data, p.data);
		GSElimination<mat2x4> e(a);		
		int rank = e.Elimine();
		if (rank < 2)
			r.state = State::identical;
		else
		{
			auto res = e.Result<vec3, float>();
			r.intersectObj.push_back(Ray(res[1], res[0]));
		}

		return r;
	}
	
	float OrientatedPlane::get_A() const { return data.x; };
	float OrientatedPlane::get_B() const { return data.y; };
	float OrientatedPlane::get_C() const { return data.z; };
	float OrientatedPlane::get_D() const { return data.w; };
	vec3 OrientatedPlane::get_Normal() const {
		return std::move(vec3(data)); 
	}

	void OrientatedPlane::invertPlane()
	{
		data *= -1;
	}

	float OrientatedPlane::distance(const vec3&  x) const
	{
		float distance = dot(get_Normal(), x - point);
		return distance;
	}

	//x is on side with normal
	bool OrientatedPlane::halfIntersect(const vec3& x) const
	{
		return distance(x) >= 0;
	}

	bool OrientatedPlane::liesIn(const vec3&  x, float epsilon) const
	{
		return abs(distance(x)) <= epsilon;
	}

	IntersectInfo<vec3> OrientatedPlane::intersect(const Segment& s) const
	{
		auto res = intersect(s.get_Ray());
		if (!res.intersect || res.state == State::liesIn)
			return res;
		else
		{
			if (liesIn(res.intersectObj[0]))
				return res;
			else
				return IntersectInfo<vec3>(false, State::notIntersect);
		}
	}

	IntersectInfo<vec3> OrientatedPlane::intersect(const Ray& p) const
	{
		if (liesIn(p.get_v1()) && liesIn(p.get_v1() + p.get_direction()))
			return IntersectInfo<vec3>(true, State::liesIn, { p.get_v1(), p.get_v1() + p.get_direction() });
		else if (!liesIn(point + p.get_direction()))
			return IntersectInfo<vec3>(false, State::parallel);
		else
		{
			vec3 direction(p.get_direction());
			vec3 v(direction.y, direction.z, direction.x);

			vec3 n1(cross(direction, v));
			vec3 n2(cross(n1, direction));

			OrientatedPlane p1(p.get_v1(), n1);
			OrientatedPlane p2(p.get_v1(), n2);

			GSElimination<mat3x4> a(mat3x4(data, p1.data, p2.data));
			int rank = a.Elimine();
			assert(rank != 3);
			
			return IntersectInfo<vec3>(true, State::intersect, vec3(a.Result<vec4, float>()[0]));
		}

	}
}