#include <Rendering.hpp>


namespace _internal {
	HINSTANCE _dll_hinstance = nullptr;
	char _classname[] = "_render_window_class";

};

void Rendering::set_hinstance(HINSTANCE hinstance)
{
	_internal::_dll_hinstance = hinstance;
}

bool Rendering::initialise_window(std::string window_name, int x, int y, int width, int height)
{
	std::wstring ws_window_name(window_name.begin(), window_name.end());
	HRESULT result = DXUTInit(false, false, nullptr, true);
	if (!SUCCEEDED(result)) {
		printf("Failed at DXUTInit\n");
		return false;
	}
	DXUTSetCursorSettings();
	result = DXUTCreateWindow(ws_window_name.c_str(), nullptr, nullptr, nullptr, x, y);
	if (!SUCCEEDED(result)) {
		printf("Failed at DXUTCreateWindow\n");
		return false;
	}

	DXUTCreateDevice(D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_1, true, width, height);
	if (!SUCCEEDED(result)) {
		printf("Failed at DXUTCreateDevice\n");
		return false;
	}

	return true;
}

void Rendering::set_on_render_fn(LPDXUTCALLBACKD3D11FRAMERENDER callback_fn, void* context)
{
	DXUTSetCallbackD3D11FrameRender(callback_fn, context);
}

void Rendering::main_loop()
{
	DXUTMainLoop();
}

void Rendering::close_window()
{
}
