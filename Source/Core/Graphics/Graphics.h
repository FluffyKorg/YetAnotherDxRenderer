#pragma once

#include <UploadBuffer.h>
#include "../../Core/Window/Window.h"
#include "../../Platform/Windows/Win32Window.h"
#include <DirectXMath.h> // in Math helper?

static DirectX::XMFLOAT4X4 Identity4x4() {
	static DirectX::XMFLOAT4X4 I(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	return I;
}

struct Vertex {
    DirectX::XMFLOAT3 Pos;
    DirectX::XMFLOAT4 Color;
};

struct ObjectConstants {
    DirectX::XMFLOAT4X4 WorldViewProj = Identity4x4();
};

class Graphics {
public:
    Graphics(Window* wnd);
    ~Graphics() = default;

    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;
    Graphics(Graphics&&) = delete;
    Graphics& operator=(Graphics&&) = delete;

	void Update(float32 deltaTime);
    void DrawFrame();
    
    void OnMouseDown(MouseButton button, int32 x, int32 y);
    void OnMouseUp(MouseButton button, int32 x, int32 y);
    void OnMouseMove(int32 x, int32 y);

private:
	void OnResize();
	void FlushCommandQueue();

    ID3D12Resource* CurrentBackBuffer() const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;
    D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;

	void BuildBoxGeometry();
    void BuildPSO();


private:
	Window* m_window = nullptr;

	UniquePtr<UploadBuffer<ObjectConstants>> m_objectCB = nullptr;

	UniquePtr<MeshGeometry> m_boxGeo = nullptr;

	int m_currBackBuffer = 0;
    static const int m_swapChainBufferCount = 2;
    ComPtr<ID3D12Resource> m_swapChainBuffer[m_swapChainBufferCount];
    ComPtr<ID3D12Resource> m_depthStencilBuffer;

	ComPtr<ID3D12PipelineState> m_PSO = nullptr;

    // Core D3D12 objects
    ComPtr<ID3D12Device> m_device;
    ComPtr<IDXGISwapChain> m_swapChain;
    ComPtr<IDXGIFactory4> m_dxgiFactory;

    ComPtr<ID3D12CommandQueue> m_commandQueue;
    ComPtr<ID3D12CommandAllocator> m_directCmdListAlloc;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;

    ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    ComPtr<ID3D12DescriptorHeap> m_dsvHeap;

    ComPtr<ID3D12RootSignature> m_rootSignature = nullptr;
    ComPtr<ID3D12DescriptorHeap> m_cbvHeap = nullptr;

	// Set true to use 4X MSAA (�4.1.8).  The default is false.
    bool m_4xMsaaState = false;    // 4X MSAA enabled
    UINT m_4xMsaaQuality = 0;      // quality level of 4X MSAA

	UINT m_rtvDescriptorSize = 0;
	UINT m_dsvDescriptorSize = 0;
	UINT m_cbvSrvUavDescriptorSize = 0;

    ComPtr<ID3DBlob> m_vsByteCode = nullptr;
    ComPtr<ID3DBlob> m_psByteCode = nullptr;
    std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputLayout;

    D3D12_VIEWPORT m_screenViewport;
    D3D12_RECT m_scissorRect;

    ComPtr<ID3D12Fence> m_fence;
    UINT64 m_currentFence = 0;

    DXGI_FORMAT m_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    DXGI_FORMAT m_depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    DirectX::XMFLOAT4X4 mWorld = Identity4x4();
    DirectX::XMFLOAT4X4 mView = Identity4x4();
    DirectX::XMFLOAT4X4 mProj = Identity4x4();

    float mTheta = 1.5f*DirectX::XM_PI;
    float mPhi = DirectX::XM_PIDIV4;
    float mRadius = 5.0f;

    POINT m_lastMousePos;

};
