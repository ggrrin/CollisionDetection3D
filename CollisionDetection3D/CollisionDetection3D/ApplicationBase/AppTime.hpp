#pragma once
#include <GLFW/glfw3.h>
#include <exception>

namespace CollisionDetection3D
{
	class AppTime
	{
	public:
		AppTime(double updateInterval = 0, double drawInterval = 0);
		virtual ~AppTime();

		//zero means unlimited
		//rate per second
		void setDrawRate(double rate){
			setDrawInterval(rate == 0 ? 0 : 1 / rate);
		}

		void setUpdateRate(double rate){
			setUpdateInterval(rate == 0 ? 0 : 1 / rate);
		}

		double getDrawRate() const{
			return 1 / drawInterval;
		}

		double getUpdateRate() const{
			return 1 / updateInterval;
		}
		
		//zero means unlimited
		void setDrawInterval(double seconds){
			if (seconds != 0 && updateInterval > seconds)
				;// throw std::invalid_argument("update interval cannnot be greater then draw interval. ");
			else
				drawInterval = seconds;
		}

		void setUpdateInterval(double seconds){
			if (drawInterval != 0 && drawInterval < seconds)
				;// throw std::invalid_argument("update interval cannnot be greater then draw interval. ");
			else
				updateInterval = seconds;
		}

		double getDrawInterval() const{
			return drawInterval;
		}

		double getUpdateInterval() const{
			return updateInterval;
		}

		//time from last draw to now
		double getRealDrawInterval() const{
			return realDrawInterval;
		}

		//time from last update to now
		double getRealUpdateInterval() const{
			return realUpdateInterval;
		}

		double getFPS() const{
			return fps;
		}

		bool needDraw(){
			auto a = drawRequired;
			drawRequired = false;
			return a;
		}

		void update();
		void draw();

	private:
		double updateInterval;
		double drawInterval;

		double loopStartTime;
		double realUpdateInterval;

		bool drawRequired;
		double lastDrawTime;
		double realDrawInterval;
		double currentFrameCount;
		double fps;
		double drawTimer;
	};

}