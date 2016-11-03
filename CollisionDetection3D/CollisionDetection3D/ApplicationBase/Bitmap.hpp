#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>


#include <stdio.h>
#include <memory>
#include <string>
#include <exception>
#include "Debug.hpp"
#include "lodepng.hpp"


namespace CollisionDetection3D
{
			typedef unsigned char byte;
		struct Pixel{
			Pixel() :Pixel(0, 0, 0){};
			Pixel(byte R, byte G, byte B, byte A) : R(R), G(G), B(B), A(A){};
			Pixel(byte R, byte G, byte B) : Pixel(R, G, B, 255){};
			byte R, G, B, A;
		};

	class Bitmap
	{
	public:


		Bitmap() : Bitmap(1, 1, Pixel(255, 0, 0)){}

		Bitmap(size_t width, size_t height, Pixel p) : width(width), height(height), data(width*height, p){}

		Bitmap(const std::string& filename)
		{
			//the raw pixels ->  4 bytes per pixel, ordered RGBA
			std::vector<byte> rawData;
			unsigned error = lodepng::decode(rawData, width, height, filename.c_str());
			if (error)
				throw std::logic_error(lodepng_error_text(error));
			else		 	
				data = vector<Pixel>(reinterpret_cast<Pixel*>(rawData.begin()._Ptr), reinterpret_cast<Pixel*>(rawData.end()._Ptr));			
		};

		size_t get_width()const{
			return width;
		};

		size_t get_height() const{
			return height;
		};

		Pixel& pixelAt(size_t x, size_t y)  {
			return data.at(width*y + x);
		}

		const Pixel& pixelAt(size_t x, size_t y) const  {
			return data.at(width*y + x);
		}

		GLuint toGlTexture() const{
			// Create one OpenGL texture
			GLuint textureID;
			glGenTextures(1, &textureID);

			// "Bind" the newly created texture : all future texture functions will modify this texture
			glBindTexture(GL_TEXTURE_2D, textureID);

			// Give the image to OpenGL
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);

			// Poor filtering, or ...
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

			// ... nice trilinear filtering.
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glGenerateMipmap(GL_TEXTURE_2D);

			// Return the ID of the texture we just created
			return textureID;
		}

		~Bitmap(){};

	private:
		unsigned int width = 0, height = 0;
		std::vector<Pixel> data;
	};

}