#include "IDrawable.hpp"


namespace CollisionDetection3D
{
	mat4 IDrawable::identity = mat4(1);

	IDrawable::IDrawable(ApplicationCore* appk) : app(appk)
	{	}

	vector<vec3>& IDrawable::getLightsPositions_()
	{
		return app->lightsPositions;
	}
	mat4& IDrawable::getProjectionMatrix_()
	{
		return app->projectionMatrix;

	}
	mat4& IDrawable::getViewMatrix_()
	{
		return app->viewMatrix;

	}
	mat4& IDrawable::getWorldMatrix_()
	{
		return app->worldMatrix;

	}
	mat4& IDrawable::getMvpMatrix_()
	{
		return app->mvpMatrix;
	}

	IDrawable::~IDrawable()
	{
		// Cleanup VBO and shader
		glDeleteBuffers(1, &vertexBufferID);
		glDeleteBuffers(1, &texCoorBufferID);
		glDeleteBuffers(1, &normalBufferID);
		glDeleteBuffers(1, &indexBufferID);
		glDeleteProgram(programShaderID);
		glDeleteTextures(1, &textureID);
	}

	GLuint IDrawable::programShaderID = 0;

	void IDrawable::initialize()
	{
		// Create and compile our GLSL program from the shaders
		if (programShaderID == 0)
			programShaderID = LoadShaders("Data/StandardShading.vertexshader", "Data/StandardShading.fragmentshader");

		// Get a handle for our "MVP" uniform
		mvpMatrixShaderID = glGetUniformLocation(programShaderID, "MVP");
		viewMatrixShaderID = glGetUniformLocation(programShaderID, "V");
		modelMatrixShaderID = glGetUniformLocation(programShaderID, "M");

		// Get a handle for our "myTextureSampler" uniform
		textureShaderID = glGetUniformLocation(programShaderID, "myTextureSampler");

		// Load it into a VBO
		glGenBuffers(1, &vertexBufferID);//vytvori buffer vrati jeho idendifikator
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);//nastavi se ze se pracuje s buffrem daneho identifikatoru
		glBufferData(GL_ARRAY_BUFFER, getVertexBuffer().size() * sizeof(glm::vec3), &getVertexBuffer()[0], GL_STATIC_DRAW); //prace s nabindovanym bufferm

		glGenBuffers(1, &texCoorBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, texCoorBufferID);
		glBufferData(GL_ARRAY_BUFFER, getTexCoorBuffer().size() * sizeof(glm::vec2), &getTexCoorBuffer()[0], GL_STATIC_DRAW);

		glGenBuffers(1, &normalBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
		glBufferData(GL_ARRAY_BUFFER, getNormalBuffer().size() * sizeof(glm::vec3), &getNormalBuffer()[0], GL_STATIC_DRAW);

		// Generate a buffer for the indices as well
		glGenBuffers(1, &indexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, getIndexBuffer().size() * sizeof(unsigned int), &getIndexBuffer()[0], GL_STATIC_DRAW);

		// Get a handle for our "LightPosition" uniform
		glUseProgram(programShaderID);
		lightShaderID = glGetUniformLocation(programShaderID, "LightPosition_worldspace");

		textureID = getTexture().toGlTexture();

	}

	void IDrawable::draw()
	{
		mat4 temp = getProjectionMatrix_() * getViewMatrix_();
			getMvpMatrix_() = getProjectionMatrix_() * getViewMatrix_() * getmodelMatrix();
		// Use our shader
		glUseProgram(programShaderID);

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(mvpMatrixShaderID, 1, GL_FALSE, &getMvpMatrix_()[0][0]);
		glUniformMatrix4fv(modelMatrixShaderID, 1, GL_FALSE, &getmodelMatrix()[0][0]);
		glUniformMatrix4fv(viewMatrixShaderID, 1, GL_FALSE, &getViewMatrix_()[0][0]);
		glUniform3f(lightShaderID, getLightsPositions_()[0].x, getLightsPositions_()[0].y, getLightsPositions_()[0].z);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(textureShaderID, 0);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, texCoorBufferID);
		glVertexAttribPointer(
			1,                                // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);

		// 3rd attribute buffer : normals
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
		glVertexAttribPointer(
			2,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);

		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

		// Draw the triangles !
		glDrawElements(
			GL_TRIANGLES,      // mode
			getIndexBuffer().size(),    // count
			GL_UNSIGNED_INT,   // type////////////////////////////////
			(void*)0           // element array buffer offset
			);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
	}


}
