#pragma once
#define point_col
#include "../ApplicationBase/ApplicationCore.hpp"
#include "AppComponents/FreeLookCamera.hpp"
#include "AppComponents/HeightMap.hpp"
#include "AppComponents/InfoDrawer.hpp"
#include "AppComponents/SimpleDrawer.hpp"
#include <random>
#include <functional>
#include "Interaction/Bounding/ObjOcTree.hpp"
#include "Body.hpp"
#include "AppComponents/SimpleDrawer.hpp"
#include "Interaction/Bounding/MapOcTree.hpp"

namespace CollisionDetection3D
{
	class MeteorTestApp : public ApplicationCore
	{
	public:
		MeteorTestApp(){};
		virtual ~MeteorTestApp(){};
	private:
		shared_ptr<HeightMap> map;
		shared_ptr<FreeLookCamera> camera;
		vector<shared_ptr<Body>> meteors;

		vector<vector<vec3>> meteorsVertices;
		vector<vector<unsigned int>> meteorsIndices;

		ObjOcTree tree;
		MapOcTree mapTree;

		unique_ptr<SimpleDrawer> vd;

		virtual void onInitialize(const vector<string>& args) override{
			timer.setDrawRate(60);
			glClearColor(0.33f, 0.9f, 1.0f, 0.0f);
			
			lightsPositions.push_back(vec3(128, 255, 128));
			camera = make_shared<FreeLookCamera>(this, vec3(-30, 150 ,-30), vec3(60), 25.0f);
			
			Bitmap y("Data/texture00.png");
			map = make_shared<HeightMap>(this, y, Bitmap("Data/map01.png "), 50.0f);			
			
			LoadMeteorsData();
			InitializeMeteors();
			InitializeOcTree();

			
		};

		void InitializeOcTree()
		{
			vd = make_unique<SimpleDrawer>(*this);

			//map tree
			auto mapBbox = map->getBBox();
			mapTree = MapOcTree(mapBbox, vector<shared_ptr<HeightMap>>(1, map), 5, true);
			mapTree.forEeachLeaf([this](MapOcTree::leafType* leaf){
				//vd->initBbox(leaf->box, vec3(1,0,0));
			});


			//Objects tree
			float  maxY = mapBbox.getMax().y;

			for (const auto& obj : meteors)
			{
				auto curY = obj->getBBox().getMax().y;
				if (curY > maxY)
					maxY = curY;
			}

			vec3 prevMax = mapBbox.getMax();

			//inflate BBox to fit all ConvecObj
			mapBbox = BoundingBox(mapBbox.getMin(), vec3(prevMax.x, maxY, prevMax.z));

			Debugln("bulinding ocTree....");
			tree = ObjOcTree(mapBbox, meteors, 4);
			Debugln("ocTree built.");

			int i = 0;
			

			tree.forEeachLeaf([this,&i]( ObjOcTree::leafType* leaf){
				//std::cout << "Leaf " << i++ << " Obj count: " << leaf->getTriangles().size() << endl;
				//vd->initBbox(leaf->box);
			
			});
		}


		void InitializeMeteors()
		{
			const int seed = 13;
			const int heightSurface = 50;
			
			size_t dataCount = meteorsVertices.size() - 1;


			std::default_random_engine generator;
			generator.seed(seed);
			std::uniform_int_distribution<int> distribution(0, dataCount);
			auto rnd = bind(distribution, generator);

			///////////////////////////////////
			//meteors.push_back(make_shared<Body>(
			//	*this,
			//	meteorsVertices[23],
			//	meteorsIndices[23],
			//	vec3(80, heightSurface, 80),
			//	1.4f *((float)rnd() + (float)dataCount) / (float)dataCount,
			//	Pixel(rnd() * 4, rnd() * 4, rnd() * 4, 255),
			//	0.0001f*vec3(1),
			//	0));
			//
			//meteors.push_back(make_shared<Body>(
			//	*this,
			//	meteorsVertices[23],
			//	meteorsIndices[23],
			//	vec3(30,100,240),
			//	1.4f *((float)rnd() + (float)dataCount) / (float)dataCount,
			//	Pixel(rnd() * 4, rnd() * 4, rnd() * 4, 255),
			//	vec3(0),
			//	1));
			//
			//meteors.push_back(make_shared<Body>(
			//	*this,
			//	meteorsVertices[23],
			//	meteorsIndices[23],
			//	vec3(240,100,30),
			//	1.4f *((float)rnd() + (float)dataCount) / (float)dataCount,
			//	Pixel(rnd() * 4, rnd() * 4, rnd() * 4, 255),
			//	vec3(0),
			//	2));
			//
			//meteors.push_back(make_shared<Body>(
			//	*this,
			//	meteorsVertices[23],
			//	meteorsIndices[23],
			//	vec3(240),
			//	1.4f *((float)rnd() + (float)dataCount) / (float)dataCount,
			//	Pixel(rnd() * 4, rnd() * 4, rnd() * 4, 255),
			//	vec3(0),
			//	3));
			//
			//return;
			
			size_t i = 0;

			vec3 collPoint(50);

			const size_t diff = 40;

			for (size_t z = 0; z < map->getZWidth(); z += diff)
			{
				for (size_t x = 0; x < map->getXWidth(); x += diff)
				{
					vec3 position = vec3(x, heightSurface + rnd(), z);

					Debugln("initialzing convex " + to_string(i));
					int index = rnd();

					meteors.push_back(make_shared<Body>(
						*this,
						meteorsVertices[index],
						meteorsIndices[index],
						position ,
						1.3f *((float)rnd() + (float)dataCount/2) / (float)dataCount,
						Pixel(rnd() * 4, rnd() * 4, rnd() * 4, 255),

#ifdef point_col
						normalize(collPoint - position)*10.0f,
#else
						10.0f*normalize(vec3(rnd(), -10*rnd(), rnd())),
#endif
						i++));
								
				}
			}

		
		}

		size_t LoadMeteorsData()
		{
			const size_t count = 59;

			meteorsVertices = vector<vector<vec3>>(count);
			meteorsIndices = vector<vector<unsigned int>>(count);

			for (size_t i = 0; i < count; i++)
			{
				string file = "Data/Meteors/Output" + to_string(i) + ".cob";

				if (!ConvexObj::loadModel(file, meteorsVertices[i], meteorsIndices[i]))
					throw logic_error(("Cannot load model " + file).c_str());
			}
			return count;
		}


		bool intersect = false;

		virtual void onUpdate() override
		{			
			double time = timer.getRealUpdateInterval();
			
			for (const auto& obj : meteors)
				obj->translate(obj->velocity * (float)time);	

			updateTree();
			MapCollision();
			Collision();
		}

		void MapCollision()
		{
			for (auto& m : meteors)
			{
				mapTree.forEeachLeafIf([this,&m](MapOcTree::leafType* leaf){
					auto res = map->intersects(*m);
					if (res.intersect)
						m->remove = true;

				}, [&m](OcElement* p){return p->box.intersects(m->getBBox()); });
			}

			//delete from global storage
			auto r = remove_if(meteors.begin(), meteors.end(), [](shared_ptr<Body> d){
				return d->remove;
			});
			meteors.erase(r, meteors.end());
		}

		virtual void onDraw() override{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			for (const auto& i : meteors)
				i->draw();
		};

		virtual void onExit() override{};

		void Collision()
		{
			//collision
			tree.forEeachLeaf([this](ObjOcTree::leafType* leaf){

				if (leaf->stuffs.size() < 2)
					return;

				for (size_t i = 0; i < leaf->stuffs.size(); i++)
				{
					for (size_t j = i + 1; j < leaf->stuffs.size(); j++)
					{
						auto a = leaf->stuffs[i];
						auto b = leaf->stuffs[j];

				
						auto res = a->intersects(*b);
						//auto res = a->getBBox().intersects(b->getBBox());
						if (res.intersect)
						{
							//vd->addLoop(res.intersectObj);

							//cout << "collsion detected " << leaf->stuffs[i]->id << " " << leaf->stuffs[j]->id << endl;							
							a->remove = b->remove = true;
						}
					}
				}

				//deletefrom leaf
				auto r = remove_if(leaf->stuffs.begin(), leaf->stuffs.end(), [](shared_ptr<Body> d){
					return d->remove;
				});
				leaf->stuffs.erase(r, leaf->stuffs.end());
			});


			//delete from global storage
			auto r = remove_if(meteors.begin(), meteors.end(), [](shared_ptr<Body> d){
				return d->remove||d->nodes.empty();
			});
			meteors.erase(r, meteors.end());
		}


		void updateTree()
		{
			tree.forEeachLeaf([this](ObjOcTree::leafType* leaf){
				for (const auto& stuff : leaf->stuffs)
				{
					if (stuff->touched)
						continue;
					//uz prohledano z nejake krabice
					stuff->touched = true;



					auto bb = stuff->getBBox();

					for (const auto& corner : bb.getCorners())
					{
						if (!leaf->box.intersects(corner))
						{//stuff escaping from box 
							auto newNodes = tree.getCollidingLeaves(stuff->getBBox());

							//copy stuff to new boxes
							for (auto& newLeaf : newNodes)
							{
								bool boxIsNotCurrentlyInUse = find(stuff->nodes.begin(), stuff->nodes.end(), newLeaf) == stuff->nodes.end();

								if (boxIsNotCurrentlyInUse)
								{
									newLeaf->stuffs.push_back(stuff);
									stuff->nodes.push_back(newLeaf);

					/*				cout << "copy stuff " << stuff->id << " from "
										<< leaf->pos.x << " " << leaf->pos.y << " " << leaf->pos.z <<
										" to "
										<< newLeaf->pos.x << " " << newLeaf->pos.y << " " << newLeaf->pos.z << " "
										<< endl;*/
								}
							}

							if (!bb.intersects(leaf->box))
							{//remove stuff from current box 
								stuff->move = true; // nastavit ze ma byt odstranen z cur listu

								//odstraneni z seznamu stuffu
								stuff->nodes.erase(remove_if(stuff->nodes.begin(), stuff->nodes.end(), [leaf](ObjOcTree::leafType* l){
									return l == leaf;
								}));

								//cout << "removing stuff " << stuff->id << " from " << leaf->pos.x << " " << leaf->pos.y << " " << leaf->pos.z << endl;
							}
							break;
						}
					}
				}

				//vymaznai z toho to listu
				auto r = remove_if(leaf->stuffs.begin(), leaf->stuffs.end(), [](shared_ptr<Body> d){
					bool r = d->move;
					d->move = false;
					return r;
				});
				leaf->stuffs.erase(r, leaf->stuffs.end());


			});

			for (auto& m : meteors)
				m->touched = false;
		}
	};

	
}

