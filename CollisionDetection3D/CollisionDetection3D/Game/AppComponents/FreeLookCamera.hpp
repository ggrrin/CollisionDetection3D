#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>


namespace CollisionDetection3D
{
	using namespace glm;

	class FreeLookCamera :
		public IAppComponent
	{
		//TODO rotation depending on time~~~~~!!!!!!
	public:
		FreeLookCamera(ApplicationCore* app, vec3 positionP = vec3(0, 0, 0), vec3 lookAtP = vec3(1, 0, 0), float speedP = 2.3, float rotateSpeedFactor = 0.001f, vec3 upP = vec3(0, 1, 0)) :
			IAppComponent(app),
			position(positionP),
			direction(normalize(lookAtP - position)),
			up(normalize(upP)),
			translationVeloctiy(speedP),
			rotateSpeedFactor(rotateSpeedFactor){};
		virtual ~FreeLookCamera() override{};

	protected:
		virtual void onWindowResize(int width, int height) override
		{
			setup(width, height);
		};

		void initialize() override{
			int width, height;
			glfwGetWindowSize(getWindow(), &width, &height);

			setup(width, height);
		};

		void update() override{
			processInput();
			getViewMatrix() = lookAt(position, position + direction, up);
		};

		virtual void processInput(){
			solveKeyboard();
			solveMouse();
		};

	private:
		vec3 position;
		//always normalized
		vec3 direction;
		vec3 up;
				
		double translationVeloctiy;
		float rotateSpeedFactor;
		vec2 sceenCenter;


		void solveKeyboard(){			
			vec3 side(normalize(cross(direction, vec3(0, 1, 0))));

			vec3 move(0);
			if (isKeyPressed(GLFW_KEY_A))
				move -= side;

			if (isKeyPressed(GLFW_KEY_D))
				move += side;

			if (isKeyPressed(GLFW_KEY_S))
				move -= direction;

			if (isKeyPressed(GLFW_KEY_W))
				move += direction;

			if (move != vec3(0))
				position += (float)(translationVeloctiy * getTimer()->getRealUpdateInterval()) * normalize(move);//same speed for oblique direction
		};

		void solveMouse(){
			double x, y;
			glfwGetCursorPos(getWindow(), &x, &y);

			vec2 move(sceenCenter.x - x, y - sceenCenter.y);
			vec3 horizontal(rotate(direction, rotateSpeedFactor * move.x, vec3(0, 1, 0)));			
			direction = rotate(horizontal, rotateSpeedFactor * move.y, -normalize(cross(direction, vec3(0, 1, 0))));
			//bounding up down look
			if (sign(horizontal.x) != sign(direction.x) || sign(horizontal.z) != sign(direction.z))
				direction = horizontal;		

			direction = normalize(direction);
			glfwSetCursorPos(getWindow(), sceenCenter.x, sceenCenter.y);			
		};

		void setup(const int& width, const int& height){
			glfwSetCursorPos(getWindow(), width / 2.0, height / 2.0);
			sceenCenter = vec2(width / 2.0, height / 2.0);

			getProjectionMatrix() = glm::perspective(45.0f, width / (float)height, 0.1f, 100000.0f);
		};
	};
}

