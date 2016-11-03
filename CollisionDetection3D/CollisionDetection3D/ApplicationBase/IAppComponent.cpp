#include "IAppComponent.hpp"

namespace CollisionDetection3D
{
	IAppComponent::IAppComponent(ApplicationCore* ac) : app(ac)
	{
		app->registerComponent(this);
	}


	IAppComponent::~IAppComponent()
	{
		app->unregisterComponent(this);
	}

	AppTime* IAppComponent::getTimer(){
		return &app->timer;
	}

	GLFWwindow* IAppComponent::getWindow()
	{
		return app->window;
	}
	vector<vec3>& IAppComponent::getLightsPositions()
	{
		return app->lightsPositions;
	}

	mat4& IAppComponent::getProjectionMatrix()
	{
		return app->projectionMatrix;
	}

	mat4& IAppComponent::getViewMatrix()
	{
		return app->viewMatrix;
	}

	mat4& IAppComponent::getWorldMatrix()
	{
		return app->worldMatrix;
	}

	mat4& IAppComponent::getMvpMatrix()
	{
		return app->mvpMatrix;
	}

	bool IAppComponent::isKeyPressed(int key){
		int status = glfwGetKey(getWindow(), key);
		return  status == GLFW_PRESS || status == GLFW_REPEAT;
	};

	void IAppComponent::initialize(){};
	void IAppComponent::update(){};
	void IAppComponent::draw(){};
	void IAppComponent::exit(){};

	void IAppComponent::onWindowResize(int widht, int height){};
}