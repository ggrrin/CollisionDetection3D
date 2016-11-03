#include "Triangle.hpp"

namespace CollisionDetection3D
{
	Triangle::Triangle(const vec3& v1, const vec3& v2, const vec3& v3, const mat4& transform) :
		v1(v1), v2(v2), v3(v3), transform(transform), v1Cache(trans(transform, v1)), v2Cache(trans(transform, v2)), v3Cache(trans(transform, v3))  {
	
	}

	IntersectInfo<vec3> Triangle::intersects(const Triangle& t) const
	{

		OrientatedPlane p1(getTv1(), getTv2(), getTv3());
		OrientatedPlane p2(t.getTv1(), t.getTv2(), t.getTv3());

		if (!(triangleCross(p2) && t.triangleCross(p1)))
			return IntersectInfo<vec3>(false, State::notIntersect);

		auto info = p1.intersect(p2);
		if (info.state == State::identical)		
			return triangleIntersection(t);		
		else
		{
			Ray& intersection = info.intersectObj[0];

			auto res1 = edgeIntersection(intersection);
			auto res2 = t.edgeIntersection(intersection);

			if (res1.intersect && res2.intersect)
			{
				if (res1.intersectObj.size() == 2 && res2.intersectObj.size() == 2)
				{
					Segment s1(res1.intersectObj[0], res1.intersectObj[1]);
					Segment s2(res2.intersectObj[0], res2.intersectObj[1]);

					return s1.intersects(s2);
				}
				else if (res1.intersectObj.size() == 1 && res2.intersectObj.size() == 1)
				{
					if (res1.intersectObj[0] == res2.intersectObj[1])//TODO epsilon rovnost!!!			
						return IntersectInfo<vec3>(true, State::intersect, res1.intersectObj[0]);
					else
						return IntersectInfo<vec3>(false, State::notIntersect);
				}
				else if (res1.intersectObj.size() == 1)
				{
					Segment s(res2.intersectObj[0], res2.intersectObj[1]);
					return s.intersects(res1.intersectObj[0]);
				}
				else if (res2.intersectObj.size() == 1)
				{
					Segment s(res1.intersectObj[0], res1.intersectObj[1]);
					return s.intersects(res2.intersectObj[0]);
				}
				assert(true);
			}
			else
			{
				return res1 += res2;
			}
		}
	}

	IntersectInfo<vec3> Triangle::intersects(const OrientatedPlane& t) const
	{
		OrientatedPlane tPlane(getTv1(), getTv2(), getTv3());
		auto res = tPlane.intersect(t);
		if (res.state == State::identical)		
			return IntersectInfo<vec3>(true, State::identical, { getTv1(), getTv2(), getTv3() });
		else		
			return edgeIntersection(res.intersectObj[0]);
	}

	IntersectInfo<vec3> Triangle::intersects(const Segment& s) const
	{
		OrientatedPlane plane(getTv1(), getTv2(), getTv3());
		auto res = plane.intersect(s);
		if (!res.intersect)
			return res;
		else
		{
			if (res.state == State::identical)
				return edgeIntersection(s);
			else
				return intersects(res.intersectObj[0]);
		}
	}

	IntersectInfo<vec3> Triangle::intersects(const Ray& r) const
	{
		OrientatedPlane plane(getTv1(), getTv2(), getTv3());
		auto res = plane.intersect(r);
		if (!res.intersect)
			return res;
		else
		{
			if (res.state == State::identical)			
				return edgeIntersection(r);			
			else
				return intersects(res.intersectObj[0]);
		}
	}

	IntersectInfo<vec3> Triangle::intersects(const vec3& v) const
	{
		vec3 edgeX(getTv2() - getTv1());
		vec3 edgeY(getTv3() - getTv1());

		mat3 invProj(edgeX, edgeY, cross(edgeX, edgeY));
		mat3 proj(inverse(invProj));

		vec3 constr = v - getTv1(); //move getTv1() to orign
		vec3 projConstr = proj * constr;

		if (abs(projConstr.z) > GlobalEpsilon || projConstr.x < 0 || projConstr.y < 0 || projConstr.y >(-projConstr.x + 1))//f(x) = -x + 1;
			return IntersectInfo<vec3>(false, State::notIntersect);
		else
			return IntersectInfo<vec3>(true, State::intersect, v);
	}

	template<typename T>
	IntersectInfo<vec3> Triangle::edgeIntersection(const  T& r) const
	{
		IntersectInfo<vec3> results(true, State::intersect);
		Segment edges[] = {
			Segment(getTv1(), getTv2()),
			Segment(getTv2(), getTv3()),
			Segment(getTv3(), getTv1())
		};

		for (const auto& i : edges)
		{
			IntersectInfo<vec3> res = i.intersects(r);
			if (res.state == State::identical)//narazili jsme na identickou hranu => mame vse co potrebujem
			{
				results.intersectObj = res.intersectObj;
				break;
			}

			if (res.intersect)
			{
				if (!results.intersectObj.empty() && results.intersectObj[0] == res.intersectObj[0])//pokud protina roh trojuhelnika
					break;//TODO : NA to se nemuzu spolehat
				else//2 body pruniku
					results.intersectObj.push_back(res.intersectObj[0]);
			}
		}

		if (results.intersectObj.size() < 1)
		{
			results.state = State::notIntersect;
			results.intersect = false;
		}

		return results;
	}

	bool Triangle::triangleCross(const OrientatedPlane& plane) const
	{
		float a = plane.distance(getTv1());
		float b = plane.distance(getTv2());
		float c = plane.distance(getTv3());

		if (a == 0 || b == 0 || c == 0)
			return true;
		else
			return !(signbit(a) && signbit(b) && signbit(c));//vsechny 3 nejsou na stejny strane
	}

	IntersectInfo<vec3> Triangle::triangleIntersection(const Triangle& t) const
	{
		IntersectInfo<vec3> results(true, State::overLap);
		Segment thisEdges[] = {
			Segment(getTv1(), getTv2()),
			Segment(getTv2(), getTv3()),
			Segment(getTv3(), getTv1())
		};

		Segment otherEdges[] = {
			Segment(t.getTv1(), t.getTv2()),
			Segment(t.getTv2(), t.getTv3()),
			Segment(t.getTv3(), t.getTv1())
		};

		for (const auto& i : thisEdges)
			for (const auto& j : otherEdges)
			{
				IntersectInfo<vec3> res = i.intersects(j);
				if (res.intersect)
					results.intersectObj.push_back(res.intersectObj[0]);//TODO unique points !!!! is that problem ? duplications...
			}

		if (results.intersectObj.size() < 1)
		{
			results.state = State::notIntersect;
			results.intersect = false;
		}
		return results;
	}



}