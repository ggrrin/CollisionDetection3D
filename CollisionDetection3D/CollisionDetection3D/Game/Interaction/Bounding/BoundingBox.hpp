#pragma once
#include <iterator>
#include <glm/glm.hpp>
#include "../Primitives/Primitives.hpp"


namespace CollisionDetection3D
{
	using namespace glm;
	using namespace std;

	class BoundingBox;

	class CornerIterator
	{
	public:
		CornerIterator(const BoundingBox& b, int index) : bb(b), i(index){};
		~CornerIterator(){};
		typedef CornerIterator self_type;
		typedef vec3 value_type;
		typedef vec3& reference;
		typedef vec3* pointer;
		typedef forward_iterator_tag iterator_category;

		value_type operator*();
		value_type operator->();
		self_type operator++();
		self_type operator++(int);
		bool operator==(const self_type& it) const;
		bool operator !=(const self_type& it) const;
	private:
		const BoundingBox& bb;
		int i;
	};

	class CornerContainer
	{
	public:
		CornerContainer(const BoundingBox& b) : bb(b){}
		~CornerContainer(){}

		CornerIterator begin() const;
		CornerIterator end() const;
	private:
		const BoundingBox& bb;
	};

	class BoundingBox final
	{
	public:
		BoundingBox();

		BoundingBox(vec3 min, vec3 max);

		~BoundingBox();

		const vec3& getMin() const;

		const vec3& getMax() const;

		bool intersects(const BoundingBox& b) const;

		bool subBoxCheck(const BoundingBox& b) const;


	

		//slow!! suitable only for initialization
		bool intersects(const Triangle& t) const;

		float getXSize() const;

		float getYSize() const;

		float getZSize() const;

		void translate(const vec3& translation);

		bool intersects(const vec3& v)const;

		CornerContainer getCorners() const;

		vec3 getCorner(size_t i) const;

		StoredTriangle getTriangle(size_t i) const;
	private:
		vec3 min;
		vec3 max;


		bool betweenCheck(const BoundingBox& b) const;

		bool vertexCheck(const BoundingBox& b) const;

	};

	
}