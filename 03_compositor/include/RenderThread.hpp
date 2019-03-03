#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <thread>
#include <array>
#include <condition_variable>
#include <mutex>
#include <future>

#include <openvr_driver.h>
#include <DriverLog.hpp>

class RenderThread {
public:
	RenderThread();
	~RenderThread();

	// starts render thread
	std::future<bool> start(std::string window_name, int width, int height, bool wait_for_completion  = false);

	// stops render thead
	void stop(bool wait_for_completion = false);

	// Submits a render job to be run on next frame
	std::future<void> run_job(std::function<void(GLuint shader_program, GLuint VAO)> render_job, bool wait_for_completion = false);

private:
	std::thread	_internal_thread;
	bool _render_thread_running;

	std::mutex _render_task_lock;
	std::vector<std::function<void(GLuint shader_program, GLuint VAO)>> _render_tasks;

	const std::string _vertex_shader_src =
		"#version 330 core						\n\
		layout(location = 0) in vec3 aPos;		\n\
		layout(location = 1) in vec3 aColor;	\n\
		layout(location = 2) in vec2 aTexCoord;	\n\
												\n\
		out vec3 ourColor;						\n\
		out vec2 TexCoord;						\n\
												\n\
		void main()								\n\
		{										\n\
			gl_Position = vec4(aPos, 0.0);		\n\
			ourColor = aColor;					\n\
			TexCoord = aTexCoord;				\n\
		}";

	const std::string _fragment_shader_src =
		"#version 330 core								\n\
		out vec4 FragColor;								\n\
														\n\
		in vec3 ourColor;								\n\
		in vec2 TexCoord;								\n\
														\n\
		uniform sampler2D texture1;						\n\
														\n\
		void main()										\n\
		{												\n\
			FragColor = texture(texture1, TexCoord);	\n\
		}";

	const float _quad_vertices[32] = {
			// positions          // colors           // texture coords
			 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
			 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
			-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
			-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
		};

	void draw_texture(const vr::PresentInfo_t * present_info, GLuint shader_program, GLuint VAO);

	bool check_shader_compile_status(GLuint obj);

	bool check_program_link_status(GLuint obj);

};