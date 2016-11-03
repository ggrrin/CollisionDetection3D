#pragma once
#include <GLFW/glfw3.h>
#include <vector>

namespace CollisionDetection3D
{
	using namespace std;

	template<typename T>
	struct Result
	{
		Result(){};
		Result(T value) : value(value) ,cached(true){};

		bool cached = false;
		T value = T();
	};

	template<typename T>
	class Cache 
	{
	public:
		typedef pair<double, T> cell;
		Cache() :data(0), width_(0), resetTime(0){}

		void prepareCache(size_t length, size_t width)
		{
			this->width_ = width;
			this->length_ = length;
			this->resetTime = glfwGetTime();
		}

		Result<T> getValue(size_t lengthIndex, size_t widthIndex)
		{
			if (lengthIndex >= data.size())
				return Result<T>(); //notcached
			else if (widthIndex >= data[lengthIndex].size())
				return Result<T>();//notcached
			else if (data[lengthIndex][widthIndex].first < resetTime)
				return Result<T>();//not cached actual value
			else
				return Result<T>(data[lengthIndex][widthIndex].second);
		}

		void setValue(size_t lengthIndex, size_t widthIndex, const  T& item)
		{
			if (lengthIndex >= data.size())
				data.resize(length_);

			if (widthIndex >= data[lengthIndex].size())
				data[lengthIndex].resize(this->width_, cell(0,T()));

			data[lengthIndex][widthIndex] = cell(glfwGetTime(), item);
		}

		size_t getLength() const
		{
			return length_;
		}


		size_t getWidth() const
		{
			return width;
		}

	private:
		double resetTime;
		vector<vector<cell>> data;
		size_t length_;
		size_t width_;

	};


}