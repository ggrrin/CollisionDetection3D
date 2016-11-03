#pragma once
#include <glm/gtx/transform.hpp>
#include "Primitives/Primitives.hpp"
#include "Bounding/Bounding.hpp"
#include "CornerTable.hpp"
#include <fstream>
#include <memory>
#include <queue>
#include "Bounding/TriangleOcTree.hpp"
#include "../Event.hpp"

namespace CollisionDetection3D
{
	using namespace std;
	using namespace glm;
	class CompositeObj;

	class ConvexObj :  public CornerTable
	{
	public:
		ConvexObj(ApplicationCore& app, vector<vec3> vertices, vector<unsigned int> indices, Bitmap texture = Bitmap(1, 1, Pixel(0, 255, 0)), mat4 transform = mat4(1.0f));

		ConvexObj(ApplicationCore& app, vector<vec3> vertices, vector<unsigned int> indices, vec3 position, float scale = 1.0f, Pixel color = Pixel(0, 255, 0));

		virtual ~ConvexObj();

		virtual const BoundingSphere& getBSphere() const;

		virtual const BoundingBox& getBBox() const;


		virtual const TriangleOcTree& getOcTree() const;

		bool Intersects(const BoundingBox& bb) const
		{
			return bb.intersects(bb);
		}

		
		IntersectInfo<vec3> intersectsDummy(const ConvexObj& c) const;
		
		IntersectInfo<vec3>intersectsTree(const ConvexObj& c) const;


		IntersectInfo<vec3> intersectsPoints(const ConvexObj& c) const;

		IntersectInfo<vec3> intersectsPP(const ConvexObj& c) const;

		virtual IntersectInfo<vec3> intersects(const ConvexObj& c) const;

		virtual IntersectInfo<vec3> intersects(const vec3& v) const;

		void translate(const vec3& translationVector);

		

		virtual void initialize() override;

		void initializeBounds();

		static bool loadModel(string path, vector<vec3>& vertices, vector<unsigned int>& indices);

		virtual const vector<vec2>& getTexCoorBuffer()  const override;
		virtual const Bitmap& getTexture()  const override;

		virtual const mat4& getmodelMatrix()   const override;

	protected:
		BoundingSphere bs;
		BoundingBox bb;
		TriangleOcTree ot;
		vector<vec2> texBuffer;
		Bitmap texture;
		mat4 modelMatrix;

	
		static void transformBEtoLE4B(char* data, int length);
	
	};
}

