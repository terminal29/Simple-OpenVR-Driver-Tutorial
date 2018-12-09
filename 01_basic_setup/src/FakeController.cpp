#include "FakeController.hpp"

FakeController::FakeController() 
{
	// Create some random but unique serial
	_serial = "fc_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
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
	_index = index;
	
	// Get the properties handle for our controller, and define the inputs & outputs
	_props = vr::VRProperties()->TrackedDeviceToPropertyContainer(_index);
	vr::VRDriverInput()->CreateBooleanComponent(_props, "/input/system/click", &_components._system_click);
	vr::VRDriverInput()->CreateBooleanComponent(_props, "/input/grip/click", &_components._grip_click);
	vr::VRDriverInput()->CreateBooleanComponent(_props, "/input/application_menu/click", &_components._app_click);
	vr::VRDriverInput()->CreateScalarComponent(_props, "/input/trigger/value", &_components._trigger_value, vr::EVRScalarType::VRScalarType_Absolute, vr::EVRScalarUnits::VRScalarUnits_NormalizedOneSided);
	vr::VRDriverInput()->CreateScalarComponent(_props, "/input/trackpad/x", &_components._trackpad_x, vr::EVRScalarType::VRScalarType_Absolute, vr::EVRScalarUnits::VRScalarUnits_NormalizedTwoSided);
	vr::VRDriverInput()->CreateScalarComponent(_props, "/input/trackpad/y", &_components._trackpad_y, vr::EVRScalarType::VRScalarType_Absolute, vr::EVRScalarUnits::VRScalarUnits_NormalizedTwoSided); 
	vr::VRDriverInput()->CreateBooleanComponent(_props, "/input/trackpad/click", &_components._trackpad_click);
	vr::VRDriverInput()->CreateBooleanComponent(_props, "/input/trackpad/touch", &_components._trackpad_touch);
	vr::VRDriverInput()->CreateHapticComponent(_props, "/output/haptic", &_components._haptic);

	return vr::VRInitError_None;
}

void FakeController::Deactivate()
{
	// Reset device index
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
