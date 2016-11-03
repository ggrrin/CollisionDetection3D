#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include "shader.hpp"
#include "Bitmap.hpp"
#include "ApplicationCore.hpp"

namespace CollisionDetection3D
{
	using namespace std;
	using namespace glm;

	enum DrawMode 
	{ None=0, Textured=1 };

	class IDrawable
	{		
	public:
		IDrawable(ApplicationCore* app);
		virtual ~IDrawable();
		virtual void initialize();
		virtual void draw();

		//current object layer
		virtual const vector<unsigned int>& getIndexBuffer() const = 0;
		virtual const vector<vec3>& getVertexBuffer() const = 0;
		virtual const vector<vec3>& getNormalBuffer() const = 0;
		virtual const vector<vec2>& getTexCoorBuffer() const = 0;
		virtual const Bitmap& getTexture() const = 0;
		virtual const mat4& getmodelMatrix() const { return identity; };

	protected:
		static mat4 identity;

		//Idrawable layer
		//ids of copied buffer in graphic card
		GLuint textureID;
		GLuint texCoorBufferID;
		GLuint normalBufferID;
		GLuint vertexBufferID;
		GLuint indexBufferID;

		//variables sent to shaders
		GLuint lightShaderID;
		GLuint textureShaderID;
		static GLuint programShaderID;
		GLuint mvpMatrixShaderID;
		GLuint viewMatrixShaderID;
		GLuint modelMatrixShaderID;
	private:
		ApplicationCore* app;
		vector<vec3>& getLightsPositions_();
		mat4& getProjectionMatrix_();
		mat4& getViewMatrix_();
		mat4& getWorldMatrix_();
		mat4& getMvpMatrix_();
	};

	
}

