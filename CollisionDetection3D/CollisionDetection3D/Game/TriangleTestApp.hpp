#pragma once
#include "../ApplicationBase/ApplicationCore.hpp"
#include "AppComponents/FreeLookCamera.hpp"
#include "AppComponents/HeightMap.hpp"
#include "Interaction/ConvexObj.hpp"
#include "AppComponents/InfoDrawer.hpp"
#include "AppComponents/SimpleDrawer.hpp"

namespace CollisionDetection3D
{
	class Application :
		public ApplicationCore
	{
	public:
		Application(){};
		virtual ~Application(){};
	private:
		shared_ptr<HeightMap> map;
		shared_ptr<FreeLookCamera> camera;
		shared_ptr<ConvexObj> item1;
		shared_ptr<ConvexObj> item2;
		shared_ptr<ConvexInfo> cvd2;
		shared_ptr<SimpleDrawer> sd;
		shared_ptr<DrawableInfo> mapD;
		shared_ptr<ConvexInfo> cvd1;

		virtual void onInitialize(const vector<string>& args) override{
			timer.setDrawRate(60);

			Bitmap x(1, 1, Pixel(255, 0, 0, 255));
			Bitmap y(args[2]);

			lightsPositions.push_back(vec3(128, 255, 128));
			//camera = make_shared<FreeLookCamera>(this, vec3(200, 100, 160), vec3(190, 100, 161), 25.0f);//vec3(140, 24, 160)

			camera = make_shared<FreeLookCamera>(this, vec3(0, 1, 1), vec3(0), 5.0f);//vec3(140, 24, 160)

			map = make_shared<HeightMap>(this, y, Bitmap(args[1]), 50.0f);
			glClearColor(0.33f, 0.9f, 1.0f, 0.0f);

			///////////////////////////////////////////////////
			vector<vec3> vertices;
			vector<unsigned int> indices;

			//bool res = ConvexObj::loadModel("Data/simple00.cob", vertices, indices);
			//item1 = make_shared<ConvexObj>(*this, vertices, indices, vec3(50, 100, 200), 1);
			//item2 = make_shared<ConvexObj>(*this, vertices, indices, vec3(50, 100, 100), 1, Pixel(0,0,255));

			indices.push_back(0);
			indices.push_back(1);
			indices.push_back(2);

			vertices.push_back(vec3(0, 0.1f, 0));
			vertices.push_back(vec3(0, 0, -1));
			vertices.push_back(vec3(-1, 0, 0));

			item1 = make_shared<ConvexObj>(*this, vertices, indices, vec3(0), 2, Pixel(255, 0, 0));

			vertices.clear();

			vertices.push_back(vec3(-0.25f, -0.25f, -0.25f));
			vertices.push_back(vec3(0, 1, -1));
			vertices.push_back(vec3(-1, 1, 0));
			vertices.push_back(vec3(-1, 1, -1));

			indices.push_back(0);
			indices.push_back(3);
			indices.push_back(1);

			indices.push_back(0);
			indices.push_back(2);
			indices.push_back(3);


			indices.push_back(1);
			indices.push_back(3);
			indices.push_back(2);

			item2 = make_shared<ConvexObj>(*this, vertices, indices, vec3(0), 3);

			cvd1 = make_shared<ConvexInfo>(*this, *item1);
			cvd2 = make_shared<ConvexInfo>(*this, *item2);

			sd = make_shared<SimpleDrawer>(*this, vec3(0, 0, 0), 5, true);

			//sd->addVector(vec3(100, 0, 0), vec3(-100, 0, 0));
			//sd->addVector(vec3(0,100, 0), vec3(0,-100, 0));
			//sd->addVector(vec3(0,0,100), vec3(0,0,-100));
			sd->addLoop({ vec3(100, 0, 0), vec3(0, 100, 0), vec3(0, 0, 100) });
		};

		bool intersect = false;

		virtual void onUpdate() override
		{
			sd->clear();

			if (isKeyPressed(GLFW_KEY_SPACE))
			{
				item1.swap(item2);
			}

			if (item2->getBBox().intersects(item1->getBBox()))
			{
				if (!intersect)
				{
					cvd1->reInit(ConvexInfo::defaultNormalColor, ConvexInfo::defaultWireColor, vec3(1, 0, 0), ConvexInfo::defaultBspherelColor);
					cvd2->reInit(ConvexInfo::defaultNormalColor, ConvexInfo::defaultWireColor, vec3(1, 0, 0), ConvexInfo::defaultBspherelColor);
				}
				intersect = true;
			}
			else
			{
				if (intersect)
				{
					cvd1->reInit(ConvexInfo::defaultNormalColor, ConvexInfo::defaultWireColor, ConvexInfo::defaultBboxColor, ConvexInfo::defaultBspherelColor);
					cvd2->reInit(ConvexInfo::defaultNormalColor, ConvexInfo::defaultWireColor, ConvexInfo::defaultBboxColor, ConvexInfo::defaultBspherelColor);
				}
				intersect = false;
			}

			auto res = item1->intersects(*item2);

			const float k = 0.001f;

			if (isKeyPressed(GLFW_KEY_DOWN))
				item2->translate(vec3(0, -k, 0));
			else if (isKeyPressed(GLFW_KEY_UP))
				item2->translate(vec3(0, k, 0));


			if (isKeyPressed(GLFW_KEY_LEFT))
				item2->translate(vec3(0, 0, k));
			else if (isKeyPressed(GLFW_KEY_RIGHT))
				item2->translate(vec3(0, 0, -k));

			if (isKeyPressed(GLFW_KEY_3))
				item2->translate(vec3(-k, 0, 0));
			else if (isKeyPressed(GLFW_KEY_2))
				item2->translate(vec3(k, 0, 0));

			if (res.intersect)
			{
				sd->addPoints(res.intersectObj);
				sd->addLoop(res.intersectObj);
			}
			Debugln(to_string(res.intersectObj.size()));

		}

		virtual void onDraw() override{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			item1->draw();
			item2->draw();
		};

		virtual void onExit() override{};
	};
}

