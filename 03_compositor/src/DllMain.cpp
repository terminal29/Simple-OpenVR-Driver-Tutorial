#include <windows.h>
#include <Rendering.hpp>
#include <DriverLog.hpp>
#include <Debug.hpp>

BOOL WINAPI DllMain(HINSTANCE dll_hinstance, DWORD call_reason, LPVOID lpvReserved){
	switch (call_reason) {
		case DLL_PROCESS_ATTACH:
		{
			Rendering::set_hinstance(dll_hinstance);
			break;
		}
		case DLL_PROCESS_DETACH:
		{
			Rendering::set_hinstance(nullptr);
			break;
		}
		default:
			break;
	}
	return true;
}