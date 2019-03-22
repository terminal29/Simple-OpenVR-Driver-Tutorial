#include <Rendering.hpp>

using Microsoft::WRL::ComPtr;

namespace DX
{
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			// Set a breakpoint on this line to catch DirectX API errors
			throw std::exception();
		}
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	static bool s_in_sizemove = false;
	static bool s_in_suspend = false;
	static bool s_minimized = false;
	static bool s_fullscreen = false;
	// TODO: Set s_fullscreen to true if defaulting to fullscreen.


	switch (message)
	{
	case WM_PAINT:
		if (s_in_sizemove)
		{
			Rendering::tick();
		}
		else
		{
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		}
		break;

	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
		{
			if (!s_minimized)
			{
				s_minimized = true;
				if (!s_in_suspend)
					Rendering::on_suspending();
				s_in_suspend = true;
			}
		}
		else if (s_minimized)
		{
			s_minimized = false;
			if (s_in_suspend)
				Rendering::on_resuming();
			s_in_suspend = false;
		}
		else if (!s_in_sizemove)
		{
			Rendering::on_window_size_changed(LOWORD(lParam), HIWORD(lParam));
		}
		break;

	case WM_ENTERSIZEMOVE:
		s_in_sizemove = true;
		break;

	case WM_EXITSIZEMOVE:
		s_in_sizemove = false;
			RECT rc;
			GetClientRect(hWnd, &rc);

			Rendering::on_window_size_changed(rc.right - rc.left, rc.bottom - rc.top);
		
		break;

	case WM_GETMINMAXINFO:
	{
		auto info = reinterpret_cast<MINMAXINFO*>(lParam);
		info->ptMinTrackSize.x = 320;
		info->ptMinTrackSize.y = 200;
	}
	break;

	case WM_ACTIVATEAPP:
		if (wParam)
		{
			Rendering::on_activated();
		}
		else
		{
			Rendering::on_deactivated();
		}
		
		break;

	case WM_POWERBROADCAST:
		switch (wParam)
		{
		case PBT_APMQUERYSUSPEND:
			if (!s_in_suspend)
				Rendering::on_suspending();
			s_in_suspend = true;
			return TRUE;

		case PBT_APMRESUMESUSPEND:
			if (!s_minimized)
			{
				if (s_in_suspend)
					Rendering::on_resuming();
				s_in_suspend = false;
			}
			return TRUE;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_SYSKEYDOWN:
		if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000)
		{
			// Implements the classic ALT+ENTER fullscreen toggle
			if (s_fullscreen)
			{
				SetWindowLongPtr(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
				SetWindowLongPtr(hWnd, GWL_EXSTYLE, 0);

				int width = 800;
				int height = 600;
				Rendering::get_default_size(width, height);

				ShowWindow(hWnd, SW_SHOWNORMAL);

				SetWindowPos(hWnd, HWND_TOP, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
			}
			else
			{
				SetWindowLongPtr(hWnd, GWL_STYLE, 0);
				SetWindowLongPtr(hWnd, GWL_EXSTYLE, WS_EX_TOPMOST);

				SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

				ShowWindow(hWnd, SW_SHOWMAXIMIZED);
			}

			s_fullscreen = !s_fullscreen;
		}
		break;

	case WM_MENUCHAR:
		// A menu is active and the user presses a key that does not correspond
		// to any mnemonic or accelerator key. Ignore so we don't produce an error beep.
		return MAKELRESULT(0, MNC_CLOSE);
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

namespace _internal {
	// Device resources.
	HWND                                            _window;
	int                                             _output_width;
	int                                             _output_height;

	D3D_FEATURE_LEVEL                               _feature_level;
	Microsoft::WRL::ComPtr<ID3D11Device1>           _d3d_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext1>    _d3d_context;

	Microsoft::WRL::ComPtr<IDXGISwapChain1>         _swap_chain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  _render_target_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  _depth_stencil_view;

	// Rendering loop timer.
	DX::StepTimer                                   _timer;

	RenderFunction									_render_fn;

	void on_device_lost();

	void update(DX::StepTimer const& timer) {
		float elapsedTime = (float)timer.GetElapsedSeconds();

		_internal::_render_fn(_d3d_device.Get(), _d3d_context.Get(), _swap_chain.Get(), _render_target_view.Get(), _depth_stencil_view.Get());

	}

	void clear() {
		_internal::_d3d_context->ClearRenderTargetView(_internal::_render_target_view.Get(), DirectX::Colors::CornflowerBlue);
		_internal::_d3d_context->ClearDepthStencilView(_internal::_depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		_internal::_d3d_context->OMSetRenderTargets(1, _internal::_render_target_view.GetAddressOf(), _internal::_depth_stencil_view.Get());

		// Set the viewport.
		CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(_output_width), static_cast<float>(_output_height));
		_internal::_d3d_context->RSSetViewports(1, &viewport);
	}

	void present() {
		HRESULT hr = _internal::_swap_chain->Present(1, 0);

		// If the device was reset we must completely reinitialize the renderer.
		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			on_device_lost();
		}
		else
		{
			DX::ThrowIfFailed(hr);
		}
	}

	void render() {
		if (_internal::_timer.GetFrameCount() == 0)
			return;
		clear();
		present();
	}

	void create_device() {
		UINT creationFlags = 0;

#ifdef _DEBUG
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		static const D3D_FEATURE_LEVEL featureLevels[] =
		{
			// TODO: Modify for supported Direct3D feature levels
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1,
		};

		// Create the DX11 API device object, and get a corresponding context.
		ComPtr<ID3D11Device> device;
		ComPtr<ID3D11DeviceContext> context;
		DX::ThrowIfFailed(D3D11CreateDevice(
			nullptr,                            // specify nullptr to use the default adapter
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			creationFlags,
			featureLevels,
			_countof(featureLevels),
			D3D11_SDK_VERSION,
			device.ReleaseAndGetAddressOf(),    // returns the Direct3D device created
			&_internal::_feature_level,         // returns feature level of device created
			context.ReleaseAndGetAddressOf()    // returns the device immediate context
		));

#ifndef NDEBUG
		ComPtr<ID3D11Debug> d3dDebug;
		if (SUCCEEDED(device.As(&d3dDebug)))
		{
			ComPtr<ID3D11InfoQueue> d3dInfoQueue;
			if (SUCCEEDED(d3dDebug.As(&d3dInfoQueue)))
			{
#ifdef _DEBUG
				d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
				d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
				D3D11_MESSAGE_ID hide[] =
				{
					D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
					// TODO: Add more message IDs here as needed.
				};
				D3D11_INFO_QUEUE_FILTER filter = {};
				filter.DenyList.NumIDs = _countof(hide);
				filter.DenyList.pIDList = hide;
				d3dInfoQueue->AddStorageFilterEntries(&filter);
			}
		}
#endif

		DX::ThrowIfFailed(device.As(&_internal::_d3d_device));
		DX::ThrowIfFailed(context.As(&_internal::_d3d_context));
	}

	void create_resources() {
		// Clear the previous window size specific context.
		ID3D11RenderTargetView* nullViews[] = { nullptr };
		_internal::_d3d_context->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
		_internal::_render_target_view.Reset();
		_internal::_depth_stencil_view.Reset();
		_internal::_d3d_context->Flush();

		UINT backBufferWidth = static_cast<UINT>(_internal::_output_width);
		UINT backBufferHeight = static_cast<UINT>(_internal::_output_height);
		DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
		DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		UINT backBufferCount = 2;

		// If the swap chain already exists, resize it, otherwise create one.
		if (_internal::_swap_chain)
		{
			HRESULT hr = _internal::_swap_chain->ResizeBuffers(backBufferCount, backBufferWidth, backBufferHeight, backBufferFormat, 0);

			if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
			{
				// If the device was removed for any reason, a new device and swap chain will need to be created.
				on_device_lost();

				// Everything is set up now. Do not continue execution of this method. OnDeviceLost will reenter this method 
				// and correctly set up the new device.
				return;
			}
			else
			{
				DX::ThrowIfFailed(hr);
			}
		}
		else
		{
			// First, retrieve the underlying DXGI Device from the D3D Device.
			ComPtr<IDXGIDevice1> dxgiDevice;
			DX::ThrowIfFailed(_internal::_d3d_device.As(&dxgiDevice));

			// Identify the physical adapter (GPU or card) this device is running on.
			ComPtr<IDXGIAdapter> dxgiAdapter;
			DX::ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));

			// And obtain the factory object that created it.
			ComPtr<IDXGIFactory2> dxgiFactory;
			DX::ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

			// Create a descriptor for the swap chain.
			DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
			swapChainDesc.Width = backBufferWidth;
			swapChainDesc.Height = backBufferHeight;
			swapChainDesc.Format = backBufferFormat;
			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.SampleDesc.Quality = 0;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.BufferCount = backBufferCount;

			DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
			fsSwapChainDesc.Windowed = TRUE;

			// Create a SwapChain from a Win32 window.
			DX::ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
				_internal::_d3d_device.Get(),
				_internal::_window,
				&swapChainDesc,
				&fsSwapChainDesc,
				nullptr,
				_internal::_swap_chain.ReleaseAndGetAddressOf()
			));

			// This template does not support exclusive fullscreen mode and prevents DXGI from responding to the ALT+ENTER shortcut.
			DX::ThrowIfFailed(dxgiFactory->MakeWindowAssociation(_internal::_window, DXGI_MWA_NO_ALT_ENTER));
		}

		// Obtain the backbuffer for this window which will be the final 3D rendertarget.
		ComPtr<ID3D11Texture2D> backBuffer;
		DX::ThrowIfFailed(_internal::_swap_chain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));

		// Create a view interface on the rendertarget to use on bind.
		DX::ThrowIfFailed(_internal::_d3d_device->CreateRenderTargetView(backBuffer.Get(), nullptr, _internal::_render_target_view.ReleaseAndGetAddressOf()));

		// Allocate a 2-D surface as the depth/stencil buffer and
		// create a DepthStencil view on this surface to use on bind.
		CD3D11_TEXTURE2D_DESC depthStencilDesc(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL);

		ComPtr<ID3D11Texture2D> depthStencil;
		DX::ThrowIfFailed(_internal::_d3d_device->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf()));

		CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
		DX::ThrowIfFailed(_internal::_d3d_device->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, _internal::_depth_stencil_view.ReleaseAndGetAddressOf()));

		// TODO: Initialize windows-size dependent objects here.
	}

	void on_device_lost() {
		_internal::_depth_stencil_view.Reset();
		_internal::_render_target_view.Reset();
		_internal::_swap_chain.Reset();
		_internal::_d3d_context.Reset();
		_internal::_d3d_device.Reset();

		create_device();

		create_resources();
	}


};

HWND Rendering::create_window()
{
	{
		HINSTANCE hInstance = GetModuleHandle(NULL);
		// Register class
		WNDCLASSEXW wcex = {};
		wcex.cbSize = sizeof(WNDCLASSEXW);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.hInstance = hInstance;
		wcex.hIcon = LoadIconW(hInstance, L"IDI_ICON");
		wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszClassName = L"Direct3D_Win32_Game2WindowClass";
		wcex.hIconSm = LoadIconW(wcex.hInstance, L"IDI_ICON");
		if (!RegisterClassExW(&wcex))
			return nullptr;

		// Create window
		int w, h;
		get_default_size(w, h);

		RECT rc = { 0, 0, static_cast<LONG>(w), static_cast<LONG>(h) };

		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

		HWND hwnd = CreateWindowExW(0, L"Direct3D_Win32_Game2WindowClass", L"Direct3D Win32 Game2", WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
			nullptr);
		// TODO: Change to CreateWindowExW(WS_EX_TOPMOST, L"Direct3D_Win32_Game2WindowClass", L"Direct3D Win32 Game2", WS_POPUP,
		// to default to fullscreen.

		if (!hwnd)
			return nullptr;

		ShowWindow(hwnd, SW_SHOWMAXIMIZED);
		// TODO: Change nCmdShow to SW_SHOWMAXIMIZED to default to fullscreen.

		//SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(g_game.get()));

		GetClientRect(hwnd, &rc);

		initialize(hwnd, rc.right - rc.left, rc.bottom - rc.top);
		return hwnd;
	}
}

void Rendering::initialize(HWND window, int width, int height) {
	_internal::_window = window;
	_internal::_output_width = std::max(width, 1);
	_internal::_output_height = std::max(height, 1);

	_internal::create_device();

	_internal::create_resources();

}

void Rendering::set_on_render_fn(RenderFunction render_fn)
{
	_internal::_render_fn = render_fn;
}

void Rendering::tick() {
	_internal::_timer.Tick([&]() {_internal::update(_internal::_timer); });
	_internal::render();
}

void Rendering::on_activated() {

}

void Rendering::on_deactivated() {

}

void Rendering::on_suspending() {

}

void Rendering::on_resuming() {

}

void Rendering::on_window_size_changed(int width, int height) {
	_internal::_output_width = std::max(width, 1);
	_internal::_output_height = std::max(height, 1);

	_internal::create_resources();
}

void Rendering::get_default_size(int& width, int& height) {
	width = 800;
	height = 600;
}

void Rendering::quit()
{
	PostQuitMessage(0);
}
