#pragma once
#include "SimpleDrawer.hpp"
#include "glm/glm.hpp"

namespace CollisionDetection3D
{
	using namespace glm;
	class ApplicationCore;

	template<class Dummy>
	class DefInit { public: static vec3 defaultNormalColor; static vec3 defaultWireColor; static vec3 defaultBboxColor; static vec3 defaultBspherelColor; static vec3 defaultOcTColor; };
	template<class Dummy> vec3 DefInit<Dummy>::defaultNormalColor = vec3(1, 1, 0);
	template<class Dummy> vec3 DefInit<Dummy>::defaultWireColor = vec3(0,0,1);
	template<class Dummy> vec3 DefInit<Dummy>::defaultBboxColor = vec3(0, 0, 0);
	template<class Dummy> vec3 DefInit<Dummy>::defaultBspherelColor = vec3(0.75f, 0, 1);
	template<class Dummy> vec3 DefInit<Dummy>::defaultOcTColor = vec3(1, 0.8f, 0);

	

	template<typename Item>	
	class InfoDrawer : private SimpleDrawer, public DefInit<void>
	{
	public:
		InfoDrawer(ApplicationCore& app, Item& subject) : dable(subject), SimpleDrawer(app)
		{
			init(subject);
		}
				
		void reInit(const vec3& normalColor, const vec3& wireColor)
		{
			clear();
			reInit(dable, normalColor, wireColor);
		}
		void reInit(const vec3& normalColor, const vec3& wireColor, const vec3& bboxColor, const vec3& sphereColor)
		{
			clear();
			init(dable, normalColor, wireColor, bboxColor, sphereColor);
		}

		void stopDraw()
		{
			drawing = false;
		}

		void startDraw()
		{
			drawing = true;
		}

	protected:
		Item& dable;

		void init(IDrawable& dable, const vec3& normalColor = defaultNormalColor, const vec3& wireColor = defaultWireColor)
		{
			initNormals(normalColor);
			initWireFrame(wireColor);
		}

		void init(ConvexObj& q, const vec3& normalColor = defaultNormalColor, const vec3& wireColor = defaultWireColor,
			const vec3& bboxColor = defaultBboxColor, const vec3& sphereColor = defaultBspherelColor, const vec3& ocTreeColor = defaultOcTColor)
		{
			auto mod = inverse(mat4(q.getmodelMatrix()));
			init(static_cast<IDrawable&>(q), normalColor, wireColor);
			initBbox(q.getBBox(), bboxColor, mod);
			iniBsphere(q.getBSphere(), sphereColor, mod);
			initOcTree(q.getOcTree(), ocTreeColor, mod * translate(q.getOcTree().getPosition()));
		}

		void initOcTree(const TriangleOcTree& bb, const vec3& color, const mat4& mod)
		{
			tree(bb.getRoot(), color, mod);
		}

		void tree(const OcElement* node,  const vec3& color, const mat4& mod)
		{
			if (!node->isLeave())
				for (const auto& s : node->sons)
					tree(s.get(), color, mod);
			else
				initBbox(node->box, color, mod);
		}

		virtual const mat4& getModelMatrix() const override
		{
			return dable.getmodelMatrix();
		}

		void initNormals(const vec3& color)
		{
			auto vIt = dable.getVertexBuffer().begin();
			auto nIt = dable.getNormalBuffer().begin();
			int i = 0;
			while (vIt != dable.getVertexBuffer().end() || nIt != dable.getNormalBuffer().end())
				addVector(*(vIt++), *(nIt++), color);
		}

		void initWireFrame(const vec3& color)
		{
			auto ib = dable.getIndexBuffer();
			auto vb = dable.getVertexBuffer();

			for (size_t j = 0; j < ib.size() - 3; j++)
			{
				auto p = vb[ib[j]];
				auto q = j % 3 == 2 ? vb[ib[j - 2]] : vb[ib[j + 1]];
				addVector(p, q - p, color);
			}
		}

		void initBbox(const BoundingBox& bb, const vec3& BBoxColor, const mat4& mod)
		{
			auto m = bb.getMin();
			auto n = bb.getMax();

			auto a = vec3(n.x - m.x, 0, 0);
			auto b = vec3(0, n.y - m.y, 0);
			auto c = vec3(0, 0, n.z - m.z);

			addSegment(trans(mod, m), trans(mod, m + a), BBoxColor);
			addSegment(trans(mod, m), trans(mod, m + a), BBoxColor);
			addSegment(trans(mod, m), trans(mod, m + b), BBoxColor);
			addSegment(trans(mod, m), trans(mod, m + c), BBoxColor);
			addSegment(trans(mod, n), trans(mod, n - a), BBoxColor);
			addSegment(trans(mod, n), trans(mod, n - b), BBoxColor);
			addSegment(trans(mod, n), trans(mod, n - c), BBoxColor);
			addSegment(trans(mod, m + b), trans(mod, m + b + a), BBoxColor);
			addSegment(trans(mod, m + b), trans(mod, m + b + c), BBoxColor);
			addSegment(trans(mod, n - b), trans(mod, n - b - a), BBoxColor);
			addSegment(trans(mod, n - b), trans(mod, n - b - c), BBoxColor);
			addSegment(trans(mod, m + a), trans(mod, m + a + b), BBoxColor);
			addSegment(trans(mod, m + c), trans(mod, m + c + b), BBoxColor);
		}

		void iniBsphere(const BoundingSphere& bs, const vec3& sphereColor, const mat4& mod)
		{
			vec3 c = bs.getCenter();
			auto r = bs.getRadius();

			addSegment(trans(mod, c - r * vec3(1, 0, 0)), trans(mod, c + r * vec3(1, 0, 0)), sphereColor);
			addSegment(trans(mod, c - r * vec3(0, 1, 0)), trans(mod, c + r * vec3(0, 1, 0)), sphereColor);
			addSegment(trans(mod, c - r * vec3(0, 0, 1)), trans(mod, c + r * vec3(0, 0, 1)), sphereColor);
		}
	};


	using ConvexInfo = InfoDrawer<ConvexObj>;
	using DrawableInfo = InfoDrawer<IDrawable>;

}