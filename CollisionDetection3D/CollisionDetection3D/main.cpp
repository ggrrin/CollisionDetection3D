#include "Game/MeteorTestApp.hpp"
//#include "Game/Application.hpp"
//#include "Game/TriangleTestApp.hpp"
using namespace std;
using namespace CollisionDetection3D;

int main(int argc, char** argv){

	mat4 lo = lookAt(vec3(1, 2, 10), vec3(3,4,5), vec3(6,7,8));


	Debugln("Main argc: " + std::to_string(argc));
	vector<string> args;
	for (int i = 0; i < argc; i++){
		Debugln(argv[i]);
		args.push_back(string(argv[i]));
	}

	auto app = make_unique<MeteorTestApp>();
	bool status = app->run(args);	

	return status ? 0 : 1;
}