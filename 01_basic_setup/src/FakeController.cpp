#include "FakeController.hpp"

FakeController::FakeController() : 
	_pose( {0} )
{
	// Create some random but unique serial
	_serial = "fc_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());

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

std::shared_ptr<FakeController> FakeController::make_new()
{
	return std::shared_ptr<FakeController>(new FakeController());
}

std::string FakeController::get_serial() const
{
	return _serial;
}

void FakeController::update()
{
	// Update time delta (for working out velocity)
	std::chrono::milliseconds time_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	double time_since_epoch_seconds = time_since_epoch.count() / 1000.0;
	double pose_time_delta_seconds = (time_since_epoch - _pose_timestamp).count() / 1000.0;

	// Update pose timestamp
	_pose_timestamp = time_since_epoch;

	// Copy the previous position data
	double previous_position[3] = { 0 };
	std::copy(std::begin(_pose.vecPosition), std::end(_pose.vecPosition), std::begin(previous_position));

	// Update the position with our new data
	_pose.vecPosition[0] = 2 * std::sin(time_since_epoch_seconds);
	_pose.vecPosition[1] = 1.0;
	_pose.vecPosition[2] = -1 + 2 * std::cos(time_since_epoch_seconds);

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

vr::TrackedDeviceIndex_t FakeController::get_index() const
{
	return _index;
}

void FakeController::process_event(const vr::VREvent_t& event)
{
}

vr::EVRInitError FakeController::Activate(vr::TrackedDeviceIndex_t index)
{
	// Save the device index
	_index = index;
	
	// Get the properties handle for our controller
	_props = vr::VRProperties()->TrackedDeviceToPropertyContainer(_index);

	// Set our universe ID
	vr::VRProperties()->SetUint64Property(_props, vr::Prop_CurrentUniverseId_Uint64, 2);

	// Add our controller components. (These are the same as the regular vive controller)
	vr::VRDriverInput()->CreateBooleanComponent(_props, "/input/system/click", &_components._system_click);
	vr::VRDriverInput()->CreateBooleanComponent(_props, "/input/grip/click", &_components._grip_click);
	vr::VRDriverInput()->CreateBooleanComponent(_props, "/input/application_menu/click", &_components._app_click);
	vr::VRDriverInput()->CreateScalarComponent(_props, "/input/trigger/value", &_components._trigger_value, vr::EVRScalarType::VRScalarType_Absolute, vr::EVRScalarUnits::VRScalarUnits_NormalizedOneSided);
	vr::VRDriverInput()->CreateScalarComponent(_props, "/input/trackpad/x", &_components._trackpad_x, vr::EVRScalarType::VRScalarType_Absolute, vr::EVRScalarUnits::VRScalarUnits_NormalizedTwoSided);
	vr::VRDriverInput()->CreateScalarComponent(_props, "/input/trackpad/y", &_components._trackpad_y, vr::EVRScalarType::VRScalarType_Absolute, vr::EVRScalarUnits::VRScalarUnits_NormalizedTwoSided); 
	vr::VRDriverInput()->CreateBooleanComponent(_props, "/input/trackpad/click", &_components._trackpad_click);
	vr::VRDriverInput()->CreateBooleanComponent(_props, "/input/trackpad/touch", &_components._trackpad_touch);
	vr::VRDriverInput()->CreateHapticComponent(_props, "/output/haptic", &_components._haptic);

	// Set our controller to use the vive controller render model
	vr::VRProperties()->SetStringProperty(_props, vr::Prop_RenderModelName_String, "vr_controller_vive_1_5");

	return vr::VRInitError_None;
}

void FakeController::Deactivate()
{
	// Clear device id
	_index = vr::k_unTrackedDeviceIndexInvalid;
}

void FakeController::EnterStandby()
{
}

void * FakeController::GetComponent(const char * component)
{
	// No extra components on this device so always return nullptr
	return nullptr;
}

void FakeController::DebugRequest(const char * request, char* response_buffer, uint32_t response_buffer_size)
{
	// No custom debug requests defined
	if (response_buffer_size >= 1)
		response_buffer[0] = 0;
}

vr::DriverPose_t FakeController::GetPose()
{
	return _pose;
}

void FakeController::set_pose(vr::DriverPose_t new_pose)
{
	_pose = new_pose;
}
