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

vr::EVRInitError VirtualCompositor::Activate(vr::TrackedDeviceIndex_t index)
{
	_index = index;
	_compositor_running = true;

	_render_thread = std::thread([&]() {
		// see http://discourse.glfw.org/t/multithreading-glfw/573/5 for more info
		GLFWwindow* _window = nullptr;
		if (glfwInit()) {
			DebugDriverLog("GLFW Init success\n");
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
			_window = glfwCreateWindow(640, 480, _serial.c_str(), nullptr, nullptr);
			if (_window != nullptr) {
				glfwMakeContextCurrent(_window);
				glfwSwapInterval(1);
				DebugDriverLog("Window Opened\n");

				while (_compositor_running) {
					glfwMakeContextCurrent(_window);
					int width, height;
					glfwGetFramebufferSize(_window, &width, &height);
					glViewport(0, 0, width, height);
					glClear(GL_COLOR_BUFFER_BIT);

					if (_render_tasks.size() > 0) {
						_render_task_lock.lock();
						for (auto it = _render_tasks.begin(); it != _render_tasks.end(); ++it) {
							(*it)();
						}
						_render_tasks.clear();
						_render_task_lock.unlock();
					}




					glfwSwapBuffers(_window);
					glfwPollEvents();

				}
				if (_window != nullptr) {
					glfwDestroyWindow(_window);
					_window = nullptr;
					glfwTerminate();
					DebugDriverLog("Window Closed\n");
				}
			}
			else {
				DebugDriverLog("Error opening window\n");
			}
		}
		else {
			DebugDriverLog("GLFW Init fail\n");
		}
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

void VirtualCompositor::DrawTexture(const vr::PresentInfo_t * present_info) {


}

void VirtualCompositor::Present(const vr::PresentInfo_t * present_info, uint32_t present_info_size)
{
	std::promise<bool> result;
	auto render_job = [&] {
		DrawTexture(present_info);
		result.set_value(true);
	};
	std::future<bool> future = result.get_future();
	
	_render_task_lock.lock();
	_render_tasks.push_back(render_job);
	_render_task_lock.unlock();
	future.wait();

}

void VirtualCompositor::WaitForPresent()
{

}

bool VirtualCompositor::GetTimeSinceLastVsync(float * seconds_since_last_vsync, uint64_t * frame_counter)
{

	return false;
}
