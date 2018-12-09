#include "FakeHMD.hpp"

FakeHMD::FakeHMD()
{
	// some random but unique serial string
	_serial = "fh_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
}

std::shared_ptr<FakeHMD> FakeHMD::make_new()
{
	return std::shared_ptr<FakeHMD>(new FakeHMD());
}

std::string FakeHMD::get_serial()
{
	return _serial;
}

void FakeHMD::update()
{
}

vr::TrackedDeviceIndex_t FakeHMD::get_index()
{
	return _index;
}

void FakeHMD::process_event(const vr::VREvent_t& event)
{
}

vr::EVRInitError FakeHMD::Activate(vr::TrackedDeviceIndex_t index)
{
	_index = index;
	return vr::VRInitError_None;
}

void FakeHMD::Deactivate()
{
	// Reset device index
	_index = vr::k_unTrackedDeviceIndexInvalid;
}

void FakeHMD::EnterStandby()
{
}

void * FakeHMD::GetComponent(const char * component)
{
	// This device has a display component, so check if the requested component is the IVRDisplayComponent, and cast and return it
	if (std::string(component) == std::string(vr::IVRDisplayComponent_Version)) {
		return static_cast<vr::IVRDisplayComponent*>(this);
	}
	return nullptr;
}

void FakeHMD::DebugRequest(const char * request, char * response_buffer, uint32_t response_buffer_size)
{
	if (response_buffer_size >= 1)
		response_buffer[0] = 0;
}

vr::DriverPose_t FakeHMD::GetPose()
{
	return _pose;
}

void FakeHMD::GetWindowBounds(int32_t * x, int32_t * y, uint32_t * width, uint32_t * height)
{
	*x = 0;
	*y = 0;
	*width = 1920;
	*height = 1080;
}

bool FakeHMD::IsDisplayOnDesktop()
{
	return true;
}

bool FakeHMD::IsDisplayRealDisplay()
{
	return false;
}

void FakeHMD::GetRecommendedRenderTargetSize(uint32_t * width, uint32_t * height)
{
	// Change these to whatever your desired viewport size is
	*width = 1920;
	*height = 1080;
}

void FakeHMD::GetEyeOutputViewport(vr::EVREye eye, uint32_t * x, uint32_t * y, uint32_t * width, uint32_t * height)
{
	if (eye == vr::EVREye::Eye_Left) {
		*x = 0;
		*y = 0;
		*width = 1920/2;
		*height = 1080;
	}
	else {
		*x = 1920/2;
		*y = 0;
		*width = 1920/2;
		*height = 1080;
	}
}

void FakeHMD::GetProjectionRaw(vr::EVREye eye, float * left, float * right, float * top, float * bottom)
{
	*left = -1;
	*right = 1;
	*top = -1;
	*bottom = 1;
}

vr::DistortionCoordinates_t FakeHMD::ComputeDistortion(vr::EVREye eye, float u, float v)
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

