#pragma once
#include<dxgi1_6.h>																							
#include <wrl.h>
#include <d3d12.h>

#include "../TestFramew/pch.h"

static  constexpr const UINT c_frameCount = 3;

class WDResources  
{
public:
	UINT64 currentframebackbufferindex;
	void WaitForGPU();
	WDResources(UINT arg_width, UINT arg_height);
	WDResources(WDResources& obj) = delete;
	WDResources(WDResources&& obj) = delete;
	void CreatingSettingDeviceResource();
	void CraeteRenderResources();
	
	Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() {
		HRESULT whr;
		DXCall(whr = m_device->GetDeviceRemovedReason());
		
		return m_device.Get(); };
	//Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> GetCmdList() { return m_commandList.Get(); };
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetCmdQueue() { return m_commandQueue.Get(); };
	Microsoft::WRL::ComPtr<ID3D12Resource> GetRenderTargetView() { return m_renderTargets[currentframebackbufferindex].Get(); };
	const D3D12_CPU_DESCRIPTOR_HANDLE CurrentRTVBackBufferHandle() {	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
																	m_rtvheap->GetCPUDescriptorHandleForHeapStart(),
																 	currentframebackbufferindex,
																	m_rtvDescreptorSize);
	}



	D3D12_CPU_DESCRIPTOR_HANDLE CurrentDSVHandle() { return m_dsvheap->GetCPUDescriptorHandleForHeapStart(); }
	HWND GetHwnd() { return m_hwnd; }
	Microsoft::WRL::ComPtr<IDXGISwapChain4> GetSwapChain() { return m_swapChain.Get(); }
	
	UINT GetWidth() { return m_Width; }
	UINT GetHeight() { return m_Height; }
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator>			m_commandAllocator[2];
	UINT													m_rtvDescreptorSize;

	auto  GetSwapChain(Microsoft::WRL::ComPtr<IDXGISwapChain4>	arg_swapChain) -> Microsoft::WRL::ComPtr<IDXGISwapChain4>;
	void  GetHardwareAdapter(IDXGIAdapter4** ppAdapter );
	void  GetHardwareAdapter2(IDXGIFactory1* pFactory,
		IDXGIAdapter1** ppAdapter,
		bool requestHighPerformanceAdapter);
	void GetHardwareAdapter1(IDXGIFactory1* pFactory, IDXGIAdapter4** ppAdapter, bool requestHighPerformanceAdapter);
	//Implementation of this function can be found in- https://github.com/walbourn/directx-vs-templates/blob/343fca39c72ee61dd43ee9a112996986b7b1f61c/d3d12game_win32_dr/DeviceResources.cpp#L640
	void GetAdapter(IDXGIAdapter1 ** ppAdapter);

	Microsoft::WRL::ComPtr<ID3D12Device>					m_device;

	Microsoft::WRL::ComPtr<IDXGISwapChain4>					m_swapChain;

	Microsoft::WRL::ComPtr<ID3D12Resource2>					m_renderTargets[c_frameCount];

	Microsoft::WRL::ComPtr<ID3D12Resource2>					m_depthStencilView;


	Microsoft::WRL::ComPtr<ID3D12CommandQueue>				m_commandQueue;
	 
	//Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4>		m_commandList;

	Microsoft::WRL::ComPtr<IDXGIFactory4>					m_factory;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>            m_rtvheap;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>            m_dsvheap;

	Microsoft::WRL::ComPtr<ID3D12Fence1>					m_fence;

	HANDLE                                                  m_fenceevent;
	UINT                                                    m_fencevalues[c_frameCount];
	UINT													m_dsvDescreptorSize;

	UINT													m_Width;
	UINT													m_Height;
	HWND													m_hwnd;

	D3D_FEATURE_LEVEL m_featureLevel = D3D_FEATURE_LEVEL_11_0;


};