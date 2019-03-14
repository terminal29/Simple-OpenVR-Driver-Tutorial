#pragma once

#include <d3d11.h>
#include <DXUT.h>
#include <DXUTDevice11.h>
#include <windows.h>
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "comctl32.lib")

#include <locale>
#include <codecvt>
#include <string>
#include <stdexcept>
#include <functional>

namespace Rendering {
	void set_hinstance(HINSTANCE hinstance);

	bool initialise_window(std::string window_name, int x, int y, int width, int height);

	void set_on_render_fn(LPDXUTCALLBACKD3D11FRAMERENDER callback_fn, void* context);

	void main_loop();

	void close_window();

};