#pragma once
#include <Support/WinInclude.h>
#include <iostream>
#include <D3D/DXContext.h>

class MyWindow
{
public:
	void Init();
	void Update();
	void Present();
	void Shutdown();
	void Resize();
	void SetFullScreen(bool fullScreenEnabled);
	inline bool ShouldResize() const { return m_shouldResize; }
	inline bool ShouldClose() const { return m_shouldClose; }
	void SetBackBufferStateRT(ID3D12GraphicsCommandList7* OUT cmdList);
	void SetBackBufferStatePRESENT(ID3D12GraphicsCommandList7* OUT cmdList);
	unsigned int GetCurrentBackBuffer() { return m_currentBackBuffer; }
	inline unsigned int GetHeight() { return m_height;  }
	inline unsigned int GetWidth() { return m_width;  }


private:
	static LRESULT(CALLBACK OnWindowMessage)(HWND wnd, UINT msg, WPARAM wparam, LPARAM lpram);
	ATOM m_wndClass = 0;
	HWND m_wnd = nullptr;
	bool m_shouldClose = false;
	bool m_shouldResize = false;
	bool m_fullScreen = false;
	ComPointer<IDXGISwapChain4> m_swapChain;
	ComPointer<ID3D12Resource> m_backBuffers[2];
	unsigned int m_width = 1280;
	unsigned int m_height = 720;
	int m_currentBackBuffer = 0;

	void GetSwapChainBuffers();
	void ReleaseSwapChainBuffers();


public:
	MyWindow(const MyWindow&) = delete;
	MyWindow& operator=(const MyWindow&) = delete; 
	static MyWindow& Get()
	{
		static MyWindow instance;
		return instance;
	}
private:
	MyWindow() = default;
};

