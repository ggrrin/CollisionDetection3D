#pragma once
#include <glm/glm.hpp>

namespace CollisionDetection3D
{
	using namespace glm;

	class BoundingSphere final
	{
	public:

		BoundingSphere(){}

		BoundingSphere(vec3 position, float radius) :position(position), r(radius)
		{}

		~BoundingSphere()
		{}

		float getRadius() const
		{
			return r;
		}

		const vec3& getCenter() const
		{
			return position;
		}

		bool intersects(const BoundingSphere& s) const 
		{
			return length(s.position - position) <= r + s.r;
		}

		bool intersects(const vec3& s) const
		{
			return length(s - position) <= r;
		}

		void translate(const vec3& translation)
		{
			position += translation;
		}

	private:
		float r;
		vec3 position;
	};
}

