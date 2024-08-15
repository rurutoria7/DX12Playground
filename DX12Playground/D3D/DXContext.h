#pragma once
#include <cstdlib>
#include <Support/Compointer.h>
#include <DebugLayer/DXDebugLayer.h>

class DXContext
{
public:
	void Init();
	void Shutdown();

	inline ComPointer<ID3D12Device10>& GetDevice() { return m_device; }
	inline ComPointer<ID3D12CommandQueue>& GetCommandQueue() { return m_commandQueue; }
	inline ComPointer<ID3D12Fence1>& GetFence() { return m_fence; }
	inline ComPointer<IDXGIFactory7>& GetFactory() { return m_factory; }
	inline void Flush(size_t cnt)
	{
		for (size_t i = 0; i < cnt; i++)
			SignalAndWait();
	}

	ID3D12GraphicsCommandList7* InitCommandList();
	void ExecuteCommandList();

	bool SignalAndWait();
	UINT64 m_fenceValue = 0;

private:
	ComPointer<ID3D12Device10> m_device;
	ComPointer<ID3D12CommandQueue> m_commandQueue;
	ComPointer<ID3D12Fence1> m_fence;
	ComPointer<ID3D12CommandAllocator> m_commandAllocator;
	ComPointer<ID3D12GraphicsCommandList7> m_commandList;
	ComPointer<IDXGIFactory7> m_factory;
	HANDLE m_fenceEvent = nullptr;

public:
	DXContext(const DXContext&) = delete;
	DXContext& operator=(const DXContext&) = delete;
	static DXContext& Get()
	{
		static DXContext instance;
		return instance;
	}

private:
	DXContext() = default;
};

