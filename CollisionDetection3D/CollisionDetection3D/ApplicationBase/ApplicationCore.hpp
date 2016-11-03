#pragma once
#include <GL/glew.h>
#include "Debug.hpp"
#include "glm/glm.hpp"
#include <vector>
#include <GLFW/glfw3.h>
#include "AppTime.hpp"
#include "IAppComponent.hpp"

namespace CollisionDetection3D
{
	using namespace std;
	using namespace glm;
	


	class ApplicationCore
	{
	friend class IAppComponent;
	public:		

		ApplicationCore();
		virtual ~ApplicationCore();
		bool run(const vector<string>& args);
		vector<vec3> lightsPositions;
		mat4 projectionMatrix;
		mat4 viewMatrix;
		mat4 worldMatrix;
		mat4 mvpMatrix;		

		bool isInitialized() const;

		bool isKeyPressed(int key);

	protected:
		vector<IAppComponent*> components;
		AppTime timer;

		GLFWwindow* window;		
		virtual void onWindowResize(int width, int height){};

		virtual void onInitialize(const vector<string>& args) = 0;
		virtual void beforeUpdate(){};
		virtual void onUpdate() = 0;
		virtual void onDraw() = 0;
		virtual void onExit() = 0;

		GLuint vertexArrayID;

	private:
		bool initialized = false;

		bool stopLoop = false;
		bool initializeWindow(const vector<string>& args);
		void loop();

		void initialize(const vector<string>& args);
		void update();
		void draw();
		void exit();

		void registerComponent(IAppComponent* cmp);
		void unregisterComponent(IAppComponent* cmp);

		static void windowResizeCallback(GLFWwindow* window, int width, int height);
		static ApplicationCore* currentApp;

	};

	
}


