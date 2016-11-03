#pragma once
#include "Interaction/ConvexObj.hpp"

namespace CollisionDetection3D
{
	

	class Body : public ConvexObj
	{
	public:

		Body(ApplicationCore& app, vector<vec3> vertices, vector<unsigned int> indices, vec3 position, float scale = 1.0f, Pixel color = Pixel(0, 255, 0), vec3 velocity = vec3(0),size_t id = 0) :
			ConvexObj( app,  vertices,  indices,  position,  scale,  color), velocity(velocity), id(id) {};

		vec3 velocity;
		size_t id;

		bool remove = false;
		bool move = false;
		bool touched = false;	

	

		vector< OcLeaf< shared_ptr<Body>, vector<shared_ptr<Body>> > *> nodes;

	};

}