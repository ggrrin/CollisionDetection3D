#include "HeightMap.hpp"

namespace CollisionDetection3D
{
	HeightMap::HeightMap(ApplicationCore* app, const Bitmap tex, const Bitmap& heightMap, float maxHeight, float textureSampling, float xElementSize, float yElementSize) :
		IDrawableComponent(app),
		width(heightMap.get_width()),
		height(heightMap.get_height()),
		xElementSize(xElementSize),
		zElementSize(yElementSize),
		textureSampling(textureSampling),
		vertexBuffer(width*height),
		texCoorBuffer(width*height),
		indexBuffer(2 * 3 * (width - 1)*(height - 1)),
		normalBuffer(width*height,vec3(0, 1, 0)),//Init of bounding normals	
		texture(tex),
		finalHeight(maxHeight)
	{
		
		auto vbIt = vertexBuffer.begin();
		auto texIt = texCoorBuffer.begin();
		auto indexIt = indexBuffer.begin();

		for (size_t y = 0; y < height; y++)
		for (size_t x = 0; x < width; x++)
		{
			auto pixel = heightMap.pixelAt(x, y);
			auto curRelativeHeight = ((float)pixel.R + (float)pixel.G + (float)pixel.B) / 255.0f / 3.0f;

			*(vbIt++) = vec3(x*zElementSize, curRelativeHeight * maxHeight, y*zElementSize);
			*(texIt++) = textureSampling * vec2(x*zElementSize, y*zElementSize);

			if (y < height - 1 && x < width - 1)
				addTriangles(x, y, indexIt);
		}

		auto normalIt = normalBuffer.begin();
		for (size_t y = 0; y < height; y++)
		for (size_t x = 0; x < width; x++)
		{
			if (y > 0 && x > 0 && y < height - 1 && x < width - 1)
				addNormal(x, y, normalIt);
			else
				++normalIt;
		}
	}

	void HeightMap::addNormal(int x, int y, vector<vec3>::iterator& it){
		//a b .
		//d e f
		//. h i

		//VBindices
		size_t e = y*width + x;//current x,y position
		size_t f = e + 1;
		size_t d = e - 1;

		size_t b = e - width;
		size_t a = b - 1;

		size_t h = e + width;
		size_t i = h + 1;

		OrientatedPlane planes[] = {
			OrientatedPlane(vertexBuffer[e], vertexBuffer[a], vertexBuffer[d]),
			OrientatedPlane(vertexBuffer[e], vertexBuffer[b], vertexBuffer[a]),
			OrientatedPlane(vertexBuffer[e], vertexBuffer[f], vertexBuffer[b]),
			OrientatedPlane(vertexBuffer[e], vertexBuffer[d], vertexBuffer[h]),
			OrientatedPlane(vertexBuffer[e], vertexBuffer[h], vertexBuffer[i]),
			OrientatedPlane(vertexBuffer[e], vertexBuffer[i], vertexBuffer[f])
		};

		vec3 normal = vec3(0, 0, 0);
		for (auto i : planes)
			normal += normalize(i.get_Normal());
		normal /= 6;

		*(it++) = normalize(normal);
	}

	void HeightMap::addTriangles(int x, int y, vector<unsigned int>::iterator& it){
		//a-b
		//c-d
		int a = y * width + x;
		int b = y * width + (x + 1);
		int c = (y + 1) * width + x;
		int d = (y + 1) * width + (x + 1);

		//t1
		*(it++) = a;
		*(it++) = c;
		*(it++) = d;

		//t2
		*(it++) = a;
		*(it++) = d;
		*(it++) = b;
	}

	void HeightMap:: onInitialize()
	{
		modelMatrix = mat4(1);
		
	}

	void HeightMap::update(){
		//getMvpMatrix() = getProjectionMatrix() * getViewMatrix() * modelMatrix;
	}

	const vector<unsigned int>& HeightMap::getIndexBuffer() const
	{
		return indexBuffer;
	}

	const vector<vec3>& HeightMap::getVertexBuffer() const
	{
		return vertexBuffer;
	}

	const vector<vec3>& HeightMap::getNormalBuffer() const
	{
		return normalBuffer;
	}

	const vector<vec2>& HeightMap::getTexCoorBuffer() const
	{
		return texCoorBuffer;
	}

	const mat4& HeightMap::getmodelMatrix()  const
	{
		return modelMatrix;
	}

	const Bitmap& HeightMap::getTexture() const
	{
		return texture;
	}

	HeightMap::~HeightMap()
	{
	}
}
