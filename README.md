# Simple-OpenVR-Driver-Tutorial
A tutorial and sample code for how to write a simple OpenVR (SteamVR) driver. 

I am by no means an expert (I am also learning this as I go) so if you have a comment or suggestion please make an issue or pull request.

## IN PROGRESS 

# Setting up your Environment
Clone the [OpenVR SDK](https://github.com/ValveSoftware/openvr/) into a folder that you will be using for libraries, my folder structure goes like:
- ExamplePlugin
  - source
  - lib
  - build
  - project

Now copy the *driver_sample.vcxproj* project file from *openvr/samples/driver_sample/* into your project folder, we will be using it as a base and just modifying a few things. Open it with VS and delete all of the .h and .cpp files because we will be adding our own. Right click the project and select *Properties* and with *All Configurations* and *x64* selected:
- Change the *Output Directory* to your chosen build directory
- Under *VC++ Directories* add the openvr *include* folder to the *Include Directories* box and the openvr *lib/win64/* to the *Library Directories* box
- Under *Linker->Input* add *openvr_api.lib* to the *Additional Dependencies* box. 

You can also add whatever other libraries/apis you will be using now as well.
  
# Ensure the VR Server is loading our driver
First of all we need to make sure the VR Server is actually loading our driver.
  
This will be run when the server loads the .dll file like main() is in a normal c/cpp program. We don't use it for anything in particular but it could be useful if you need to confirm the server is actually loading your file but putting a *MessageBoxA(NULL, "","Driver Loaded!",MB_OK)* under *DLL_PROCESS_ATTACH*.
