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
	inline bool ShouldClose() const { return m_shouldClose; }

private:
	static LRESULT(CALLBACK OnWindowMessage)(HWND wnd, UINT msg, WPARAM wparam, LPARAM lpram);
	ATOM m_wndClass = 0;
	HWND m_wnd = nullptr;
	bool m_shouldClose = false;
	void SetShouldClose() { m_shouldClose = true; }
	ComPointer<IDXGISwapChain4> m_swapChain;

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

