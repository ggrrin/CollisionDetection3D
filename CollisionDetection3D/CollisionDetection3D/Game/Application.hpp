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
			camera = make_shared<FreeLookCamera>(this, vec3(0,0,10), vec3(0), 25.0f);//vec3(140, 24, 160)		
			map = make_shared<HeightMap>(this, y, Bitmap(args[1]), 50.0f);
			glClearColor(0.33f, 0.9f, 1.0f, 0.0f);

			//mapD = make_shared<DrawableInfo>(*this, *map);
			///////////////////////////////////////////////////
			vector<vec3> vertices;
			vector<unsigned int> indices;

			bool res = ConvexObj::loadModel("Data/simple00.cob", vertices, indices);
			item1 = make_shared<ConvexObj>(*this, vertices, indices, vec3(105), 2);
			res = ConvexObj::loadModel("Data/simple01.cob", vertices, indices);
			item2 = make_shared<ConvexObj>(*this, vertices, indices, vec3(103,50,103), 1, Pixel(107,82,20));//Bitmap(1,1, Pixel(0, 0, 255)), translate(vec3(104,95,103.1)) * scale(vec3(0.3,8,1)) );


			cvd1 = make_shared<ConvexInfo>(*this, *item1);
			cvd2 = make_shared<ConvexInfo>(*this, *item2);

			//cvd1->stopDraw(); cvd2->stopDraw();


			sd = make_shared<SimpleDrawer>(*this, vec3(1, 0, 0), 1, false);


			//sd->addLoop({ vec3(100, 0, 0), vec3(0, 100, 0), vec3(0, 0, 100) });
		};


		bool intersect = false;

		virtual void onUpdate() override
		{
			sd->clear();
			sd->addPoint(vec3(2, -1, 0));

			//auto item2bbox = item2->getBBox();
			//auto qres = map->intersects(item2bbox);
			//if (qres.intersect)
			//	sd->addLoop({item2bbox.getMin(), item2bbox.getMax()});
				
			
			auto mres = map->intersects(*item2.get());
			if (mres.intersect)
				sd->addLoop(mres.intersectObj);
			
			sd->addPoints(map->intersects(item2->getBBox()).intersectObj);

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



			float k = 10*timer.getRealUpdateInterval();

			if (isKeyPressed(GLFW_KEY_DOWN))
				item2->translate(vec3(0, -k, 0));
			else if (isKeyPressed(GLFW_KEY_UP))
				item2->translate(vec3(0, k, 0));


			if (isKeyPressed(GLFW_KEY_LEFT))
				item2->translate(vec3(0, 0, k));
			else if (isKeyPressed(GLFW_KEY_RIGHT))
				item2->translate(vec3(0, 0, -k));

			if (isKeyPressed(GLFW_KEY_3))
				item2->translate(vec3(-k,0, 0 ));
			else if (isKeyPressed(GLFW_KEY_2))
				item2->translate(vec3(k,0, 0 ));

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

