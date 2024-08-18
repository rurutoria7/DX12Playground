#include <DebugLayer/DXDebugLayer.h>
#include <D3D/DXContext.h>
#include <Support/MyWindow.h>
#include <iostream>
#include <Support/Shader.h>



struct RenderOrder
{
    ComPointer<ID3D12Resource2> m_uploadRes = nullptr;
    ComPointer<ID3D12Resource2> m_defaultRes = nullptr;
    ComPointer<ID3D12PipelineState> m_pso = nullptr;
    ComPointer<ID3D12RootSignature> m_rootSiganature = nullptr;
    D3D12_VERTEX_BUFFER_VIEW m_vbv{};


    D3D12_INPUT_ELEMENT_DESC  m_layout[1] = {
        {"aPos", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
    };

    void init()
    {
        bufferVerticesToGPU(m_uploadRes, m_defaultRes);

        Shader vertexShader("VertexShader.cso");
        Shader pixelShader("PixelShader.cso");
        Shader rootSignatureShader("RootSignature.cso");

        DXContext::Get().GetDevice()->CreateRootSignature(
            0, rootSignatureShader.GetBuffer(),
            rootSignatureShader.GetSize(), IID_PPV_ARGS(&m_rootSiganature));

        D3D12_GRAPHICS_PIPELINE_STATE_DESC gfxPSOd;
        gfxPSOd.pRootSignature = m_rootSiganature;
        gfxPSOd.InputLayout.NumElements = _countof(m_layout);
        gfxPSOd.InputLayout.pInputElementDescs = m_layout;
        gfxPSOd.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
        gfxPSOd.VS.pShaderBytecode = vertexShader.GetBuffer();
        gfxPSOd.VS.BytecodeLength = vertexShader.GetSize();
        gfxPSOd.PS.pShaderBytecode = pixelShader.GetBuffer();
        gfxPSOd.PS.BytecodeLength = pixelShader.GetSize();
        gfxPSOd.DS.pShaderBytecode = nullptr;
        gfxPSOd.DS.BytecodeLength = 0;
        gfxPSOd.HS.pShaderBytecode = nullptr;
        gfxPSOd.HS.BytecodeLength = 0;
        gfxPSOd.GS.pShaderBytecode = nullptr;
        gfxPSOd.GS.BytecodeLength = 0;
        gfxPSOd.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
        gfxPSOd.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
        gfxPSOd.RasterizerState.FrontCounterClockwise = false;
        gfxPSOd.RasterizerState.DepthBias = 0;
        gfxPSOd.RasterizerState.DepthBiasClamp = .0f;
        gfxPSOd.RasterizerState.SlopeScaledDepthBias = 0.f;
        gfxPSOd.RasterizerState.DepthClipEnable = false;
        gfxPSOd.RasterizerState.MultisampleEnable = false;
        gfxPSOd.RasterizerState.AntialiasedLineEnable = false;
        gfxPSOd.RasterizerState.ForcedSampleCount = 0;
        gfxPSOd.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

        gfxPSOd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        gfxPSOd.StreamOutput.NumEntries = 0;
        gfxPSOd.StreamOutput.pSODeclaration = nullptr;
        gfxPSOd.StreamOutput.pBufferStrides = nullptr;
        gfxPSOd.StreamOutput.NumStrides = 0;
        gfxPSOd.StreamOutput.RasterizedStream = 0;
        gfxPSOd.NodeMask = 0;
        gfxPSOd.CachedPSO.pCachedBlob = nullptr;
        gfxPSOd.CachedPSO.CachedBlobSizeInBytes = 0;
        gfxPSOd.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
        gfxPSOd.NumRenderTargets = 1;
        gfxPSOd.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        gfxPSOd.RTVFormats[1] = DXGI_FORMAT_UNKNOWN;
        gfxPSOd.RTVFormats[2] = DXGI_FORMAT_UNKNOWN;
        gfxPSOd.RTVFormats[3] = DXGI_FORMAT_UNKNOWN;
        gfxPSOd.RTVFormats[4] = DXGI_FORMAT_UNKNOWN;
        gfxPSOd.RTVFormats[5] = DXGI_FORMAT_UNKNOWN;
        gfxPSOd.RTVFormats[6] = DXGI_FORMAT_UNKNOWN;
        gfxPSOd.RTVFormats[7] = DXGI_FORMAT_UNKNOWN;
        gfxPSOd.DSVFormat = DXGI_FORMAT_UNKNOWN;
        gfxPSOd.SampleDesc.Count = 1;
        gfxPSOd.BlendState.AlphaToCoverageEnable = false;
        gfxPSOd.BlendState.IndependentBlendEnable = false;
        gfxPSOd.BlendState.RenderTarget[0].BlendEnable = false;
        gfxPSOd.BlendState.RenderTarget[0].LogicOpEnable = false;
        gfxPSOd.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
        gfxPSOd.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
        gfxPSOd.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
        gfxPSOd.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ZERO;
        gfxPSOd.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
        gfxPSOd.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
        gfxPSOd.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
        gfxPSOd.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        gfxPSOd.DepthStencilState.DepthEnable = false;
        gfxPSOd.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
        gfxPSOd.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        gfxPSOd.DepthStencilState.StencilEnable = false;
        gfxPSOd.DepthStencilState.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
        gfxPSOd.DepthStencilState.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
        gfxPSOd.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
        gfxPSOd.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
        gfxPSOd.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
        gfxPSOd.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        gfxPSOd.DepthStencilState.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
        gfxPSOd.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
        gfxPSOd.DepthStencilState.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
        gfxPSOd.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        gfxPSOd.SampleMask = 0xFFFFFFFF;
        gfxPSOd.SampleDesc.Count = 1;
        gfxPSOd.SampleDesc.Quality = 0;

        DXContext::Get().GetDevice()->CreateGraphicsPipelineState(&gfxPSOd, IID_PPV_ARGS(&m_pso));

        m_vbv.BufferLocation = m_defaultRes->GetGPUVirtualAddress();
        m_vbv.SizeInBytes = sizeof(float) * 3 * 3;
        m_vbv.StrideInBytes = sizeof(float) * 3;
    } 

    void update()
    {

    }

    void render(auto cmdList)
    {
        clearRTV(cmdList);
        drawTriangle(cmdList);
    }
    void drawTriangle(ID3D12GraphicsCommandList7* OUT cmdList)
    {
        cmdList->SetPipelineState(m_pso);
        cmdList->SetGraphicsRootSignature(m_rootSiganature);
        cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmdList->IASetVertexBuffers(0, 1, &m_vbv);

        D3D12_VIEWPORT vp;
        vp.TopLeftX = vp.TopLeftY = 0;
        vp.Width = MyWindow::Get().GetWidth();
        vp.Height = MyWindow::Get().GetHeight();
        vp.MinDepth = 1.f;
        vp.MaxDepth = 0.f;
        cmdList->RSSetViewports(1, &vp);

        RECT scRect;
        scRect.left = scRect.top = 0;
        scRect.right = MyWindow::Get().GetWidth();
        scRect.bottom = MyWindow::Get().GetHeight();
        cmdList->RSSetScissorRects(1, &scRect);

        cmdList->DrawInstanced(3, 1, 0, 0);
    }
    void bufferVerticesToGPU(ComPointer<ID3D12Resource2> OUT& uploadRes, ComPointer<ID3D12Resource2> OUT& defaultRes)
    {
        struct Vertex
        {
            float x, y, z;
        };
        Vertex vertices[] = {
            {0.0f, 0.5f, 0.0f},
            {0.5f, -0.5f, 0.0f},
            {-0.5f, -0.5f, 0.0f}
        };

        auto device = DXContext::Get().GetDevice();

        D3D12_HEAP_PROPERTIES hpUpload{};
        hpUpload.Type = D3D12_HEAP_TYPE_UPLOAD;
        hpUpload.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        hpUpload.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        hpUpload.CreationNodeMask = 0;
        hpUpload.VisibleNodeMask = 0;

        D3D12_HEAP_PROPERTIES hpDefault{};
        hpDefault.Type = D3D12_HEAP_TYPE_DEFAULT;
        hpDefault.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        hpDefault.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        hpDefault.CreationNodeMask = 0;
        hpDefault.VisibleNodeMask = 0;

        D3D12_RESOURCE_DESC resDesc{};
        resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resDesc.Alignment = 0;
        resDesc.Width = sizeof(vertices);
        resDesc.Height = 1;
        resDesc.DepthOrArraySize = 1;
        resDesc.MipLevels = 1;
        resDesc.Format = DXGI_FORMAT_UNKNOWN;
        resDesc.SampleDesc.Count = 1;
        resDesc.SampleDesc.Quality = 0;
        resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

        device->CreateCommittedResource(&hpUpload, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&uploadRes));
        device->CreateCommittedResource(&hpDefault, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&defaultRes));

        D3D12_RANGE range{};
        range.Begin = 0;
        range.End = sizeof(vertices);
        void* mapped;
        uploadRes->Map(0, &range, &mapped);
        memcpy(mapped, vertices, sizeof(vertices));

        auto cmdList = DXContext::Get().InitCommandList();
        cmdList->CopyBufferRegion(defaultRes, 0, uploadRes, 0, sizeof(vertices));
        DXContext::Get().ExecuteCommandList();
    }
    void clearRTV(ID3D12GraphicsCommandList7* OUT cmdList)
    {
        int backBuffer = MyWindow::Get().GetCurrentBackBuffer();
        auto backBufferHandle = DXContext::Get().GetRTVHeapHandle()[backBuffer];
        float clearColor[] = { 1.0f, 0.0f, 0.0f, 1.0f };
        cmdList->ClearRenderTargetView(backBufferHandle, clearColor, 0, nullptr);
        cmdList->OMSetRenderTargets(1, &backBufferHandle, false, nullptr);
    }
} ro;

int main()
{
    DXDebugLayer::Get().Init();
    DXContext::Get().Init();
    MyWindow::Get().Init();

    ro.init();

    while (!MyWindow::Get().ShouldClose())
    {
        MyWindow::Get().Update();

        auto cmdList = DXContext::Get().InitCommandList();
        MyWindow::Get().SetBackBufferStateRT(cmdList);

        ro.render(cmdList);

        MyWindow::Get().SetBackBufferStatePRESENT(cmdList);
        DXContext::Get().ExecuteCommandList();
        MyWindow::Get().Present();
    }

    DXContext::Get().Flush(1);
    MyWindow::Get().Shutdown();
    DXContext::Get().Shutdown();

    DXDebugLayer::Get().Shutdown(); // report com objects
}