#include "MyWindow.h"

void MyWindow::Init()
{
	WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(wcex);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = &MyWindow::OnWindowMessage;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = GetModuleHandle(nullptr);
    wcex.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
    wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wcex.hbrBackground = nullptr;
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName =  L"DX12WindowClass";
    wcex.hIconSm = LoadIconW(nullptr, IDI_APPLICATION);
    m_wndClass = RegisterClassExW(&wcex);
    
    if (!m_wndClass)
    {
        std::cerr << "Failed to register window class" << std::endl;
        std::exit(-1);
    }

    m_wnd = CreateWindowExW(WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW, (LPCWSTR)m_wndClass, L"DX12 Playground", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 720, 480, nullptr, nullptr, wcex.hInstance, nullptr);

    if (!m_wnd)
	{
		std::cerr << "Failed to create window" << std::endl;
		std::exit(-1);
	}

    // Describe swap chain
    DXGI_SWAP_CHAIN_DESC1 sd{};
    sd.Width = 1920;
    sd.Height = 1080; 
    sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // the format write to swap chain, rgba, 8 bit per channel, unsigned normalized number
    sd.Stereo = false; // enable 3d
    sd.SampleDesc.Count = 1;    // msaa setupt, always set to 1
    sd.SampleDesc.Quality = 0;  // msaa setupt, always set to 0
    // specify the usage of swap chain, might for optimization or r/w access control
    // BACK_BUFFER: back buffer of swap chain
    // RENDER_TARGET_OUTPUT: as a pipeline output target
    sd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT ; 
    sd.BufferCount = 2; // number of frame buffer
    sd.Scaling =  DXGI_SCALING_STRETCH; // what to do when swap chain size doesn't match the window size
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // how buffer swap will happen
    sd.AlphaMode = DXGI_ALPHA_MODE_IGNORE; // alpha blending 
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING; // flags

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsd{};
    fsd.Windowed = true; // full screen is windowed 

    auto& factory = DXContext::Get().GetFactory();
    ComPointer<IDXGISwapChain1> sc1;
    factory->CreateSwapChainForHwnd(DXContext::Get().GetCommandQueue(), m_wnd, &sd, &fsd, nullptr, &sc1);
    if (!sc1.QueryInterface(m_swapChain))
	{
		std::cerr << "Failed to create swap chain" << std::endl;
		std::exit(-1);
	}
}

void MyWindow::Update()
{
    MSG msg;
    while (PeekMessageW(&msg, m_wnd, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
		DispatchMessageW(&msg);
    }
}

void MyWindow::Present()
{
    m_swapChain->Present(1, 0);
}

void MyWindow::Shutdown()
{
    if(m_wnd) DestroyWindow(m_wnd);
    if(m_wndClass) UnregisterClassW((LPCWSTR)m_wndClass, GetModuleHandle(nullptr));
    m_swapChain.Release();
}

LRESULT(MyWindow::OnWindowMessage)(HWND wnd, UINT msg, WPARAM wparam, LPARAM lpram)
{
    if (msg == WM_DESTROY)
	{
        MyWindow::Get().SetShouldClose();
	}
    return DefWindowProcW(wnd, msg, wparam, lpram);
}
