#include "VRDriver.hpp"
#include <Driver/HMDDevice.hpp>
#include <Driver/TrackerDevice.hpp>
#include <Driver/ControllerDevice.hpp>
#include <Driver/TrackingReferenceDevice.hpp>

vr::EVRInitError ExampleDriver::VRDriver::Init(vr::IVRDriverContext* pDriverContext)
{
    // Perform driver context initialisation
    if (vr::EVRInitError init_error = vr::InitServerDriverContext(pDriverContext); init_error != vr::EVRInitError::VRInitError_None) {
        return init_error;
    }

    Log("Activating AprilTag Driver Bridge " + version + "...");

    // Add a HMD
    //this->AddDevice(std::make_shared<HMDDevice>("Example_HMDDevice"));

    // Add a couple controllers
    //this->AddDevice(std::make_shared<ControllerDevice>("Example_ControllerDevice", ControllerDevice::Handedness::ANY));
    //this->AddDevice(std::make_shared<ControllerDevice>("Example_ControllerDevice_Right", ControllerDevice::Handedness::RIGHT));
    
    ipcServer.init("ApriltagPipeIn");

    std::thread pipeThread(&ExampleDriver::VRDriver::PipeThread, this);
    pipeThread.detach();
  
    // Add a couple tracking references
    //this->AddDevice(std::make_shared<TrackingReferenceDevice>("Example_TrackingReference_A"));
    //this->AddDevice(std::make_shared<TrackingReferenceDevice>("Example_TrackingReference_B"));

    Log("AprilTag Driver Loaded Successfully");

	return vr::VRInitError_None;
}

void ExampleDriver::VRDriver::Cleanup()
{
}

void ExampleDriver::VRDriver::PipeThread()
{
    char buffer[1024];

    for (;;) 
    {
        Ipc::Connection ipcConnection = ipcServer.accept();
        //MessageBoxA(NULL, "connected", "Example Driver", MB_OK);

        //we go and read it into our buffer
        if (ipcConnection.recv(buffer, sizeof(buffer)))
        {
            //MessageBoxA(NULL, "connected2", "Example Driver", MB_OK);
            //convert our buffer to string

            //MessageBoxA(NULL, buffer, "Example Driver", MB_OK);

            std::string rec = buffer;

            //Log("Received message: " + rec);

            std::istringstream iss(rec);
            std::string word;

            std::string s = "";

            while (iss >> word)
            {
                if (word == "addhipmove")
                {
                    if (fakemove_ != nullptr)
                    {
                        s = s + " alreadyadded";
                    }
                    else
                    {
                        fakemove_ = std::make_shared<ControllerDevice>("Example_ControllerDevice", ControllerDevice::Handedness::ANY);
                        this->AddDevice(fakemove_);

                        s = s + " added";
                    }
                }
                else if (word == "hipmoveinput")
                {
                    if (fakemove_ == nullptr)
                    {
                        s = s + " notspawned";
                    }
                    else
                    {
                        float x, y, rx, ry, a, b;
                        iss >> x; iss >> y; iss >> rx; iss >> ry; iss >> a; iss >> b;

                        fakemove_->SetDirection(x, y, rx, ry, a, b);

                        s = s + " updated";
                    }
                }
                else if (word == "addtracker")
                {
                    //MessageBoxA(NULL, word.c_str(), "Example Driver", MB_OK);
                    std::string name, role;

                    iss >> name;
                    iss >> role;

                    if (name == "")
                    {
                        name = "UnnamedTracker" + std::to_string(this->trackers_.size());
                        role = "TrackerRole_Waist";        //should be "vive_tracker_left_foot" or "vive_tracker_right_foot" or "vive_tracker_waist"
                    }

                    auto addtracker = std::make_shared<TrackerDevice>(name, role);
                    this->AddDevice(addtracker);
                    this->trackers_.push_back(addtracker);
                    addtracker->reinit(tracker_max_saved, tracker_max_time, tracker_smoothing);
                    s = s + " added";
                }
                else if (word == "addstation")
                {
                    auto addstation = std::make_shared<TrackingReferenceDevice>("AprilCamera" + std::to_string(this->devices_.size()));
                    this->AddDevice(addstation);
                    this->stations_.push_back(addstation);
                    s = s + " added";
                }
                else if (word == "updatestation")
                {
                    int idx;
                    double a, b, c, qw, qx, qy, qz;
                    iss >> idx; iss >> a; iss >> b; iss >> c; iss >> qw; iss >> qx; iss >> qy; iss >> qz;

                    if (idx < this->stations_.size())
                    {
                        this->stations_[idx]->UpdatePose(a, b, c, qw, qx, qy, qz);
                        s = s + " updated";
                    }
                    else
                    {
                        s = s + " idinvalid";
                    }

                }
                else if (word == "synctime")
                {
                    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
                    s = s + " " + std::to_string(this->frame_timing_avg_);
                    s = s + " " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(now - this->last_frame_time_).count());
                }
                else if (word == "updatepose")
                {
                    int idx;
                    double a, b, c, qw, qx, qy, qz, time, smoothing;
                    iss >> idx; iss >> a; iss >> b; iss >> c; iss >> qw; iss >> qx; iss >> qy; iss >> qz; iss >> time; iss >> smoothing;

                    if (idx < this->trackers_.size())
                    {
                        if(time < 0)
                            time = -time;
                        this->trackers_[idx]->save_current_pose(a, b, c, qw, qx, qy, qz, time);
                        //this->trackers_[idx]->UpdatePos(a, b, c, time, 1-smoothing);
                        //this->trackers_[idx]->UpdateRot(qw, qx, qy, qz, time, 1-smoothing);

                        //this->trackers_[idx]->Update();
                        s = s + " updated";
                    }
                    else
                    {
                        s = s + " idinvalid";
                    }

                }
                /*                                      no longer supported by new smoothing
                else if (word == "updatepos")
                {
                    int idx;
                    double a, b, c, time, smoothing;
                    iss >> idx; iss >> a; iss >> b; iss >> c; iss >> time; iss >> smoothing;

                    if (idx < this->devices_.size())
                    {
                        this->trackers_[idx]->UpdatePos(a, b, c, time, smoothing);
                        this->trackers_[idx]->Update();
                        s = s + " updated";
                    }
                    else
                    {
                        s = s + " idinvalid";
                    }

                }
                else if (word == "updaterot")
                {
                    int idx;
                    double qw, qx, qy, qz, time, smoothing;
                    iss >> qw; iss >> qx; iss >> qy; iss >> qz; iss >> time; iss >> smoothing;

                    if (idx < this->devices_.size())
                    {
                        this->trackers_[idx]->UpdateRot(qw, qx, qy, qz, time, smoothing);
                        this->trackers_[idx]->Update();
                        s = s + " updated";
                    }
                    else
                    {
                        s = s + " idinvalid";
                    }

                }*/
                else if (word == "getdevicepose")
                {
                    int idx;
                    iss >> idx;

                    vr::TrackedDevicePose_t hmd_pose[10];
                    vr::VRServerDriverHost()->GetRawTrackedDevicePoses(1, hmd_pose, 10);

                    vr::HmdQuaternion_t q = GetRotation(hmd_pose[idx].mDeviceToAbsoluteTracking);
                    vr::HmdVector3_t pos = GetPosition(hmd_pose[idx].mDeviceToAbsoluteTracking);

                    s = s + " devicepose " + std::to_string(idx);
                    s = s + " " + std::to_string(pos.v[0]) +
                        " " + std::to_string(pos.v[1]) +
                        " " + std::to_string(pos.v[2]) +
                        " " + std::to_string(q.w) +
                        " " + std::to_string(q.x) +
                        " " + std::to_string(q.y) +
                        " " + std::to_string(q.z);
                }
                else if (word == "gettrackerpose")
                {
                    int idx;
                    double time_offset;
                    iss >> idx;
                    iss >> time_offset;

                    if (idx < this->devices_.size())
                    {
                        s = s + " trackerpose " + std::to_string(idx);

                        double pose[7];
                        int statuscode = this->trackers_[idx]->get_next_pose(time_offset, pose);

                        s = s + " " + std::to_string(pose[0]) +
                            " " + std::to_string(pose[1]) +
                            " " + std::to_string(pose[2]) +
                            " " + std::to_string(pose[3]) +
                            " " + std::to_string(pose[4]) +
                            " " + std::to_string(pose[5]) +
                            " " + std::to_string(pose[6]) +
                            " " + std::to_string(statuscode);
                    }
                    else
                    {
                        s = s + " idinvalid";
                    }

                }
                else if (word == "numtrackers")
                {
                    s = s + " numtrackers " + std::to_string(this->trackers_.size()) + " " + version;
                }
                else if (word == "settings")
                {
                    int msaved;
                    double mtime;
                    double msmooth;
                    iss >> msaved;
                    iss >> mtime;
                    iss >> msmooth;
                    
                    for (auto& device : this->trackers_)
                        device->reinit(msaved,mtime,msmooth);

                    tracker_max_saved = msaved;
                    tracker_max_time = mtime;
                    tracker_smoothing = msmooth;

                    s = s + "  changed";
                }
                else
                {
                    s = s + "  unrecognized";
                }
            }

            s = s + "  OK\0";

            // = length of string + terminating '\0' !!!
            ipcConnection.send(s.c_str(), (s.length() + 1));
        }
        /*
        }
        else
        {
            Sleep(1);
        }
        */
    }
}

void ExampleDriver::VRDriver::RunFrame()
{
    //MessageBox(NULL,"hi", "Example Driver", MB_OK);
    // Collect events
    vr::VREvent_t event;
    std::vector<vr::VREvent_t> events;
    while (vr::VRServerDriverHost()->PollNextEvent(&event, sizeof(event)))
    {
        events.push_back(event);
    }
    this->openvr_events_ = events;

    // Update frame timing
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    this->frame_timing_ = std::chrono::duration_cast<std::chrono::milliseconds>(now - this->last_frame_time_);
    this->last_frame_time_ = now;

    this->frame_timing_avg_ = this->frame_timing_avg_ * 0.9 + ((double)this->frame_timing_.count()) * 0.1;
    //MessageBox(NULL, std::to_string(((double)this->frame_timing_.count()) * 0.1).c_str(), "Example Driver", MB_OK);

    for (auto& device : this->trackers_)
        device->Update();

}

bool ExampleDriver::VRDriver::ShouldBlockStandbyMode()
{
    return false;
}

void ExampleDriver::VRDriver::EnterStandby()
{
}

void ExampleDriver::VRDriver::LeaveStandby()
{
}

std::vector<std::shared_ptr<ExampleDriver::IVRDevice>> ExampleDriver::VRDriver::GetDevices()
{
    return this->devices_;
}

std::vector<vr::VREvent_t> ExampleDriver::VRDriver::GetOpenVREvents()
{
    return this->openvr_events_;
}

std::chrono::milliseconds ExampleDriver::VRDriver::GetLastFrameTime()
{
    return this->frame_timing_;
}

bool ExampleDriver::VRDriver::AddDevice(std::shared_ptr<IVRDevice> device)
{
    vr::ETrackedDeviceClass openvr_device_class;
    // Remember to update this switch when new device types are added
    switch (device->GetDeviceType()) {
        case DeviceType::CONTROLLER:
            openvr_device_class = vr::ETrackedDeviceClass::TrackedDeviceClass_Controller;
            break;
        case DeviceType::HMD:
            openvr_device_class = vr::ETrackedDeviceClass::TrackedDeviceClass_HMD;
            break;
        case DeviceType::TRACKER:
            openvr_device_class = vr::ETrackedDeviceClass::TrackedDeviceClass_GenericTracker;
            break;
        case DeviceType::TRACKING_REFERENCE:
            openvr_device_class = vr::ETrackedDeviceClass::TrackedDeviceClass_TrackingReference;
            break;
        default:
            return false;
    }
    bool result = vr::VRServerDriverHost()->TrackedDeviceAdded(device->GetSerial().c_str(), openvr_device_class, device.get());
    if(result)
        this->devices_.push_back(device);
    return result;
}

ExampleDriver::SettingsValue ExampleDriver::VRDriver::GetSettingsValue(std::string key)
{
    vr::EVRSettingsError err = vr::EVRSettingsError::VRSettingsError_None;
    int int_value = vr::VRSettings()->GetInt32(settings_key_.c_str(), key.c_str(), &err);
    if (err == vr::EVRSettingsError::VRSettingsError_None) {
        return int_value;
    }
    err = vr::EVRSettingsError::VRSettingsError_None;
    float float_value = vr::VRSettings()->GetFloat(settings_key_.c_str(), key.c_str(), &err);
    if (err == vr::EVRSettingsError::VRSettingsError_None) {
        return float_value;
    }
    err = vr::EVRSettingsError::VRSettingsError_None;
    bool bool_value = vr::VRSettings()->GetBool(settings_key_.c_str(), key.c_str(), &err);
    if (err == vr::EVRSettingsError::VRSettingsError_None) {
        return bool_value;
    }
    std::string str_value;
    str_value.reserve(1024);
    vr::VRSettings()->GetString(settings_key_.c_str(), key.c_str(), str_value.data(), 1024, &err);
    if (err == vr::EVRSettingsError::VRSettingsError_None) {
        return str_value;
    }
    err = vr::EVRSettingsError::VRSettingsError_None;

    return SettingsValue();
}

void ExampleDriver::VRDriver::Log(std::string message)
{
    std::string message_endl = message + "\n";
    vr::VRDriverLog()->Log(message_endl.c_str());
}

vr::IVRDriverInput* ExampleDriver::VRDriver::GetInput()
{
    return vr::VRDriverInput();
}

vr::CVRPropertyHelpers* ExampleDriver::VRDriver::GetProperties()
{
    return vr::VRProperties();
}

vr::IVRServerDriverHost* ExampleDriver::VRDriver::GetDriverHost()
{
    return vr::VRServerDriverHost();
}

//-----------------------------------------------------------------------------
// Purpose: Calculates quaternion (qw,qx,qy,qz) representing the rotation
// from: https://github.com/Omnifinity/OpenVR-Tracking-Example/blob/master/HTC%20Lighthouse%20Tracking%20Example/LighthouseTracking.cpp
//-----------------------------------------------------------------------------

vr::HmdQuaternion_t ExampleDriver::VRDriver::GetRotation(vr::HmdMatrix34_t matrix) {
    vr::HmdQuaternion_t q;

    q.w = sqrt(fmax(0, 1 + matrix.m[0][0] + matrix.m[1][1] + matrix.m[2][2])) / 2;
    q.x = sqrt(fmax(0, 1 + matrix.m[0][0] - matrix.m[1][1] - matrix.m[2][2])) / 2;
    q.y = sqrt(fmax(0, 1 - matrix.m[0][0] + matrix.m[1][1] - matrix.m[2][2])) / 2;
    q.z = sqrt(fmax(0, 1 - matrix.m[0][0] - matrix.m[1][1] + matrix.m[2][2])) / 2;
    q.x = copysign(q.x, matrix.m[2][1] - matrix.m[1][2]);
    q.y = copysign(q.y, matrix.m[0][2] - matrix.m[2][0]);
    q.z = copysign(q.z, matrix.m[1][0] - matrix.m[0][1]);
    return q;
}
//-----------------------------------------------------------------------------
// Purpose: Extracts position (x,y,z).
// from: https://github.com/Omnifinity/OpenVR-Tracking-Example/blob/master/HTC%20Lighthouse%20Tracking%20Example/LighthouseTracking.cpp
//-----------------------------------------------------------------------------

vr::HmdVector3_t ExampleDriver::VRDriver::GetPosition(vr::HmdMatrix34_t matrix) {
    vr::HmdVector3_t vector;

    vector.v[0] = matrix.m[0][3];
    vector.v[1] = matrix.m[1][3];
    vector.v[2] = matrix.m[2][3];

    return vector;
}
