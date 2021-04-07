# Hip locomotion branch

Navigation in VR using hip orientation. You need a hip tracker that provides orientation information, which can be a vive tracker, apriltag trackers or a phone with owotrack!

For help or questions, write into hip-locomotion related channels on my [discord](https://discord.gg/CSnp8AB3yy)!

### Installation

1. Download hip_locomotion_v0.1.zip from [releases](https://github.com/ju1ce/Simple-OpenVR-Bridge-Driver/releases/tag/v0.1) and extract it
2. Run install_driver.exe from driver_files
3. Start up steamvr, connect your tracker and ensure it is set to waist.
4. Launch your game of choice.
5. Go to steamvr settings -> controllers -> manage controller bindings, set it to custom and click edit this binding. Unbind the left joystick position of your controller. Only unbind the position part, leave touch and click bound!
6. Ensure your games locomotion direction is set to hmd.
7. Launch hip_locomotion.exe from the bin directory
8. Play!

### Calibration

To calibrate the direction of your hip controller, simply:

1. Open your steamvr dashboard
2. Stand and look straight
3. Close steamvr dashboard

Hip direction should now be calibrated!

## Troubleshooting:

Vive and index bindings were broken in 0.1, make sure you have 0.1.1!

On vive, make sure you dont unbind "activate movement", only unbind the trackpad position!

Dont unbind click or touch, only position!

If you have decamove installed, try uninstalling it as it may sometimes still disable input even if its not connected.

Using kinecttovr or driver4vr is untested: if you tried using it, please let me know, if it works or not!

If you find any other problems, write into the hip-locomotion-help channel of ApriltagTrackers [link to Discord](https://discord.gg/CSnp8AB3yy "https://discord.gg/CSnp8AB3yy"), or write an issue.

## Examples of tracking systems you can use with this: 

- OwoTrack ([link to Discord](https://discord.com/invite/ZVFfgt7tuj "https://discord.com/invite/ZVFfgt7tuj")) - uses phone as a hip tracker
- ApriltagTrackers ([link to Discord](https://discord.gg/CSnp8AB3yy "https://discord.gg/CSnp8AB3yy"), [link to Github]( https://github.com/ju1ce/April-Tag-VR-FullBody-Tracker "https://github.com/ju1ce/April-Tag-VR-FullBody-Tracker")) - uses cardboard/3d printed markers for fullbody tracking
- any other commercial tracker systems where you have a hip tracker

### Known issues:

- The tracker should not be pointing up or direction is not going to work properly. This means that, if you use owotrack with a phone in your pocket, crouching may break direction.

Below is the build instructions of the original driver that this one is based on. Most should still be relavant in case you want to build this driver. **This is only to build it from source for development. If you just wish to try this project, you do not need it.**

# Simple OpenVR Driver Tutorial
I created this driver as a demonstration for how to write some of the most common things a SteamVR/OpenVR driver would want to do. You will need to understand C++11 and some C++17 features at least to make the most use of this repo. It features:

- [Central driver setup](driver_files/src/Driver/IVRDriver.hpp)
to manage addition and removal of devices, and updating devices each frame, collecting events, access to OpenVR internals, etc...

- [Reading configuration files](driver_files/src/Driver/VRDriver.cpp#L114)
to load user settings 

- [Logging](driver_files/src/Driver/VRDriver.cpp#L142)
for simple debug messages

- [Tracked HMD](driver_files/src/Driver/HMDDevice.hpp)
which is a tracked device that acts as a video output

- [Tracked Controllers](driver_files/src/Driver/ControllerDevice.hpp)
which is a tracked device that has mapped buttons, triggers, touchpads, joysticks, etc...

- [Tracked Trackers](driver_files/src/Driver/TrackerDevice.hpp)
which is a device purely meant for tracking the location of an object

- [Tracking References (base stations)](driver_files/src/Driver/TrackingReferenceDevice.hpp)
which is a base station or camera designed as a fixed point of reference to the real world

- [Custom Device Render Models](driver_files/driver/example/resources/rendermodels/example_controller)
so your new controllers look cool

- [Visual Studio Debugging Setup for SteamVR](#debugging)
because a debugger is a developers best friend <sup>(besides ctrl-z)</sup>.

## Building
- Clone the project and submodules
	- `git clone --recursive https://github.com/terminal29/Simple-OpenVR-Driver-Tutorial.git`
- Build project with CMake
	- `cd Simple-OpenVR-Driver-Tutorial && cmake .`
- Open project with Visual Studio and hit build
	- Driver folder structure and files will be copied to the output folder as `example`.
	
## Installation

There are two ways to "install" your plugin:

- Find your SteamVR driver directory, which should be at:
  `C:\Program Files (x86)\Steam\steamapps\common\SteamVR\drivers`
  and copy the `example` directory from the project's build directory into the SteamVR drivers directory. Your folder structure should look something like this:

![Drivers folder structure](https://i.imgur.com/hOsDk1H.png)
or

- Navigate to `C:\Users\<Username>\AppData\Local\openvr` and find the `openvrpaths.vrpath` file. Open this file with your text editor of choice, and under `"external_drivers"`, add another entry with the location of the `example` folder. For example mine looks like this after adding the entry:

```json
{
	"config" : 
	[
		"C:\\Program Files (x86)\\Steam\\config",
		"c:\\program files (x86)\\steam\\config"
	],
	"external_drivers" : 
	[
		"C:\\Users\\<Username>\\Documents\\Programming\\c++\\Simple-OpenVR-Driver-Tutorial\\build\\Debug\\example"
	],
	"jsonid" : "vrpathreg",
	"log" : 
	[
		"C:\\Program Files (x86)\\Steam\\logs",
		"c:\\program files (x86)\\steam\\logs"
	],
	"runtime" : 
	[
		"C:\\Program Files (x86)\\Steam\\steamapps\\common\\SteamVR"
	],
	"version" : 1
}
```

## Debugging
Debugging SteamVR is not as simple as it seems because of the startup procedure it uses. The SteamVR ecosystem consists of a couple programs:

 - **vrserver**: the driver host
 - **vrcompositor**: the render engine
 - **vrmonitor**: the popup that displays status information
 - **vrdashboard**: the VR menu/overlay
 - **vrstartup**: a program to start everything up
 
 To debug effectively in Visual Studio, you can use an extension called [Microsoft Child Process Debugging Power Tool](https://marketplace.visualstudio.com/items?itemName=vsdbgplat.MicrosoftChildProcessDebuggingPowerTool) and enable debugging child processes, disable debugging for all other child processes, and add `vrserver.exe` as a child process to debug as below:
  
![Child process debugging settings](https://i.imgur.com/yDNvLMm.png)

Set the program the project should run in debug mode to **vrstartup** (Usually located `C:\Program Files (x86)\Steam\steamapps\common\SteamVR\bin\win64\vrstartup.exe`). Now we can start up SteamVR without needing to go through Steam, and can properly startup all the other programs vrserver needs. 

## Issues
I don't have an issue template, but if you find what you think is a bug, and can describe how to reproduce it, please leave an issue and/or pull request with the details.

## License
MIT License

Copyright (c) 2020 Jacob Hilton (Terminal29)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
