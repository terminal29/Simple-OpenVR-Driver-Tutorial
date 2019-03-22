#include <windows.h>
#include <Rendering.hpp>
#include <DriverLog.hpp>
#include <Debug.hpp>

BOOL WINAPI DllMain(HINSTANCE dll_hinstance, DWORD call_reason, LPVOID lpvReserved){
	switch (call_reason) {
		case DLL_PROCESS_ATTACH:
		{
			break;
		}
		case DLL_PROCESS_DETACH:
		{
			break;
		}
		default:
			break;
	}
	return true;
}