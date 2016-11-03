#include "ApplicationCore.hpp"

namespace CollisionDetection3D
{
	ApplicationCore* ApplicationCore::currentApp = NULL;

	ApplicationCore::ApplicationCore()
	{
		if (currentApp != NULL)
			throw logic_error("Can NOT create multiple instance of an application");
		else
			currentApp = this;
	}

	ApplicationCore::~ApplicationCore()
	{
		if (ApplicationCore::currentApp == NULL)
			assert(true);
		
		currentApp = NULL;
	}

	bool ApplicationCore::run(const vector<string>& args)
	{
		if (!initializeWindow(args))
			return false;

		initialize(args);
		loop();
		exit();
		return true;
	}

	bool ApplicationCore::initializeWindow(const vector<string>& args)
	{
		// Initialise GLFW
		if (!glfwInit())
		{
			fprintf(stderr, "Failed to initialize GLFW\n");
			return false;
		}

		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// Open a window and create its OpenGL context
		window = glfwCreateWindow(546, 384, args[0].c_str(), NULL, NULL);
		if (window == NULL){
			fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
			glfwTerminate();
			return false;
		}
		glfwMakeContextCurrent(window);

		// Initialize GLEW
		glewExperimental = true; // Needed for core profile
		if (glewInit() != GLEW_OK) {
			fprintf(stderr, "Failed to initialize GLEW\n");
			glfwTerminate();
			return false;
		}

		// Ensure we can capture the escape key being pressed below
		glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

		//////////////////////////////////////////////////////////////////// InitVAO buffer & other GL

		// Dark blue background
		glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

		// Enable depth test
		glEnable(GL_DEPTH_TEST);
		// Accept fragment if it closer to the camera than the former one
		glDepthFunc(GL_LESS);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// Cull triangles which normal is not towards the camera
		glEnable(GL_CULL_FACE);

		glGenVertexArrays(1, &vertexArrayID);
		glBindVertexArray(vertexArrayID);

		return true;
	}

	void ApplicationCore::loop()
	{
		while (!stopLoop){
			timer.update();
			update();		

			if (timer.needDraw()){
				timer.draw();
				draw();
			}
		}
	};

	void ApplicationCore::update()
	{
		glfwPollEvents();

		if (glfwWindowShouldClose(window) != 0)
			stopLoop = true;



		onUpdate();

		for (auto i : components)
			i->update();
	}

	void ApplicationCore::draw()
	{
		onDraw();

		for (auto i : components)
			i->draw();

		glfwSwapBuffers(window);
	}

	void ApplicationCore::exit()
	{
		for (auto i : components)
			i->exit();

		onExit();

		//Delete VAO
		glDeleteVertexArrays(1, &vertexArrayID);

		// Close OpenGL window and terminate GLFW
		glfwTerminate();
	}

	void ApplicationCore::initialize(const vector<string>& args)
	{
		onInitialize(args);

		glfwSetWindowSizeCallback(window, windowResizeCallback);

		for (auto i : components)
			i->initialize();

		initialized = true;
	}

	void ApplicationCore::windowResizeCallback(GLFWwindow* window, int width, int height)
	{
		if (width > 0 && height > 0){
			glViewport(0, 0, width, height);

			for (auto i : currentApp->components)
				i->onWindowResize(width, height);

			currentApp->onWindowResize(width, height);
		}
	}

	void ApplicationCore::registerComponent(IAppComponent* cmp)
	{
		cmp->ID = components.size();
		components.push_back(cmp);		
	}

	void ApplicationCore::unregisterComponent(IAppComponent* cmp)
	{
		components.erase(components.begin() + cmp->ID);
		for (auto& it = components.begin() + cmp->ID; it != components.end(); ++it)
			(*(it))->ID--;
	}

	bool ApplicationCore::isInitialized() const
	{
		return initialized;
	}

	bool ApplicationCore::isKeyPressed(int key){
		int status = glfwGetKey(window, key);
		return  status == GLFW_PRESS || status == GLFW_REPEAT;
	}

}