#pragma once
#include "../../ApplicationBase/ApplicationCore.hpp"
#include "../Interaction/Primitives/Primitives.hpp"
#include "../../ApplicationBase/IDrawableComponent.hpp"

#include "../Interaction/ConvexObj.hpp"
#include "SimpleDrawer.hpp"


using namespace std;
using namespace glm;

namespace CollisionDetection3D
{

	class HeightMap : public IDrawableComponent
	{
	public:	

		HeightMap(ApplicationCore* app, const Bitmap tex, const Bitmap& heightMap, float maxHeight = 255, float textureSampling = 1, float xElementSize = 1, float yElementSize = 1);
		virtual ~HeightMap();

		IntersectInfo<vec3> intersects(const BoundingBox& bb) const
		{			
			int xMinIndex, zMinIndex;
			xMinIndex = (int)(bb.getMin().x / xElementSize) - 1;
			zMinIndex = (int)(bb.getMin().z / zElementSize) - 1;


			int xSize = (int)ceilf(bb.getXSize() / xElementSize) + 2;
			int zSize = (int)ceilf(bb.getZSize() / zElementSize) + 2;


			auto result = IntersectInfo<vec3>(false, State::intersect,
			{				
				getAt(xMinIndex, zMinIndex),
				getAt(xSize + xMinIndex, zSize + zMinIndex),
				getAt(xMinIndex + xSize, zMinIndex),
				getAt(xMinIndex, (zSize + zMinIndex)),
				//!!!!!!!!!!!!!!!!!!indices !!!!!!
				vec3(xMinIndex, 0, zMinIndex),
				vec3(xSize, 0, zSize)
			});
			
			for (size_t x = 0; x < xSize; x++)
			{
				for (size_t z = 0; z < zSize; z++)
				{
					size_t curZ = zMinIndex + z;				


					size_t curX = xMinIndex + x;
					if (getAt(curX, curZ ).y > bb.getMin().y)
					{
						result.intersect = true;
						break;
					}
				}
				if (result.intersect)
					break;
			}

			if (result.intersect)
				result.state = State::intersect;
	
			return result;		
		}
		
		

		IntersectInfo<vec3> intersects(const ConvexObj& c) const
		{
		

			auto bbRes = intersects(c.getBBox());
			if (bbRes.intersect)
			{
				//GET indices
				int xMinIndex = bbRes.intersectObj[4].x;
				int zMinIndex = bbRes.intersectObj[4].z;
				int xSize = bbRes.intersectObj[5].x;
				int zSize = bbRes.intersectObj[5].z;

				IntersectInfo<vec3> result(false);

				Triangle t1 = getTriangle1(xMinIndex, zMinIndex, xSize, zSize);
				Triangle t2 = getTriangle2(xMinIndex, zMinIndex, xSize, zSize);

				for (auto t : c.getTriangles())
				{
					auto t1res = t.intersects(t1);
					auto t2res = t.intersects(t2);
					result *= t1res *= t2res;
				}
				return result;
			}
			else
				return IntersectInfo<vec3>(false, State::notIntersect);

		}

		Triangle getTriangle1(int x, int y, int xSize, int zSize)const{
			//a-b
			//c-d			
			int a = y * width + x;
			int b = y * width + (x + xSize);
			int c = (y + zSize) * width + x;
			int d = (y + zSize) * width + (x + xSize);

			if (a < 0 || a >= vertexBuffer.size() ||
				b < 0 || b >= vertexBuffer.size() ||
				c < 0 || c >= vertexBuffer.size() ||
				d < 0 || d >= vertexBuffer.size())
				return StoredTriangle(vec3(0), vec3(0), vec3(0));

			return Triangle(vertexBuffer[a], vertexBuffer[c], vertexBuffer[d]);
		}

		Triangle getTriangle2(int x, int y, int xSize, int zSize)const{
			//a-b
			//c-d
			int a = y * width + x;
			int b = y * width + (x + xSize);
			int c = (y + zSize) * width + x;
			int d = (y + zSize) * width + (x + xSize);
			
			if (a < 0 || a >= vertexBuffer.size() ||
				b < 0 || b >= vertexBuffer.size() ||
				c < 0 || c >= vertexBuffer.size() ||
				d < 0 || d >= vertexBuffer.size())
				return StoredTriangle(vec3(0), vec3(0), vec3(0));

			return Triangle(vertexBuffer[a], vertexBuffer[d], vertexBuffer[b]);

		}

		vec3 getAt(size_t x, size_t z) const
		{
			if (x >= 0 && x < width && z >= 0 && z < height)
				return vertexBuffer[z * width + x];
			else
				return vec3(x*xElementSize, -std::numeric_limits<float>::infinity(), z* zElementSize);
		}

		float getXWidth() const
		{
			return width* xElementSize;
		}

		float getZWidth() const
		{
			return height * zElementSize;
		}

		BoundingBox getBBox()
		{
			return BoundingBox(vec3(0), vec3(width*xElementSize, finalHeight, height*zElementSize));
		}

	protected:
		
		virtual void onInitialize() override;
		virtual void update() override;		
					

		virtual const vector<unsigned int>& getIndexBuffer() const override;
		virtual const vector<vec3>& getVertexBuffer() const override;
		virtual const vector<vec3>& getNormalBuffer() const override;
		virtual const vector<vec2>& getTexCoorBuffer() const override;
		virtual const mat4& getmodelMatrix() const override;
		virtual const Bitmap& getTexture() const override;

	private:
		size_t width, height;
		float xElementSize, zElementSize, textureSampling;
		float finalHeight;

		vector<vec3> vertexBuffer;
		vector<vec2> texCoorBuffer;
		vector<unsigned int> indexBuffer;
		vector<vec3> normalBuffer;
		mat4 modelMatrix;
		Bitmap texture;
		
		void addTriangles(int x, int y, vector<unsigned int>::iterator& it);
		void addNormal(int x, int y, vector<vec3>::iterator& it);
			
	};

}


