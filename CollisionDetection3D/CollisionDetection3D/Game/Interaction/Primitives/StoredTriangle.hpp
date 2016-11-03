#pragma once
#include "Triangle.hpp"
#include <array>

namespace CollisionDetection3D
{
	using namespace std;
	using namespace glm;

	struct StoredTriangle : public Triangle
	{
		StoredTriangle(const vec3& v1, const vec3& v2, const vec3& v3) : StoredTriangle(v1, v2, v3, mat4(1)){}

		StoredTriangle(const vec3& v1, const vec3& v2, const vec3& v3, const mat4& transform) : Triangle(data[0], data[1], data[2], transform)
		{
			data[0] = vec3(transform * vec4(v1, 1));
			data[1] = vec3(transform * vec4(v2, 1));
			data[2] = vec3(transform * vec4(v3, 1));
		}

		virtual vec3 getTv1() const override
		{
			return data[0];
		}

		virtual vec3 getTv2() const override
		{
			return data[1];
		}

		virtual vec3 getTv3() const override
		{
			return data[2];
		}

	private:
		array<vec3, 3> data;
	};


}

