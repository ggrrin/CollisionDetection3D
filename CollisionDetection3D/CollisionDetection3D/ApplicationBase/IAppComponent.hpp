#pragma once
#include "ApplicationCore.hpp"

namespace CollisionDetection3D
{
	using namespace glm;
	class ApplicationCore;

	class IAppComponent
	{
	friend class ApplicationCore;
	public:
		IAppComponent(ApplicationCore* appP);
		virtual ~IAppComponent();		

	protected:
		virtual void initialize();
		virtual void update();
		virtual void draw();
		virtual void exit();

		virtual void onWindowResize(int widht, int height);

		bool isKeyPressed(int key);

		AppTime* getTimer();
		GLFWwindow* getWindow();

		std::vector<vec3>& getLightsPositions();
		mat4& getProjectionMatrix();
		mat4& getViewMatrix();
		mat4& getWorldMatrix();
		mat4& getMvpMatrix();

	private:		
		ApplicationCore* app;
		size_t ID;
	};
}

