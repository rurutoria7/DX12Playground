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

    m_wnd = CreateWindowExW(WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW, (LPCWSTR)m_wndClass, L"DX12 Playground", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, m_width, m_height, nullptr, nullptr, wcex.hInstance, nullptr);

    if (!m_wnd)
	{
		std::cerr << "Failed to create window" << std::endl;
		std::exit(-1);
	}

    // Describe swap chain
    DXGI_SWAP_CHAIN_DESC1 sd{};
    sd.Width = m_width;
    sd.Height = m_height;
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
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // how buffer swap will happen
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

    GetSwapChainBuffers();
}

void MyWindow::Update()
{
    // clear message queue
    MSG msg;
    while (PeekMessageW(&msg, m_wnd, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
		DispatchMessageW(&msg);
    }


    if (MyWindow::Get().ShouldResize())
    {
        DXContext::Get().Flush(1);
        MyWindow::Get().Resize();
    }
}

void MyWindow::Present()
{
    m_swapChain->Present(1, 0);
}

void MyWindow::Shutdown()
{
    ReleaseSwapChainBuffers();
    if(m_wnd) DestroyWindow(m_wnd);
    if(m_wndClass) UnregisterClassW((LPCWSTR)m_wndClass, GetModuleHandle(nullptr));
    m_swapChain.Release();
}

/*

Please flush before call this

*/
void MyWindow::Resize()
{
    RECT rect;
    if (!GetClientRect(m_wnd, &rect)) return;
    m_shouldResize = false;
    m_width = rect.right - rect.left;
    m_height = rect.bottom - rect.top;
    std::cout << "Resized to " << m_width << "x" << m_height << std::endl;

    m_height = std::max(1U, m_height);
    m_width = std::max(1U, m_width);

    ReleaseSwapChainBuffers();
    m_swapChain->ResizeBuffers(2, m_width, m_height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);
    GetSwapChainBuffers();
}

void MyWindow::SetFullScreen(bool fullScreenEnabled)
{
    unsigned int style = (fullScreenEnabled ? WS_POPUP : WS_OVERLAPPEDWINDOW) | WS_VISIBLE;
    unsigned int exStyle = (fullScreenEnabled ? WS_EX_OVERLAPPEDWINDOW : 0) | WS_EX_APPWINDOW;
    if (!SetWindowLongPtrA(m_wnd, GWL_STYLE, style))
    {
        std::cout << "Failed to set window style" << std::endl;
        std::exit(-1);
    }
    if (!SetWindowLongPtrA(m_wnd, GWL_EXSTYLE, exStyle))
    {
        std::cout << "Failed to set ex window style" << std::endl;
        std::exit(-1);
    }

    // set window position
    if (fullScreenEnabled)
    {
        HMONITOR monitor = MonitorFromWindow(m_wnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi{};
        mi.cbSize = sizeof(mi);
        if (!GetMonitorInfoW(monitor, &mi))
		{
			std::cout << "Failed to get monitor info" << std::endl;
			std::exit(-1);
		}
        int x = mi.rcMonitor.left;
        int y = mi.rcMonitor.top;
        int w = mi.rcMonitor.right - mi.rcMonitor.left;
        int h = mi.rcMonitor.bottom - mi.rcMonitor.top;
        if (!SetWindowPos(m_wnd, HWND_TOP, x, y, w, h, SWP_NOZORDER))
        {
            std::cout << "Failed to set window position" << std::endl;
			std::exit(-1);
        }
    }
    else
    {
        ShowWindow(m_wnd, SW_RESTORE);
    }
    m_shouldResize = true;
    m_fullScreen = fullScreenEnabled;
}

void MyWindow::SetBackBufferStateRT(ID3D12GraphicsCommandList7* OUT cmdList)
{
    m_currentBackBuffer = m_swapChain->GetCurrentBackBufferIndex();

    D3D12_RESOURCE_BARRIER brr{};
    brr.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    brr.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    brr.Transition.pResource = m_backBuffers[m_currentBackBuffer];
    brr.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    brr.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    brr.Transition.Subresource = 0;

    cmdList->ResourceBarrier(1, &brr);
}

void MyWindow::SetBackBufferStatePRESENT(ID3D12GraphicsCommandList7* OUT cmdList)
{
    D3D12_RESOURCE_BARRIER brr{};
    brr.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    brr.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    brr.Transition.pResource = m_backBuffers[m_currentBackBuffer];
    brr.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    brr.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    brr.Transition.Subresource = 0;

    cmdList->ResourceBarrier(1, &brr);
}

LRESULT(MyWindow::OnWindowMessage)(HWND wnd, UINT msg, WPARAM wparam, LPARAM lpram)
{
    if (msg == WM_KEYDOWN && wparam == VK_F11)
	{
        MyWindow::Get().SetFullScreen(!MyWindow::Get().m_fullScreen);
	}
    else if (msg == WM_SIZE)
    {
        MyWindow::Get().m_shouldResize = true;
    }
    else if (msg == WM_DESTROY)
	{
        MyWindow::Get().m_shouldClose = true;
	}
    return DefWindowProcW(wnd, msg, wparam, lpram);
}

void MyWindow::GetSwapChainBuffers()
{
    for (int i = 0; i < 2; i++)
    {
        if (FAILED(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_backBuffers[i]))))
        {
            std::cerr << "Failed to get swap chain buffer" << std::endl;
            std::exit(-1);
        }
    }

    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Texture2D.MipSlice = 0;
    rtvDesc.Texture2D.PlaneSlice = 0;

    DXContext::Get().GetDevice()->CreateRenderTargetView(m_backBuffers[0], &rtvDesc, DXContext::Get().GetRTVHeapHandle()[0]);
    DXContext::Get().GetDevice()->CreateRenderTargetView(m_backBuffers[1], &rtvDesc, DXContext::Get().GetRTVHeapHandle()[1]);
}


void MyWindow::ReleaseSwapChainBuffers()
{
	m_backBuffers[0].Release();
	m_backBuffers[1].Release();
}
