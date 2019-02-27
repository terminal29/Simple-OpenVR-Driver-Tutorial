#include "VirtualCompositor.hpp"

VirtualCompositor::VirtualCompositor() {
	_serial = "vc_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
	
	

}

void VirtualCompositor::GetWindowBounds(int32_t * x, int32_t * y, uint32_t * width, uint32_t * height)
{
	*x = _display_properties.display_offset_x;
	*y = _display_properties.display_offset_y;
	*width = _display_properties.display_width;
	*height = _display_properties.display_height;
}

bool VirtualCompositor::IsDisplayOnDesktop()
{
	return false;
}

bool VirtualCompositor::IsDisplayRealDisplay()
{
	return true;
}

void VirtualCompositor::GetRecommendedRenderTargetSize(uint32_t * width, uint32_t * height)
{// Use the stored display properties to return the render target size
	*width = _display_properties.render_width;
	*height = _display_properties.render_height;
}

void VirtualCompositor::GetEyeOutputViewport(vr::EVREye eye, uint32_t * x, uint32_t * y, uint32_t * width, uint32_t * height)
{
	*y = _display_properties.display_offset_y;
	*width = _display_properties.render_width / 2;
	*height = _display_properties.render_height;

	if (eye == vr::EVREye::Eye_Left) {
		*x = _display_properties.display_offset_x;
	}
	else {
		*x = _display_properties.display_offset_x + _display_properties.render_width / 2;
	}
}

void VirtualCompositor::GetProjectionRaw(vr::EVREye eEye, float * left, float * right, float * top, float * bottom)
{
	*left = -1;
	*right = 1;
	*top = -1;
	*bottom = 1;
}

vr::DistortionCoordinates_t VirtualCompositor::ComputeDistortion(vr::EVREye eEye, float u, float v)
{
	vr::DistortionCoordinates_t coordinates;
	coordinates.rfBlue[0] = u;
	coordinates.rfBlue[1] = v;
	coordinates.rfGreen[0] = u;
	coordinates.rfGreen[1] = v;
	coordinates.rfRed[0] = u;
	coordinates.rfRed[1] = v;
	return coordinates;
}

VirtualCompositor::~VirtualCompositor() {
	
}

std::shared_ptr<VirtualCompositor> VirtualCompositor::make_new()
{
	return std::shared_ptr<VirtualCompositor>(new VirtualCompositor());
}

std::string VirtualCompositor::get_serial() const
{
	return _serial;
}

void VirtualCompositor::update()
{


}

vr::TrackedDeviceIndex_t VirtualCompositor::get_index() const
{
	return _index;
}

void VirtualCompositor::process_event(const vr::VREvent_t & event)
{
}

void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	DebugDriverLog("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}

// https://github.com/progschj/OpenGL-Examples/blob/master/01shader_vbo1.cpp
bool check_shader_compile_status(GLuint obj) {
	GLint status;
	glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		GLint length;
		glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &length);
		std::vector<char> log(length);
		glGetShaderInfoLog(obj, length, &length, &log[0]);
		DebugDriverLog(&log[0]);
		//std::cerr << &log[0];
		return false;
	}
	return true;
}

bool check_program_link_status(GLuint obj) {
	GLint status;
	glGetProgramiv(obj, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint length;
		glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &length);
		std::vector<char> log(length);
		glGetProgramInfoLog(obj, length, &length, &log[0]);
		DebugDriverLog(&log[0]);
		//std::cerr << &log[0];
		return false;
	}
	return true;
}

vr::EVRInitError VirtualCompositor::Activate(vr::TrackedDeviceIndex_t index)
{
	_index = index;
	_compositor_running = true;

	_render_thread = std::thread([&]() {
		// see http://discourse.glfw.org/t/multithreading-glfw/573/5 for more info
		// Init GLFW
		GLFWwindow* _window = nullptr;
		if (!glfwInit()) {
			DebugDriverLog("GLFW init fail\n");
			return;
		}
		DebugDriverLog("GLFW init success\n");

		// Open Window
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		//glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

		_window = glfwCreateWindow(640, 480, _serial.c_str(), nullptr, nullptr);
		if (_window == nullptr) {
			DebugDriverLog("Error opening window\n");
			return;
		}

		glfwSetWindowSize(_window, _display_properties.display_width/2, _display_properties.display_height/2);
		glfwSetWindowPos(_window, _display_properties.display_width / 4, _display_properties.display_height / 4);
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
			glClearColor(1, 1, 0, 0);

			unsigned int VBO;
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);

			unsigned int VAO;
			glGenVertexArrays(1, &VAO);
			glBindVertexArray(VAO);


			float vertices[] = {
				// positions          // colors           // texture coords
				 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
				 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
				-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
				-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
			};

			// 2. copy our vertices array in a buffer for OpenGL to use
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
			glEnableVertexAttribArray(2);


			const char* vertex_shader =
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

			const char* fragment_shader =

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

			GLuint vs = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vs, 1, &vertex_shader, NULL);
			glCompileShader(vs);

			GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fs, 1, &fragment_shader, NULL);
			glCompileShader(fs);

			GLuint shader_programme = glCreateProgram();
			glAttachShader(shader_programme, fs);
			glAttachShader(shader_programme, vs);
			glLinkProgram(shader_programme);

			check_shader_compile_status(vs);
			check_shader_compile_status(fs);
			check_program_link_status(shader_programme);

			// Do render loop
			while (_compositor_running) {
				glfwMakeContextCurrent(_window);
				int width, height;

				glfwGetFramebufferSize(_window, &width, &height);
				glViewport(0, 0, width, height);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				if (_render_tasks.size() > 0) {
					std::lock_guard<std::mutex> task_guard(_render_task_lock);
					for (auto render_task : _render_tasks) {
						render_task(shader_programme, VAO);
					}
					_render_tasks.clear();
				}

				glfwSwapBuffers(_window);
				glfwPollEvents();

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

	// Get the properties handle
	_props = vr::VRProperties()->TrackedDeviceToPropertyContainer(_index);

	// Set some universe ID (Must be 2 or higher)
	vr::VRProperties()->SetUint64Property(_props, vr::Prop_CurrentUniverseId_Uint64, 2);

	// Set the IPD to be whatever steam has configured
	vr::VRProperties()->SetFloatProperty(_props, vr::Prop_UserIpdMeters_Float, vr::VRSettings()->GetFloat(vr::k_pch_SteamVR_Section, vr::k_pch_SteamVR_IPD_Float));

	// Set the display FPS
	vr::VRProperties()->SetFloatProperty(_props, vr::Prop_DisplayFrequency_Float, 90.f);

	// Disable warnings about compositor not being fullscreen
	vr::VRProperties()->SetBoolProperty(_props, vr::Prop_IsOnDesktop_Bool, true);
	
	return vr::VRInitError_None;
}

void VirtualCompositor::Deactivate()
{
	_compositor_running = false;
	_render_thread.join();
	// Clear device id
	_index = vr::k_unTrackedDeviceIndexInvalid;
}

void VirtualCompositor::EnterStandby()
{
}

void * VirtualCompositor::GetComponent(const char * component)
{
	if (std::string(component) == std::string(vr::IVRVirtualDisplay_Version)){
		return static_cast<vr::IVRVirtualDisplay*>(this);
	}
	if (std::string(component) == std::string(vr::IVRDisplayComponent_Version)){
		return static_cast<vr::IVRDisplayComponent*>(this);
	}
	return nullptr;
}

void VirtualCompositor::DebugRequest(const char * request, char * response_buffer, uint32_t response_buffer_size)
{
	// No custom debug requests defined
	if (response_buffer_size >= 1)
		response_buffer[0] = 0;
}

vr::DriverPose_t VirtualCompositor::GetPose()
{
	vr::DriverPose_t pose = { 0 };
	pose.poseIsValid = true;
	pose.result = vr::TrackingResult_Running_OK;
	pose.deviceIsConnected = true;
	pose.qWorldFromDriverRotation.w = 1;
	pose.qWorldFromDriverRotation.x = 0;
	pose.qWorldFromDriverRotation.y = 0;
	pose.qWorldFromDriverRotation.z = 0;
	pose.qDriverFromHeadRotation.w = 1;
	pose.qDriverFromHeadRotation.x = 0;
	pose.qDriverFromHeadRotation.y = 0;
	pose.qDriverFromHeadRotation.z = 0;
	return pose;
}

void VirtualCompositor::DrawTexture(const vr::PresentInfo_t * present_info, GLuint shader_program, GLuint VAO) {


	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	
	
}

void VirtualCompositor::Present(const vr::PresentInfo_t * present_info, uint32_t present_info_size)
{
	std::promise<bool> result;

	static auto render_job = [&](GLuint shader_program, GLuint VAO) {
		
		DrawTexture(present_info, shader_program, VAO);
		result.set_value(true);
	};

	std::future<bool> future = result.get_future();
	{
		std::lock_guard<std::mutex> task_guard(_render_task_lock);
		_render_tasks.push_back(render_job);
	}
	future.wait();
	
}

void VirtualCompositor::WaitForPresent()
{
}

bool VirtualCompositor::GetTimeSinceLastVsync(float * seconds_since_last_vsync, uint64_t * frame_counter)
{

	return false;
}
