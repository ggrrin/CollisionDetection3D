#include "ConvexObj.hpp"

namespace CollisionDetection3D
{
	ConvexObj::ConvexObj(ApplicationCore& app, vector<vec3> vertices, vector<unsigned int> indices, Bitmap texture, mat4 transform ) :
		texture(texture), modelMatrix(transform), CornerTable(app, vertices, indices)
	{
		initialize();
	}

	ConvexObj::ConvexObj(ApplicationCore& app, vector<vec3> vertices, vector<unsigned int> indices, vec3 position, float scale , Pixel color )
		:ConvexObj(app, vertices, indices, Bitmap(1, 1, color), glm::translate(position)* glm::scale(scale* vec3(1))){}

	 ConvexObj::~ConvexObj(){	}

	 const BoundingSphere& ConvexObj::getBSphere() const
	{
		return bs;
	}

	 const BoundingBox& ConvexObj::getBBox() const
	{
		return bb;
	}
	

	 const TriangleOcTree& ConvexObj::getOcTree() const
	{
		return ot;
	}


	IntersectInfo<vec3> ConvexObj::intersectsDummy(const ConvexObj& c) const
	{	
		IntersectInfo<vec3> res(false, State::intersect);
		for (auto t1 : getTriangles())
		{
			for (auto t2 : c.getTriangles())
			{
				auto status = t1.intersects(t2);
				if (status.intersect)
					res *= status;
			}
		}
		return res;
	}

	IntersectInfo<vec3> ConvexObj::intersectsTree(const ConvexObj& c) const
	{
		auto res = ot.intersects(c.ot);
		if (res.intersect)
		{
			TriangleIterator initT1 = res.intersectObj[0];
			TriangleIterator initT2 = res.intersectObj[1];

			auto finalResult = IntersectInfo<vec3>(true, State::intersect, { (*initT1).getTv1(), (*initT1).getTv2(), (*initT1).getTv3() });
			//pridani bodu s interakce prvnich trojuhelniku
			//auto qerqerqerqer = (*initT1).intersects(*initT2).intersectObj;
			//finalResult.intersectObj.insert(finalResult.intersectObj.begin(), qerqerqerqer.begin(), qerqerqerqer.end());
			///////////////////////////////////////////
			//
			//queue<TriangleIterator> t1Neighbours;
			//queue<TriangleIterator> t2Neighbours;
			//
			//t1Neighbours.push(initT1);
			//t2Neighbours.push(initT2);
			//
			//vector<bool> thisTriangles(getTriangles().size(),false);
			//thisTriangles[initT1.getTriangleId()] = true;
			//
			//vector<bool> otherTriangles(c.getTriangles().size(), false);
			//otherTriangles[initT2.getTriangleId()] = true;
			//
			//while (!t1Neighbours.empty() && !t2Neighbours.empty())
			//{
			//	Triangle t1 = *t1Neighbours.front();
			//	Triangle t2 = *t2Neighbours.front();
			//
			//	for (const auto& x : t2Neighbours.front().getTriangleNeigbours())
			//	{
			//		if (!otherTriangles[x.getTriangleId()])//pokud jsem ho jeste neprosli
			//		{
			//			otherTriangles[x.getTriangleId()] = true;
			//
			//			auto res_ = t1.intersects(*x);
			//			if (res_.intersect)
			//			{
			//				finalResult.intersectObj.insert(finalResult.intersectObj.end(), res_.intersectObj.begin(), res_.intersectObj.end());
			//				t2Neighbours.push(x);
			//			}
			//		}
			//	}
			//
			//	for (const auto& y : t1Neighbours.front().getTriangleNeigbours())
			//	{
			//		if (!thisTriangles[y.getTriangleId()])
			//		{
			//			thisTriangles[y.getTriangleId()] = true;
			//
			//			auto res_ = t2.intersects(*y);
			//			if (res_.intersect)
			//			{
			//				finalResult.intersectObj.insert(finalResult.intersectObj.end(), res_.intersectObj.begin(), res_.intersectObj.end());
			//				t1Neighbours.push(y);
			//			}
			//		}
			//	}
			//
			//	t1Neighbours.pop();
			//	t2Neighbours.pop();
			//
			//}

			return finalResult;
		}
		else
			return IntersectInfo<vec3>(false, State::notIntersect);
	}


	IntersectInfo<vec3> ConvexObj::intersectsPoints(const ConvexObj& c) const
	{
		for (const auto& vertex : vertexBuffer)//TODO vertexIterator
		{
			auto res = c.intersects(trans(modelMatrix, vertex));
			if (res.intersect)
				return res;
		}

		return IntersectInfo<vec3>(false, State::none);
	}

	IntersectInfo<vec3> ConvexObj::intersectsPP(const ConvexObj& c) const
	{
		auto res = intersectsPoints(c);
		if (!res.intersect)
			res = c.intersectsPoints(*this);

		return res;
	}

	 IntersectInfo<vec3> ConvexObj::intersects(const ConvexObj& c) const
	{
		if (bs.intersects(c.bs) && bb.intersects(c.bb))
		{
			return intersectsDummy(c);
		}
		else
			return IntersectInfo<vec3>(false, State::notIntersect);
	}

	 IntersectInfo<vec3> ConvexObj::intersects(const vec3& v) const
	{
		if (bs.intersects(v) && bb.intersects(v))
		{
			for (const auto a : getPlanesModelTransformed())
				if (a.halfIntersect(v))
					return IntersectInfo<vec3>(false, State::notIntersect);
			return IntersectInfo<vec3>(true, State::liesIn, v);
		}
		else
			return IntersectInfo<vec3>(false, State::notIntersect);
	}

	void ConvexObj::translate(const vec3& translationVector)
	{
		modelMatrix = glm::translate(translationVector) * modelMatrix;
		bs.translate(translationVector);
		bb.translate(translationVector);
		ot.translate(translationVector);

	
	}

	 void ConvexObj::initialize() 
	{
		//texture = Bitmap(1, 1, Pixel(0, 255, 0, 255));//"Data/texture00.png");//
		texBuffer = vector<vec2>(vertexBuffer.size(), vec2(0));

		int k = 0;
		for (auto& i : texBuffer)
			i = vec2(k, k++);

		initializeBounds();

		ot = TriangleOcTree(bb, getTriangles(), 1);
		IDrawable::initialize();
	}

	void ConvexObj::initializeBounds()
	{
		vec3 center;
		float minX, minY, minZ, maxX, maxY, maxZ;

		vec3 init = vec3(modelMatrix * vec4(vertexBuffer[0], 1));
		maxX = minX = init.x;
		maxY = minY = init.y;
		maxZ = minZ = init.z;


		for (const auto& q : vertexBuffer)
		{
			vec3 v = vec3(modelMatrix * vec4(q, 1));

			center += v;
			//min
			if (v.x < minX)
				minX = v.x;

			if (v.y < minY)
				minY = v.y;

			if (v.z < minZ)
				minZ = v.z;

			//max
			if (v.x > maxX)
				maxX = v.x;

			if (v.y > maxY)
				maxY = v.y;

			if (v.z > maxZ)
				maxZ = v.z;
		}
		center /= vertexBuffer.size();
		bb = BoundingBox(vec3(minX, minY, minZ), vec3(maxX, maxY, maxZ));

		float maxRadius = 0;
		for (const auto& q : vertexBuffer)
		{
			vec3 v = vec3(modelMatrix * vec4(q, 1));
			float d = length(center - v);
			if (d > maxRadius)
				maxRadius = d;
		}

		bs = BoundingSphere(center, maxRadius);
	}

	bool ConvexObj::loadModel(string path, vector<vec3>& vertices, vector<unsigned int>& indices)
	{
		vertices.clear();
		indices.clear();

		const int headerLength = 3 * sizeof(int);
		ifstream reader;
		reader.open(path, ios_base::binary | ios_base::in);
		if (reader.fail())
			return false;

		unsigned formatLE = 0xAFCAD9A8;
		unsigned formatBE = 0xA8D9CAAF;

		char headerBuffer[headerLength];
		//read header
		reader.read(headerBuffer, headerLength);

		if (reader.fail())
			return false;

		unsigned header = *reinterpret_cast<unsigned*>(headerBuffer);
		bool littleEndian = header == formatLE;

		if (!littleEndian)
			transformBEtoLE4B(headerBuffer, sizeof(headerBuffer));

		int* ptr = reinterpret_cast<int*>(headerBuffer)+1;
		int verticeCount = *ptr;
		int indiceCount = *(ptr + 1);

		if ((!littleEndian && formatBE != header) || verticeCount < 0 || indiceCount < 0)//invalid file format			
			return false;

		int dataBufferLength = sizeof(vec3)*verticeCount + sizeof(int)*indiceCount;

		unique_ptr<char[]> dataBuffer(new char[dataBufferLength]);

		reader.read(dataBuffer.get(), dataBufferLength);

		if (reader.fail())
			return false;

		if (!littleEndian)
			transformBEtoLE4B(dataBuffer.get(), dataBufferLength);

		vec3* vertBeg = reinterpret_cast<vec3*>(dataBuffer.get());
		vec3* vertEnd = vertBeg + verticeCount;
		unsigned int* indicesBeg = reinterpret_cast<unsigned int*>(vertEnd);
		unsigned int* indicesEnd = indicesBeg + indiceCount;

		vertices.insert(vertices.end(), vertBeg, vertEnd);
		indices.insert(indices.end(), indicesBeg, indicesEnd);

		return true;
	}

	 const vector<vec2>& ConvexObj::getTexCoorBuffer()  const 
	{
		return texBuffer;
	}
	 const Bitmap& ConvexObj::getTexture()  const 
	{
		return texture;
	}

	 const mat4& ConvexObj::getmodelMatrix()   const 
	{
		return modelMatrix;
	}

	 void ConvexObj::transformBEtoLE4B(char* data, int length){
		 for (size_t i = 0; i < length; i += 4)
		 {
			 swap(data[i + 0], data[i + 3]);
			 swap(data[i + 1], data[i + 2]);
		 }
	 }

}