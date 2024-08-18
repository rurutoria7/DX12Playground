#include "DXDebugLayer.h"

bool DXDebugLayer::Init()
{
#ifdef _DEBUG
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&m_debugController))))
	{
		m_debugController->EnableDebugLayer();
	}
	else
	{
		return false;
	}

	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&m_dxgiDebug))))
	{
		m_dxgiDebug->EnableLeakTrackingForThread();
	}
	else
	{
		return false;
	}

	return true;
#endif

    return false;
}

void DXDebugLayer::Shutdown()
{
#ifdef _DEBUG
	if (m_dxgiDebug)
	{
		OutputDebugStringW(L"[app] DXGI Reports living device objects:\n");
		m_dxgiDebug->ReportLiveObjects(
			DXGI_DEBUG_ALL, (DXGI_DEBUG_RLO_FLAGS)(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL)
		);
	}
#endif
}
