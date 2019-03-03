#include <RenderThread.hpp>

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	DebugDriverLog("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n", (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
}

RenderThread::RenderThread() {
}

RenderThread::~RenderThread()
{
	stop(true);
}

std::future<bool> RenderThread::start(std::string window_name, int width, int height, bool wait_for_completion) {
	std::promise<bool> completion;
	std::future<bool> completion_result = completion.get_future();
	_internal_thread = std::thread([&]() {
		// see http://discourse.glfw.org/t/multithreading-glfw/573/5 for more info
		// Init GLFW
		GLFWwindow* _window = nullptr;
		if (!glfwInit()) {
			DebugDriverLog("GLFW init fail\n");
			completion.set_value(false);
			return;
		}
		DebugDriverLog("GLFW init success\n");
		// Open Window
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		//glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

		_window = glfwCreateWindow(width, height, window_name.c_str(), nullptr, nullptr);
		if (_window == nullptr) {
			DebugDriverLog("Error opening window\n");
			completion.set_value(false);
			return;
		}

		glfwMakeContextCurrent(_window);
		glfwSwapInterval(1);
		DebugDriverLog("Window Opened\n");

		glewExperimental = GL_TRUE;
		if (glewInit() == GLEW_OK) {
			DebugDriverLog("GLEW init success\n");

			// Enable debugging messages
			glEnable(GL_DEBUG_OUTPUT);
			glDebugMessageCallback(MessageCallback, 0);

			glEnable(GL_DEPTH_TEST);

			glDepthFunc(GL_LESS);
			glClearColor(0, 1, 1, 0);

			unsigned int VBO;
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);

			unsigned int VAO;
			glGenVertexArrays(1, &VAO);
			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(_quad_vertices), _quad_vertices, GL_STATIC_DRAW);

			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
			glEnableVertexAttribArray(2);

			const char* vertex_shader_cptr = _vertex_shader_src.c_str();
			GLuint vs = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vs, 1, &vertex_shader_cptr, NULL);
			glCompileShader(vs);

			const char* frament_shader_cptr = _fragment_shader_src.c_str();
			GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fs, 1, &frament_shader_cptr, NULL);
			glCompileShader(fs);

			GLuint shader_program = glCreateProgram();
			glAttachShader(shader_program, fs);
			glAttachShader(shader_program, vs);
			glLinkProgram(shader_program);

			if (check_shader_compile_status(vs) && !check_shader_compile_status(fs) && !check_program_link_status(shader_program)) {
				completion.set_value(false);
			}
			else {
				// Do render loop
				completion.set_value(true);
				while (_render_thread_running) {
					glfwMakeContextCurrent(_window);
					int width, height;

					glfwGetFramebufferSize(_window, &width, &height);
					glViewport(0, 0, width, height);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

					if (_render_tasks.size() > 0) {
						std::lock_guard<std::mutex> task_guard(_render_task_lock);
						for (auto render_task : _render_tasks) {
							render_task(shader_program, VAO);
						}
						_render_tasks.clear();
					}
					glfwSwapBuffers(_window);
					glfwPollEvents();
				}
			}
		}
		else {
			DebugDriverLog("GLEW init fail\n");
		}
		glfwDestroyWindow(_window);
		_window = nullptr;
		glfwTerminate();
		DebugDriverLog("Window Closed\n");
	});
	
	if (wait_for_completion) {
		completion_result.wait();
	}
	return completion_result;
}

void RenderThread::stop(bool wait_for_completion) {
	_render_thread_running = false;
	if (wait_for_completion) {
		_internal_thread.join();
	}
}

std::future<void> RenderThread::run_job(std::function<void(GLuint shader_program, GLuint VAO)> render_job, bool wait_for_completion) {
	std::promise<void> result;
	auto render_task = [&](GLuint shader_program, GLuint VAO) {
		render_job(shader_program, VAO);
		result.set_value();
	};
	std::future<void> future = result.get_future();
	{
		std::lock_guard<std::mutex> task_guard(_render_task_lock);
		_render_tasks.push_back(render_task);
	}
	if (wait_for_completion) {
		future.wait();
	}
	return future;
}

// https://github.com/progschj/OpenGL-Examples/blob/master/01shader_vbo1.cpp
bool RenderThread::check_shader_compile_status(GLuint obj) {
	GLint status;
	glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		GLint length;
		glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &length);
		std::vector<char> log(length);
		glGetShaderInfoLog(obj, length, &length, &log[0]);
		DebugDriverLog(&log[0]);
		return false;
	}
	return true;
}

bool RenderThread::check_program_link_status(GLuint obj) {
	GLint status;
	glGetProgramiv(obj, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint length;
		glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &length);
		std::vector<char> log(length);
		glGetProgramInfoLog(obj, length, &length, &log[0]);
		DebugDriverLog(&log[0]);
		return false;
	}
	return true;
}
