#pragma once
#include "AppTime.hpp"

namespace CollisionDetection3D
{
	AppTime::AppTime(double updateInterval, double drawInterval) :
		updateInterval(updateInterval),
		drawInterval(drawInterval),
		drawRequired(true),
		currentFrameCount(0),
		lastDrawTime(0)
	{
		if (drawInterval != 0 && updateInterval > drawInterval)
			;// throw std::invalid_argument("update interval cannnot be greater then draw interval. ");
	}

	AppTime::~AppTime(){};

	void AppTime::update()
	{
		if (updateInterval != 0){
			double remainingTime = updateInterval - (glfwGetTime() - loopStartTime);
			double beforeLoop = glfwGetTime();
			while ((glfwGetTime() - beforeLoop) < remainingTime)
				;//TODO passive wait for big intervals
		}

		double currentTime = glfwGetTime();
		realUpdateInterval = currentTime - loopStartTime;		

		//LOOP START
		//////////////////////////////////////////////////////////////////////
		loopStartTime = glfwGetTime();

		drawRequired = (glfwGetTime() - lastDrawTime) > drawInterval;
	}

	void AppTime::draw()
	{
		double currentTime = glfwGetTime();
		drawTimer += currentTime - lastDrawTime;
		if (drawTimer >= 1)
		{
			fps = currentFrameCount;
			drawTimer = currentFrameCount = 0;
		}

		//Debugln("fps: " +std::to_string(fps));
		//Debugln("update interval: " + std::to_string(currentTime - loopStartTime));

		realDrawInterval = currentTime - lastDrawTime;

		//DRAW START	
		///////////////////////////////////////////////////////////////
		lastDrawTime = currentTime;
		currentFrameCount++;
	}
}