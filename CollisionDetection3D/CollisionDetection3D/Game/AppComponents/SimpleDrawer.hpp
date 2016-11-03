#pragma once
#include "../../ApplicationBase/IAppComponent.hpp"
#include <iterator>

namespace CollisionDetection3D
{
	class ApplicationCore;
	using namespace glm;
	using namespace std;

	template<class Dummy> class StatInit{ protected: static mat4 identity; };
	template<class Dummy> mat4 StatInit<Dummy>::identity = mat4(1);

	class SimpleDrawer : public IAppComponent, StatInit<void>
	{
	public:
		SimpleDrawer(ApplicationCore& app, vec3 color = vec3(1, 1, 0), float lineWidth = 1.0f, bool antialiasing = false) :
			lineWidth(lineWidth), antialiasing(antialiasing), defaultColor(color), IAppComponent(&app){}

		virtual ~SimpleDrawer()
		{
			glDeleteBuffers(1, &vertexBufferID);
			glDeleteBuffers(1, &colorBufferID);
			glDeleteProgram(programShaderID);
		}


		void addVector(const vec3& position, const vec3& v, const vec3& color = vec3(-1))
		{
			vec3 usedColor = color;
			if (color == vec3(-1))
				usedColor = defaultColor;

			vertexBuffer.push_back(position);
			vertexBuffer.push_back(position + v);

			colorBuffer.push_back(usedColor);
			colorBuffer.push_back(usedColor);

			updateBufferRequired = true;
		}

		void addSegment(const vec3& p1, const vec3& p2, const vec3& color = vec3(-1))
		{
			addVector(p1, p2 - p1, color);
		}


		template<typename iter>
		void addLoop(const iter begin, const  iter end, const vec3& color = vec3(-1))
		{
			iter cur = begin + 1;

			addSegment(begin[0], end[-1], color);
			for (; cur != end; ++cur)
				addSegment(cur[-1], cur[0], color);
		}

		void addLoop(vector<vec3> items, const vec3& color = vec3(-1))
		{
			if (items.size() > 0)
				addLoop(items.begin(), items.end(), color);
		}

		void addLoop(initializer_list<vec3> items, const vec3& color = vec3(-1))
		{
			addLoop(items.begin(), items.end(), color);
		}

		float defaultPointSize = 0.3f;

		void addPoint(const vec3& point, const vec3& color = vec3(-1), float size = -1)
		{
			if (size == -1)
				size = defaultPointSize;

			float half = 0.5f * size;

			addVector(point - half * vec3(1, 0, 0), size * vec3(1, 0, 0), color);
			addVector(point - half * vec3(0, 1, 0), size * vec3(0, 1, 0), color);
			addVector(point - half * vec3(0, 0, 1), size * vec3(0, 0, 1), color);
		}

		template<typename iterator>
		void addPoints(const iterator begin, const iterator end, const vec3& color = vec3(-1), float size = -1)
		{
			auto cur = begin;
			for (; cur != end; ++cur)
				addPoint(*cur, color, size);
		}

		void addPoints(const vector<vec3> points, const vec3& color = vec3(-1), float size = -1)
		{
			addPoints(points.begin(), points.end(), color, size);
		}

		void addPoints(initializer_list<vec3> points, const vec3& color = vec3(-1), float size = 1)
		{
			addPoints(points.begin(), points.end(), color, size);
		}

		void addTriangle(const Triangle& t)
		{
			addLoop({ t.getTv1(), t.getTv2(), t.getTv3() });
		}

		void clear(){
			vertexBuffer.clear();
			colorBuffer.clear();
			lastCapacity = lastSize = 0;
			updateBufferRequired = true;
		}

		void initBbox(const BoundingBox& bb, vec3& BBoxColor = vec3(-1), const mat4& mod = mat4(1))
		{
			if (BBoxColor == vec3(-1))
				 BBoxColor = defaultColor;

			auto m = bb.getMin();
			auto n = bb.getMax();

			auto a = vec3(n.x - m.x, 0, 0);
			auto b = vec3(0, n.y - m.y, 0);
			auto c = vec3(0, 0, n.z - m.z);

			addSegment(trans(mod, m), trans(mod, m + a), BBoxColor);
			addSegment(trans(mod, m), trans(mod, m + a), BBoxColor);
			addSegment(trans(mod, m), trans(mod, m + b), BBoxColor);
			addSegment(trans(mod, m), trans(mod, m + c), BBoxColor);
			addSegment(trans(mod, n), trans(mod, n - a), BBoxColor);
			addSegment(trans(mod, n), trans(mod, n - b), BBoxColor);
			addSegment(trans(mod, n), trans(mod, n - c), BBoxColor);
			addSegment(trans(mod, m + b), trans(mod, m + b + a), BBoxColor);
			addSegment(trans(mod, m + b), trans(mod, m + b + c), BBoxColor);
			addSegment(trans(mod, n - b), trans(mod, n - b - a), BBoxColor);
			addSegment(trans(mod, n - b), trans(mod, n - b - c), BBoxColor);
			addSegment(trans(mod, m + a), trans(mod, m + a + b), BBoxColor);
			addSegment(trans(mod, m + c), trans(mod, m + c + b), BBoxColor);
		}

		bool drawing = true;

	protected:
		virtual const mat4& getModelMatrix() const
		{
			return identity;
		}
	private:

		vec3 defaultColor;
		GLfloat lineWidth;
		bool antialiasing;
		bool updateBufferRequired = false;

		//ids of copied buffer in graphic card
		GLuint vertexBufferID;
		GLuint colorBufferID;

		//variables sent to shaders
		GLuint programShaderID;
		GLuint mvpMatrixShaderID;

		vector<vec3> vertexBuffer;
		vector<vec3> colorBuffer;

		virtual void initialize() override final
		{
			programShaderID = LoadShaders("Data/simpleColor.vertexshader", "Data/simpleColor.fragmentshader");
			mvpMatrixShaderID = glGetUniformLocation(programShaderID, "MVP");

			glGenBuffers(1, &vertexBufferID);
			glGenBuffers(1, &colorBufferID);
			updateBuffers();
		}

		vector<vec3>::size_type lastCapacity = 0;
		vector<vec3>::size_type lastSize = 0;

		void updateBuffers()
		{
			if (lastCapacity >= vertexBuffer.capacity())//buffer size didnt change => add only new vertices	
			{
				glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
				glBufferSubData(GL_ARRAY_BUFFER, lastSize* sizeof(vec3), (vertexBuffer.size() - lastSize) * sizeof(vec3), &vertexBuffer[lastSize]);

				glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
				glBufferSubData(GL_ARRAY_BUFFER, lastSize* sizeof(vec3), (colorBuffer.size() - lastSize) * sizeof(vec3), &colorBuffer[lastSize]);
			}
			else//realocate whole buffer	
			{
				glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
				glBufferData(GL_ARRAY_BUFFER, vertexBuffer.capacity() * sizeof(vec3), NULL, GL_DYNAMIC_DRAW);
				glBufferSubData(GL_ARRAY_BUFFER, 0, vertexBuffer.size() * sizeof(vec3), &vertexBuffer[0]);

				glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
				glBufferData(GL_ARRAY_BUFFER, colorBuffer.capacity() * sizeof(vec3), NULL, GL_DYNAMIC_DRAW);
				glBufferSubData(GL_ARRAY_BUFFER, 0, colorBuffer.size() * sizeof(vec3), &colorBuffer[0]);

				lastCapacity = vertexBuffer.capacity();
			}
			lastSize = vertexBuffer.size();
		}


		virtual void draw() override final
		{
			if (!drawing)
				return;

			if (updateBufferRequired)
			{
				updateBuffers();
				updateBufferRequired = false;
			}

			getMvpMatrix() = getProjectionMatrix() * getViewMatrix() * getModelMatrix();

			GLfloat prevWidth;
			glGetFloatv(GL_LINE_WIDTH, &prevWidth);
			glLineWidth(lineWidth);

			bool prevAntial = glIsEnabled(GL_LINE_SMOOTH);

			if (antialiasing)
				glEnable(GL_LINE_SMOOTH);
			else
				glDisable(GL_LINE_SMOOTH);

			glUseProgram(programShaderID);

			glUniformMatrix4fv(mvpMatrixShaderID, 1, GL_FALSE, &getMvpMatrix()[0][0]);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

			glDrawArrays(GL_LINES, 0, vertexBuffer.size());
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);

			glLineWidth(prevWidth);

			if (prevAntial)
				glEnable(GL_LINE_SMOOTH);
			else
				glDisable(GL_LINE_SMOOTH);
		}
	};

}