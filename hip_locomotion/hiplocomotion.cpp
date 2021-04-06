#include "fakemove.h"

const int BUFSIZE = 1024;

TCHAR chReadBuf[BUFSIZE];
BOOL fSuccess;
DWORD cbRead;
LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\ApriltagPipeIn");

int trackernum = 4;

void check_error(int line, vr::EVRInitError error) { if (error != 0) printf("%d: error %s\n", line, VR_GetVRInitErrorAsSymbol(error)); }

int main()
{
	vr::EVRInitError error;
	VR_Init(&error, vr::VRApplication_Overlay);
	check_error(__LINE__, error);

	std::istringstream ret;
	std::string word;

	ret = Send(TEXT("addfakemove"));
	ret >> word;
	if (word != "added")
	{
		std::cout << "Wrong message received!" << std::endl;
	}

	vr::VROverlayHandle_t handle;
	vr::VROverlay()->CreateOverlay("image", "image", &handle); /* key has to be unique, name doesn't matter */
	vr::VROverlay()->SetOverlayFromFile(handle, "deca.jpg");
	vr::VROverlay()->SetOverlayWidthInMeters(handle, 3);
	vr::VROverlay()->ShowOverlay(handle);

	vr::HmdMatrix34_t transform = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, -2.0f
	};
	vr::VROverlay()->SetOverlayTransformAbsolute(handle, vr::TrackingUniverseStanding, &transform);

	vr::VRActionHandle_t m_actionAnalongInput = vr::k_ulInvalidActionHandle;
	vr::VRActionHandle_t m_actionHideCubes = vr::k_ulInvalidActionHandle;
	vr::VRActionHandle_t m_actionsetDemo = vr::k_ulInvalidActionHandle;
	vr::VRActionHandle_t m_actionPose = vr::k_ulInvalidActionHandle;

	DWORD  retval = 0;
	BOOL   success;
	TCHAR  buffer[BUFSIZE] = TEXT("");
	TCHAR  buf[BUFSIZE] = TEXT("");
	TCHAR** lppPart = { NULL };

	retval = GetFullPathName("fakemove_actions.json",
		BUFSIZE,
		buffer,
		lppPart);

	if (retval == 0)
	{
		// Handle an error condition.
		printf("GetFullPathName failed (%d)\n", GetLastError());
		return -1;
	}
	else
	{
		std::cout << "The full path name is: " <<  buffer << std::endl;
		if (lppPart != NULL && *lppPart != 0)
		{
			std::cout << "The final component in the path name is: " <<  *lppPart <<std::endl;
		}
	}


	vr::VRInput()->SetActionManifestPath(buffer);

	vr::VRInput()->GetActionHandle("/actions/demo/in/AnalogInput", &m_actionAnalongInput);
	vr::VRInput()->GetActionHandle("/actions/demo/in/HideCubes", &m_actionHideCubes);
	vr::VRInput()->GetActionHandle("/actions/demo/in/Hand_Left", &m_actionPose);

	vr::VRInput()->GetActionSetHandle("/actions/demo", &m_actionsetDemo);

	vr::TrackedDevicePose_t pTrackedDevicePose[10];

	vr::VRSystem()->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding,0, pTrackedDevicePose,10);

	float controllerRotation = 0;
	float hmdRotation = 0;

	while (true) {
		vr::VRSystem()->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0, pTrackedDevicePose, 10);

		vr::VRActiveActionSet_t actionSet = { 0 };
		actionSet.ulActionSet = m_actionsetDemo;
		vr::VRInput()->UpdateActionState(&actionSet, sizeof(actionSet), 1);

		
		//std::cout << pTrackedDevicePose[0].mDeviceToAbsoluteTracking.m[0][3] << " " << pTrackedDevicePose[0].mDeviceToAbsoluteTracking.m[1][3] << " " << pTrackedDevicePose[0].mDeviceToAbsoluteTracking.m[2][3] << std::endl;		
		hmdRotation = atan2(pTrackedDevicePose[0].mDeviceToAbsoluteTracking.m[0][2], pTrackedDevicePose[0].mDeviceToAbsoluteTracking.m[2][2]);

		vr::InputAnalogActionData_t analogData;
		if (vr::VRInput()->GetAnalogActionData(m_actionAnalongInput, &analogData, sizeof(analogData), vr::k_ulInvalidInputValueHandle) == vr::VRInputError_None && analogData.bActive)
		{
			//std::cout << "x: " << analogData.x << " y: " << analogData.y << std::endl;
			//SendMove(analogData.x, analogData.y);
		}
		else
			std::cout << "error getting data " << analogData.bActive <<  std::endl;
		
		//std::cout << GetDigitalActionState(m_actionHideCubes) << std::endl;

		vr::InputPoseActionData_t poseData;

		//std::cout << "error:" <<vr::VRInput()->GetPoseActionDataForNextFrame(m_actionPose, vr::TrackingUniverseStanding, &poseData, sizeof(poseData), vr::k_ulInvalidInputValueHandle) << std::endl;
		if (vr::VRInput()->GetPoseActionDataForNextFrame(m_actionPose, vr::TrackingUniverseStanding, &poseData, sizeof(poseData), vr::k_ulInvalidInputValueHandle) == vr::VRInputError_None)
		{
			//std::cout <<  "controller pose: " << poseData.pose.mDeviceToAbsoluteTracking.m[0][3] << " " << poseData.pose.mDeviceToAbsoluteTracking.m[1][3] << " " << poseData.pose.mDeviceToAbsoluteTracking.m[2][3] << std::endl;

			controllerRotation = atan2(poseData.pose.mDeviceToAbsoluteTracking.m[0][2], poseData.pose.mDeviceToAbsoluteTracking.m[2][2]);
		}

		std::cout << "rotation fix: " << controllerRotation - hmdRotation << std::endl;

		float magnitude = sqrt(analogData.x * analogData.x + analogData.y * analogData.y);
		float angle = atan2(analogData.x, analogData.y);

		float newDataX = sin(angle - (controllerRotation - hmdRotation)) * magnitude;
		float newDataY = cos(angle - (controllerRotation - hmdRotation)) * magnitude;

		std::cout << "Received data: " << analogData.x << "," << analogData.y << " Calculated data: " << newDataX << "," << newDataY << std::endl;

		SendMove(newDataX, newDataY);

		Sleep(10);

	}
	return 0;
}

bool GetDigitalActionState(vr::VRActionHandle_t action, vr::VRInputValueHandle_t* pDevicePath)
{
	vr::InputDigitalActionData_t actionData;
	vr::VRInput()->GetDigitalActionData(action, &actionData, sizeof(actionData), vr::k_ulInvalidInputValueHandle);
	if (pDevicePath)
	{
		*pDevicePath = vr::k_ulInvalidInputValueHandle;
		if (actionData.bActive)
		{
			vr::InputOriginInfo_t originInfo;
			if (vr::VRInputError_None == vr::VRInput()->GetOriginTrackedDeviceInfo(actionData.activeOrigin, &originInfo, sizeof(originInfo)))
			{
				*pDevicePath = originInfo.devicePath;
			}
		}
	}
	return actionData.bActive && actionData.bState;
}

std::istringstream Send(LPTSTR lpszWrite)
{
	fSuccess = CallNamedPipe(
		lpszPipename,        // pipe name 
		lpszWrite,           // message to server 
		(lstrlen(lpszWrite) + 1) * sizeof(TCHAR), // message length 
		chReadBuf,              // buffer to receive reply 
		BUFSIZE * sizeof(TCHAR),  // size of read buffer 
		&cbRead,                // number of bytes read 
		2000);                 // waits for 2 seconds 

	if (fSuccess || GetLastError() == ERROR_MORE_DATA)
	{
		std::cout << chReadBuf << std::endl;
		chReadBuf[cbRead] = '\0'; //add terminating zero
					//convert our buffer to string
		std::string rec = chReadBuf;
		std::istringstream iss(rec);
		// The pipe is closed; no more data can be read. 

		if (!fSuccess)
		{
			printf("\nExtra data in message was lost\n");
		}
		return iss;
	}
	else
	{
		std::cout << GetLastError() << " :(" << std::endl;
		std::string rec = " senderror";
		std::istringstream iss(rec);
		return iss;
	}
}

std::istringstream SendMove(double x, double y)
{
	std::string s;
	s = " fakemoveinput " + std::to_string(x) +
		" " + std::to_string(y)  + "\n";

	//send the string to our driver

	LPTSTR sendstring = (LPTSTR)s.c_str();

	return Send(sendstring);
}