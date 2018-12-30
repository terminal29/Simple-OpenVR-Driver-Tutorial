#include "FakeHMD.hpp"

FakeHMD::FakeHMD() : 
	_pose({ 0 }) // Zero initialize the pose (mandatory)
{
	// some random but unique serial string
	_serial = "fh_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());

	// Set up some defalt rotation pointing down -z
	_pose.qRotation.w = 1.0;
	_pose.qRotation.x = 0.0;
	_pose.qRotation.y = 0.0;
	_pose.qRotation.z = 0.0;

	_pose.qWorldFromDriverRotation.w = 1.0;
	_pose.qWorldFromDriverRotation.x = 0.0;
	_pose.qWorldFromDriverRotation.y = 0.0;
	_pose.qWorldFromDriverRotation.z = 0.0;

	_pose.qDriverFromHeadRotation.w = 1.0;
	_pose.qDriverFromHeadRotation.x = 0.0;
	_pose.qDriverFromHeadRotation.y = 0.0;
	_pose.qDriverFromHeadRotation.z = 0.0;

	// To ensure no complaints about tracking
	_pose.poseIsValid = true;
	_pose.result = vr::ETrackingResult::TrackingResult_Running_OK;
	_pose.deviceIsConnected = true;
}

std::shared_ptr<FakeHMD> FakeHMD::make_new()
{
	return std::shared_ptr<FakeHMD>(new FakeHMD());
}

std::string FakeHMD::get_serial() const
{
	return _serial;
}

void FakeHMD::update()
{
	// Update time delta (for working out velocity)
	std::chrono::milliseconds time_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	double time_since_epoch_seconds = time_since_epoch.count() / 1000.0;
	double pose_time_delta_seconds = (time_since_epoch - _pose_timestamp).count() / 1000.0;

	// Update pose timestamp
	_pose_timestamp = time_since_epoch;

	// Copy the previous position data
	double previous_position[3] = { 0 };
	std::copy(&_pose.vecPosition[0], &_pose.vecPosition[2], previous_position);

	// Update the position with our new data
	_pose.vecPosition[0] = 2 * std::sin(time_since_epoch_seconds);
	_pose.vecPosition[1] = 1.0;
	_pose.vecPosition[2] = 2 * std::cos(time_since_epoch_seconds);

	// Update the velocity
	_pose.vecVelocity[0] = (_pose.vecPosition[0] - previous_position[0]) / pose_time_delta_seconds;
	_pose.vecVelocity[1] = (_pose.vecPosition[1] - previous_position[1]) / pose_time_delta_seconds;
	_pose.vecVelocity[2] = (_pose.vecPosition[2] - previous_position[2]) / pose_time_delta_seconds;

	// If we are still tracking, update openvr with our new pose data
	if (_index != vr::k_unTrackedDeviceIndexInvalid)
	{
		vr::VRServerDriverHost()->TrackedDevicePoseUpdated(_index, _pose, sizeof(vr::DriverPose_t));
	}
}

vr::TrackedDeviceIndex_t FakeHMD::get_index() const
{
	return _index;
}

void FakeHMD::process_event(const vr::VREvent_t& event)
{
}

vr::EVRInitError FakeHMD::Activate(vr::TrackedDeviceIndex_t index)
{
	// Save the index we are given
	_index = index;

	// Get the properties handle
	_props = vr::VRProperties()->TrackedDeviceToPropertyContainer(_index);

	// Set some universe ID (Must be 2 or higher)
	vr::VRProperties()->SetUint64Property(_props, vr::Prop_CurrentUniverseId_Uint64, 2);
	
	// set the IPD to be whatever steam has configured
	vr::VRProperties()->SetFloatProperty(_props, vr::Prop_UserIpdMeters_Float, vr::VRSettings()->GetFloat(vr::k_pch_SteamVR_Section, vr::k_pch_SteamVR_IPD_Float));

	// Set the display FPS
	vr::VRProperties()->SetFloatProperty(_props, vr::Prop_DisplayFrequency_Float, 90.f);

	// Disasble warnings about compositor not being fullscreen
	vr::VRProperties()->SetBoolProperty(_props, vr::Prop_IsOnDesktop_Bool, true);

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
	// Use our stored properties to return the window bounds
	*x = _display_properties.display_offset_x;
	*y = _display_properties.display_offset_y;
	*width = _display_properties.display_width;
	*height = _display_properties.display_height;
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
	*width = _display_properties.render_width;
	*height = _display_properties.render_height;
}

void FakeHMD::GetEyeOutputViewport(vr::EVREye eye, uint32_t * x, uint32_t * y, uint32_t * width, uint32_t * height)
{
	*y = _display_properties.display_offset_y;
	*width = _display_properties.render_width / 2;
	*height = _display_properties.render_height;

	if (eye == vr::EVREye::Eye_Left) {
		*x = _display_properties.display_offset_x;
	}
	else {
		*x = _display_properties.display_offset_x + _display_properties.render_width/2;
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

vr::DriverPose_t FakeHMD::get_pose() const
{
	return _pose;
}

void FakeHMD::set_pose(vr::DriverPose_t new_pose)
{
	_pose = new_pose;
}

