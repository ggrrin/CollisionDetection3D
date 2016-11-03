#include "Segment.hpp"


namespace CollisionDetection3D
{
	Segment::Segment(vec3 v1, vec3 v2) : ray(v1, v2 - v1){}

	Segment::~Segment(){}

	vec3 Segment::get_v1() const
	{
		return ray.get_v1();
	}

	vec3 Segment::get_v2() const
	{
		return ray.get_v1() + ray.get_direction();
	}

	IntersectInfo<vec3> Segment::intersects(const Triangle& t) const
	{
		return t.intersects(*this);
	}

	IntersectInfo<vec3> Segment::intersects(const Segment& segment) const
	{
		auto baseRay = ray.intersects(segment.ray);
		if (!baseRay.intersect)
			return baseRay;
		else
		{
			if (baseRay.state == State::identical)
			{			
				return segmentsCheck(segment);
			}
			else
			{
				if (intersects(baseRay.intersectObj[0]).intersect && segment.intersects(baseRay.intersectObj[0]).intersect)
					return baseRay;
				else
					return IntersectInfo<vec3>(false, State::notIntersect);
			}
		}
	}

	IntersectInfo<vec3> Segment::identicalTest(const Segment& segment) const
	{
		auto res1 = intersects(segment.get_v1());
		auto res2 = intersects(segment.get_v2());
		if (res1.intersect && res2.intersect)
			return IntersectInfo<vec3>(true, State::liesIn, { segment.get_v1(), segment.get_v2() });
		else if (res1.intersect || res2.intersect)
		{
			auto res = IntersectInfo<vec3>(true, State::intersect, res1.intersect ? res1.intersectObj[0] : res2.intersectObj[0]);

			auto vres1 = segment.intersects(get_v1());
			auto vres2 = segment.intersects(get_v2());

			if (vres1.intersect || vres2.intersect)
				res.intersectObj.push_back(vres1.intersect ? vres1.intersectObj[0] : vres2.intersectObj[0]);

			//if (res.intersectObj[0] == res.intersectObj[1]) //TODO epsilon rovnost!!!
			//	res.intersectObj.erase(res.intersectObj.begin());

			return res;
		}

		return 	IntersectInfo<vec3>(false, State::notIntersect);
	}

	IntersectInfo<vec3> Segment::segmentsCheck(const Segment& s) const
	{
		auto res1 = identicalTest(s);
		auto res2 = s.identicalTest(*this);

		if (res1.intersect)
			return res1;
		else if (res2.intersect)
			return res2;
		else
			return IntersectInfo<vec3>(false, State::notIntersect);
	}


	//IntersectInfo<vec3> Segment::segmentsCheck(const Segment& segment) const
	//{
	//	//this segment
	//	float v1 = 0;
	//	float v2 = length(get_v2() - get_v1()) * dir(get_v1(), get_v2());
	//	bool s1swap = v1 > v2;
	//	if (s1swap)
	//		swap(v1, v2);

	//	//other segment
	//	float v3 = length(segment.get_v1() - get_v1()) * dir(get_v1(), segment.get_v1());
	//	float v4 = length(segment.get_v2() - get_v1())  * dir(get_v1(), segment.get_v2());
	//	bool s2swap = v3 > v4;
	//	if (s2swap)
	//		swap(v3, v4);

	//	if (v2 < v3)//noIntersection
	//		return IntersectInfo<vec3>(false, State::notIntersect);
	//	else if (abs(v1 - v3) < GlobalEpsilon && abs(v2 - v4) < GlobalEpsilon)//identickee
	//		return IntersectInfo<vec3>(true, State::identical, { get_v1(), get_v2() });
	//	else if (abs(v3 - v2) < GlobalEpsilon) // v3 ~~ v2
	//		return IntersectInfo<vec3>(true, State::intersect, s1swap ? get_v1() : get_v2());
	//	else if (v3 > v1 && v3 < v2 && v2 < v4)//cross
	//		return IntersectInfo<vec3>(true, State::intersect, { s2swap ? segment.get_v2() : segment.get_v1(), s1swap ? get_v1() : get_v2() });
	//	else if (v3 < v2 && v2 > v4)//in
	//		return IntersectInfo<vec3>(true, State::intersect, { segment.get_v1(), segment.get_v2() });
	//	else
	//		return IntersectInfo<vec3>(false, State::notIntersect);
	//}

	IntersectInfo<vec3> Segment::intersects(const Ray& r) const
	{
		auto base = ray.intersects(r);
		if (!base.intersect)
			return base;
		else
		{
			if (base.state == State::identical)
			{
				base.intersectObj.push_back(get_v1());
				base.intersectObj.push_back(get_v2());
				return base;
			}
			else
				return intersects(base.intersectObj[0]);
		}
	}

	IntersectInfo<vec3> Segment::intersects(const vec3& p) const
	{
		auto base = ray.intersects(p);
		if (!base.intersect)
			return base;
		else
		{
			vec3 x = p - ray.get_v1();
			float dotp = dot(x, ray.get_direction());
			float baseDot = dot(ray.get_direction(), ray.get_direction());
			if (dotp < 0 || dotp > baseDot)
				return IntersectInfo<vec3>(false, State::notIntersect);
			else
				return base;
		}
	}

	const Ray& Segment::get_Ray() const
	{
		return ray;
	}


}