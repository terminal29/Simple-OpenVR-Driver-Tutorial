#pragma once
#include <StepTimer.hpp>

#include <locale>
#include <codecvt>
#include <string>
#include <stdexcept>
#include <functional>
#include <algorithm>

typedef std::function<void(ID3D11Device1*, ID3D11DeviceContext1*, IDXGISwapChain1*, ID3D11RenderTargetView*, ID3D11DepthStencilView*)> RenderFunction;

namespace Rendering {

	HWND create_window();

	void initialize(HWND window, int width, int height);

	void set_on_render_fn(RenderFunction render_fn);

	void tick();

	void on_activated();
	void on_deactivated();
	void on_suspending();
	void on_resuming();
	void on_window_size_changed(int width, int height);

	void get_default_size(int& width, int& height);

	void quit();

};