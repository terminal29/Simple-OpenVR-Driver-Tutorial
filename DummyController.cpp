#include "DummyController.h"

DummyController::DummyController()
{
}

DummyController::DummyController(std::string serial, bool side, DriverPose_t initial_pose, VRControllerState_t initial_state) :
	serial_(serial),
	side_(side),
	controller_pose_(initial_pose),
	controller_state_(initial_state)
{
}

DummyController::~DummyController()
{
}

void DummyController::updateControllerState(VRControllerState_t new_state)
{
	controller_state_ = new_state;
}

void DummyController::updateControllerPose(DriverPose_t new_pose)
{
	controller_pose_ = new_pose;
}

uint32_t DummyController::getObjectID()
{
	return object_id_;
}

VRControllerState_t DummyController::GetControllerState()
{
	return controller_state_;
}

bool DummyController::TriggerHapticPulse(uint32_t unAxisId, uint16_t usPulseDurationMicroseconds)
{
	return false;
}

EVRInitError DummyController::Activate(uint32_t unObjectId)
{
	object_id_ = unObjectId;

	PropertyContainerHandle_t prop_handle = VRProperties()->TrackedDeviceToPropertyContainer(unObjectId);

	VRProperties()->SetBoolProperty(prop_handle, vr::Prop_WillDriftInYaw_Bool, false);
	VRProperties()->SetBoolProperty(prop_handle, vr::Prop_DeviceIsWireless_Bool, true);
	VRProperties()->SetBoolProperty(prop_handle, vr::Prop_HasControllerComponent_Bool, true);

	if (side_) {
		VRProperties()->SetInt32Property(prop_handle, vr::Prop_ControllerRoleHint_Int32, ETrackedControllerRole::TrackedControllerRole_RightHand);
	}else {
		VRProperties()->SetInt32Property(prop_handle, vr::Prop_ControllerRoleHint_Int32, ETrackedControllerRole::TrackedControllerRole_LeftHand);
	}

	VRProperties()->SetInt32Property(prop_handle, vr::Prop_Axis0Type_Int32, vr::k_eControllerAxis_TrackPad);
	VRProperties()->SetInt32Property(prop_handle, vr::Prop_Axis1Type_Int32, vr::k_eControllerAxis_Trigger);

	VRProperties()->SetStringProperty(prop_handle, Prop_SerialNumber_String, serial_.c_str());
	VRProperties()->SetStringProperty(prop_handle, Prop_ModelNumber_String, "Vive Controller MV");
	VRProperties()->SetStringProperty(prop_handle, Prop_RenderModelName_String, "vr_controller_vive_1_5");
	VRProperties()->SetStringProperty(prop_handle, Prop_ManufacturerName_String, "HTC");
	
	uint64_t available_buttons =	vr::ButtonMaskFromId(vr::k_EButton_ApplicationMenu) |
									vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Touchpad) |
									vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Trigger) |
									vr::ButtonMaskFromId(vr::k_EButton_System) |
									vr::ButtonMaskFromId(vr::k_EButton_Grip);

	vr::VRProperties()->SetUint64Property(prop_handle, Prop_SupportedButtons_Uint64, available_buttons);
	
	return EVRInitError::VRInitError_None;
}

void DummyController::Deactivate()
{
}

void DummyController::EnterStandby()
{
}

void * DummyController::GetComponent(const char * pchComponentNameAndVersion)
{
	if (0 == strcmp(IVRControllerComponent_Version, pchComponentNameAndVersion))
	{
		return (vr::IVRControllerComponent*)this;
	}

	return NULL;
}

void DummyController::DebugRequest(const char * pchRequest, char * pchResponseBuffer, uint32_t unResponseBufferSize)
{
	if (unResponseBufferSize >= 1)
		pchResponseBuffer[0] = 0;
}

DriverPose_t DummyController::GetPose()
{
	return controller_pose_;
}
