#pragma once
#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <vector>

inline void Debug(const char* arg){
#ifdef DEBUG 
	std::cout << arg;
#endif
}

inline void Debug(const std::string& arg){
	Debug(arg.c_str());
}

inline void Debugln(const std::string& arg){
	Debug((arg + "\n").c_str());
}

inline void Debugln(const char* arg){
	Debugln(std::string(arg));
}

inline void Debugln(std::initializer_list<glm::vec3> items){

	for (auto i : items)
	{
		Debug("[ ");
		Debug(std::to_string(i.x));
		Debug(", ");
		Debug(std::to_string(i.y));
		Debug(", ");
		Debug(std::to_string(i.z));
		Debug(" ] ");
	}

	Debugln("");

	
}

inline void Debugln(std::vector<glm::vec3> items){

	for (auto i : items)
	{
		Debug("[ ");
		Debug(std::to_string(i.x));
		Debug(", ");
		Debug(std::to_string(i.y));
		Debug(", ");
		Debug(std::to_string(i.z));
		Debug(" ] ");
	}

	Debugln("");


}



