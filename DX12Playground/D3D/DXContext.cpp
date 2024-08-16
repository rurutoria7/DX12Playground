#include "DXContext.h"

/*[notes]

GPU
<------->
Command Queue
<------->
CommandList

*/
void DXContext::Init()
{
	if (FAILED(CreateDXGIFactory2(0, IID_PPV_ARGS(&m_factory))))
		std::exit(-1);

	/* [notes]
	
	* create gpu device
		* nullptr: use default gpu
		* D3D_FEATURE_LEVEL_11_0: minimum feature level required to gpu
		* IID_PPV_ARGS(&device): COM Type GUID & result COM object pointer
	*/
	if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device))))
		std::exit(-1);

	/* [notes]

	* queueDesc: command queue description
	* queueDesc.Type: type of command list
	* queueDesc.Priority: priority of command list
			* app is game, use D3D12_COMMAND_QUEUE_PRIORITY_HIGH
			* normal app, use D3D12_COMMAND_QUEUE_PRIORITY_NORMAL
	* queueDesc.Flags:
			* D3D12_COMMAND_QUEUE_FLAG_NONE: no flags
			* D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT:disable gpu timeout
				* don't do this, if you're task block the gpu, you're screen will black out
	
	*/
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.NodeMask = 0;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	if (FAILED(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue))))
		std::exit(-1);

	/*[notes]
	
	* create fence
		* 0: initial value of fence
		* D3D12_FENCE_FLAG_NONE: no flags

	*/
	if (FAILED(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence))))
		std::exit(-1);

	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_fenceEvent == nullptr)
		std::exit(-1);

	if (FAILED(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator))))
		std::exit(-1);

	// [ques]
	// how the command list know which allocator to use?
	if (FAILED(m_device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&m_commandList))))
		std::exit(-1);

}

void DXContext::Shutdown()
{
	m_commandList.Release();
	m_commandAllocator.Release();
	m_fence.Release();
	m_device.Release();
	m_factory.Release();
	if (m_fenceEvent) CloseHandle(m_fenceEvent);
}

ID3D12GraphicsCommandList7* DXContext::InitCommandList()
{
	m_commandAllocator->Reset();
	m_commandList->Reset(m_commandAllocator, nullptr);
	return m_commandList;
}

void DXContext::ExecuteCommandList()
{
	if (FAILED(m_commandList->Close()))
		std::exit(-1);
	ID3D12CommandList* lists[] = {m_commandList};
	m_commandQueue->ExecuteCommandLists(1, lists);
	SignalAndWait();
}

bool DXContext::SignalAndWait()
{
	if (FAILED(m_commandQueue->Signal(m_fence, ++m_fenceValue)))
		std::exit(-1);
	if (FAILED(m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent)))
		std::exit(-1);
	if (WaitForSingleObject(m_fenceEvent, 20000) != WAIT_OBJECT_0)
		std::exit(-1);
	return true;
}
