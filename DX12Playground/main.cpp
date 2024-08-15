#include <Debug/DXDebugLayer.h>
#include <D3D/DXContext.h>
#include <Support/MyWindow.h>
#include <iostream>

int main()
{
	DXDebugLayer::Get().Init();
	DXContext::Get().Init();
	MyWindow::Get().Init();

	while (!MyWindow::Get().ShouldClose())
	{
		MyWindow::Get().Update();
		auto* cmdList = DXContext::Get().InitCommandList();
		DXContext::Get().ExecuteCommandList();
		MyWindow::Get().Present();
	}

	DXContext::Get().Flush(2);
	MyWindow::Get().Shutdown();
	DXContext::Get().Shutdown();
	DXDebugLayer::Get().Shutdown(); // report com objects
}