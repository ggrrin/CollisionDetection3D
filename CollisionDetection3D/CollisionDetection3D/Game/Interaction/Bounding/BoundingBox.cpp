#include "BoundingBox.hpp"


namespace CollisionDetection3D
{
	vec3 BoundingBox::getCorner(size_t i) const
	{
		auto m = min;
		auto n = max;

		auto a = vec3(n.x - m.x, 0, 0);
		auto b = vec3(0, n.y - m.y, 0);
		auto c = vec3(0, 0, n.z - m.z);

		switch (i)
		{
		case 0:
			return m;
		case 1:
			return m + a;
		case 2:
			return m + b;
		case 3:
			return m + c;
		case 4:
			return n;
		case 5:
			return n - a;
		case 6:
			return n - b;
		case 7:
			return n - c;
		default:
			throw logic_error("unexpect state;");
		}
	}

	StoredTriangle BoundingBox::getTriangle(size_t i) const
	{
		switch (i)
		{
		case 0:
			return StoredTriangle(getCorner(0), getCorner(1), getCorner(2));
		case 1:
			return StoredTriangle(getCorner(1), getCorner(7), getCorner(2));
		case 2:
			return StoredTriangle(getCorner(0), getCorner(2), getCorner(3));
		case 3:
			return StoredTriangle(getCorner(2), getCorner(5), getCorner(3));
		case 4:
			return StoredTriangle(getCorner(0), getCorner(3), getCorner(1));
		case 5:
			return StoredTriangle(getCorner(3), getCorner(6), getCorner(1));
		case 6:
			return StoredTriangle(getCorner(1), getCorner(6), getCorner(7));
		case 7:
			return StoredTriangle(getCorner(6), getCorner(4), getCorner(7));
		case 8:
			return StoredTriangle(getCorner(6), getCorner(5), getCorner(4));
		case 9:
			return StoredTriangle(getCorner(6), getCorner(3), getCorner(5));
		case 10:
			return StoredTriangle(getCorner(2), getCorner(7), getCorner(5));
		case 11:
			return StoredTriangle(getCorner(7), getCorner(4), getCorner(5));
		default:
			throw logic_error("unexpect state");

		}
		
	}

	CornerIterator::value_type CornerIterator::operator*(){
		return bb.getCorner(i);
	}

	CornerIterator::value_type CornerIterator::operator->(){
		return **this;
	}

	CornerIterator::self_type CornerIterator::operator++(){
		i++;
		return (*this);
	}

	CornerIterator::self_type CornerIterator::operator++(int){
		i++;
		return (*this);
	}

	bool CornerIterator::operator==(const self_type& it) const{
		if (&it.bb != &bb)
			throw std::logic_error("Iterators iterate over different data!");
		return it.i == i;
	}

	bool CornerIterator::operator !=(const self_type& it) const{
		return !(it == *this);
	}

	///////////////////////////////////////////////////

	CornerIterator CornerContainer::begin() const
	{
		return CornerIterator(bb, 0);
	}

	CornerIterator CornerContainer::end() const
	{
		return CornerIterator(bb, 8);
	}


	///////////////////////////////////////////////


	BoundingBox::BoundingBox(){}

	BoundingBox::BoundingBox(vec3 min, vec3 max) :min(min), max(max)
	{
		if (min.x >= max.x && min.y >= max.y && min.z >= max.z)
			throw logic_error("min has to be lower then max!");
	}

	BoundingBox::~BoundingBox()
	{}

	const vec3& BoundingBox::getMin() const
	{
		return min;
	}

	const vec3& BoundingBox::getMax() const
	{
		return max;
	}

	bool BoundingBox::intersects(const BoundingBox& b) const
	{
		if (this->vertexCheck(b) || b.vertexCheck(*this))
			return true;
		else if (this->subBoxCheck(b) || b.subBoxCheck(*this))
			return true;
		else
			return false;
	}

	bool BoundingBox::subBoxCheck(const BoundingBox& b) const
	{
		auto bmin = b.getMin();
		auto bmax = b.getMax();
		for (size_t i = 0; i < 3; i++)
		{
			if (bmin[i] <= min[i] && bmax[i] >= max[i])
			{
				bmin[i] = min[i];
				bmax[i] = max[i];
			}
		}
		BoundingBox subBox(bmin, bmax);
		return this->vertexCheck(subBox);
	}

	//slow!! suitable only for initialization
	bool BoundingBox::intersects(const Triangle& t) const
	{
		if (intersects(t.getTv1()) || intersects(t.getTv2()) || intersects(t.getTv3()))
			return true;
		else
		{
			for (size_t i = 0; i < 12; i++)
			{
				auto r = t.intersects(getTriangle(i));
				if (r.intersect)
					return true;
			}
			return false;
		}
	}

	float BoundingBox::getXSize() const
	{
		return  max.x - min.x;
	}

	float BoundingBox::getYSize() const
	{
		return max.y - min.y;
	}

	float BoundingBox::getZSize() const
	{
		return max.z - min.z;
	}

	void BoundingBox::translate(const vec3& translation)
	{
		min += translation;
		max += translation;
	}

	bool BoundingBox::intersects(const vec3& v)const
	{
		if (min.x <= v.x && v.x <= max.x)//a b a			
			if (min.y <= v.y && v.y <= max.y)//a b a				
				if (min.z <= v.z && v.z <= max.z)//a b a					
					return true;
		return false;
	}

	CornerContainer BoundingBox::getCorners() const
	{
		return CornerContainer(*this);
	}


	
	


	bool BoundingBox::betweenCheck(const BoundingBox& b) const
		{
			bool between = false;
			for (size_t i = 0; i < 3; i++)
			{
				if (min[i] <= b.min[i] && b.max[i] <= max[i])
				{
					between = true;
					break;
				}
			}

			if (between)
			{
				for (size_t i = 0; i < 3; i++)
				{
					if (!((min[i] <= b.min[i] || b.max[i] <= max[i]) ||
						(b.min[i] <= min[i] || max[i] <= b.max[i])))
						return false;
				}

				return true;
			}
			else
			{
				return false;
			}
		}

	bool BoundingBox::vertexCheck(const BoundingBox& b) const
		{
			for (const auto& v : b.getCorners())
				if (intersects(v))
					return true;
			return false;
		}


}