#include "Res.h"
#include"pch.h"
#include"DebugResources.h"


using namespace Microsoft::WRL;

void WDResources::WaitForGPU()
{
	currentframebackbufferindex = m_swapChain->GetCurrentBackBufferIndex();
	// After GPU Execute It's Work ,Set The FenceValue to fenc's FenceValue.
DB::ThrowIfFailed(	m_commandQueue->Signal(m_fence.Get(), m_fencevalues[currentframebackbufferindex]));
	
		//Signal The Event if the fence value is been set(by Signal func).
DB::ThrowIfFailed(	m_fence->SetEventOnCompletion(m_fencevalues[currentframebackbufferindex], m_fenceevent));
	   //Wait for the Event to be singnaled
	WaitForSingleObjectEx(m_fenceevent, INFINITE, FALSE);

		
	m_fencevalues[currentframebackbufferindex]++;

	
}

WDResources::WDResources(UINT arg_width, UINT arg_height) : m_Width(arg_width),m_Height(arg_height), m_fenceevent(NULL),
m_fencevalues{},
m_dsvDescreptorSize(0),
m_rtvDescreptorSize(0),
m_device(nullptr),
m_swapChain(nullptr),
m_renderTargets{},
m_depthStencilView(nullptr),
m_commandAllocator{},
m_commandQueue(nullptr),
//m_commandList(nullptr),
m_factory(nullptr),
m_rtvheap(nullptr),
m_dsvheap(nullptr),
m_fence(nullptr)
{
	CreatingSettingDeviceResource();
	CraeteRenderResources();
}

void WDResources::CreatingSettingDeviceResource()
{

       UINT createFactoryFlags = 0x00;
#if defined(_DEBUG)
	{
		ComPtr<ID3D12Debug> m_debugController;

		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&m_debugController))))
		{
			m_debugController->EnableDebugLayer();
		}
	}
   createFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
  
#endif // defined(_DEBUG)

  // ComPtr<IDXGIFactory4> factory;
   DB::ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&m_factory)));


	ComPtr<IDXGIAdapter1> adapter;


	ComPtr<IDXGIAdapter1> hardwareAdapter;
	GetHardwareAdapter2(m_factory.Get(), &hardwareAdapter,true);
	DXGI_ADAPTER_DESC1 desc;
	hardwareAdapter->GetDesc1(&desc);
	DB::ThrowIfFailed(D3D12CreateDevice(
		hardwareAdapter.Get(),
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&m_device)
	));

	/*GetAdapter(adapter.GetAddressOf());
	DB::ThrowIfFailed(D3D12CreateDevice(adapter.Get(), m_featureLevel, IID_PPV_ARGS(&m_device)));
	*/
	static const D3D_FEATURE_LEVEL s_featureLevels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D12_FEATURE_DATA_FEATURE_LEVELS featLevels =
	{
		_countof(s_featureLevels), s_featureLevels, m_featureLevel
	};

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	HRESULT hr_ = m_device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS,
		&featLevels, sizeof(featLevels));
	DB::ThrowIfFailed(hr_);
	if (SUCCEEDED(hr_))
	{
		featureLevel = featLevels.MaxSupportedFeatureLevel;
	}

#if defined(_DEBUG)
	//If the above initialization fails use WARP adapters
	//if (FAILED(hr) ||
	//	(m_featureLevel != D3D_FEATURE_LEVEL_11_1))
	//{
	//	ComPtr<IDXGIAdapter4> warpAdapter;
	//	DB::ThrowIfFailed(m_factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));
	//	hr = D3D12CreateDevice(warpAdapter.Get(), m_featureLevel, IID_PPV_ARGS(&m_device));
	//}
	ComPtr<ID3D12InfoQueue> pInfoQueue;

	m_device->QueryInterface(__uuidof(ID3D12InfoQueue), &pInfoQueue);
	pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
	pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
	pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
	pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_INFO, true);
	pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_MESSAGE, true);
#endif // defined(_DEBUG)

	//DB::ThrowIfFailed(hr);
	HRESULT whr;
	DXCall(whr = m_device->GetDeviceRemovedReason());

	//Create the command queue
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	DB::ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));
	DebugName(m_commandQueue);

	DXCall(whr = m_device->GetDeviceRemovedReason());
	
	
	
	
	D3D12_DESCRIPTOR_HEAP_DESC rtvdesc;
	rtvdesc.NumDescriptors = 3;
	rtvdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvdesc.NodeMask = 0;
HRESULT hr =      m_device->CreateDescriptorHeap(&rtvdesc, IID_PPV_ARGS(&m_rtvheap));

	DebugName(m_rtvheap);

	m_rtvDescreptorSize =	m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	
	D3D12_DESCRIPTOR_HEAP_DESC dsvdesc;
	dsvdesc.NumDescriptors = 1;
	dsvdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvdesc.NodeMask = 0;
	DB::ThrowIfFailed(m_device->CreateDescriptorHeap(&dsvdesc, IID_PPV_ARGS(&m_dsvheap)));
	DebugName(m_dsvheap);
	m_dsvDescreptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	for (int i = 0; i < 2; i++)
	{
		DB::ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator[i])));
	}
	DXCall(whr = m_device->GetDeviceRemovedReason());
	m_swapChain = GetSwapChain(m_swapChain);

	

	m_device->CreateFence(m_fencevalues[m_swapChain->GetCurrentBackBufferIndex()], D3D12_FENCE_FLAG_SHARED, IID_PPV_ARGS(&m_fence));
	m_fencevalues[m_swapChain->GetCurrentBackBufferIndex()]++;

	m_fenceevent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
	DXCall(whr = m_device->GetDeviceRemovedReason());

	
}

void WDResources::CraeteRenderResources()
{
	//Create Swap Chain //alternatively you can say inizilatizw swap chain
	{
	
	}

	//Creating RenderTargetView
	{
		currentframebackbufferindex = m_swapChain->GetCurrentBackBufferIndex();
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Buffer.FirstElement = 0;
		rtvDesc.Buffer.NumElements = 3;
		
		for (UINT i{ 0 }; i < c_frameCount; i++)
		{
			DB::ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i])));
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvheap->GetCPUDescriptorHandleForHeapStart());

			rtvHandle.Offset(i,m_rtvDescreptorSize);
			m_device->CreateRenderTargetView(m_renderTargets[i].Get(),&rtvDesc, rtvHandle);
			//This is for setting Descriptor in Diffrent Memory Bloclks.
			/*The very first descriptor we can put in the heap start but for the next descriptor
			we need to offset position by the size of the descriptor. 
			And finally we’re asking the device to create specified descriptor in specified place */
				
		//	rtvHandle.ptr += m_rtvDescreptorSize * i ;			
		}
	
	}
	//Create Depth-Stencil View
	{
		D3D12_RESOURCE_DESC depthStencilDesc;
		depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthStencilDesc.Alignment = 0;
		depthStencilDesc.Width = m_Width;
		depthStencilDesc.Height = m_Height;
		depthStencilDesc.DepthOrArraySize = 1;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality =  0;
		depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		D3D12_CLEAR_VALUE optClear;
		optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		optClear.DepthStencil.Depth = 1.0f;
		optClear.DepthStencil.Stencil = 0;
		CD3DX12_HEAP_PROPERTIES myHeapPropertyPtr = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		DXCall(m_device->CreateCommittedResource(
			&myHeapPropertyPtr,
			D3D12_HEAP_FLAG_NONE,
			&depthStencilDesc,
			D3D12_RESOURCE_STATE_COMMON,
			&optClear,
			IID_PPV_ARGS(m_depthStencilView.GetAddressOf())));
		
		
		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_dsvheap->GetCPUDescriptorHandleForHeapStart());
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.Texture2D.MipSlice = 0;
		
		m_device->CreateDepthStencilView(m_depthStencilView.Get(),nullptr, dsvHandle);
	}

	
}

auto WDResources::GetSwapChain(Microsoft::WRL::ComPtr<IDXGISwapChain4> arg_swapChain) -> Microsoft::WRL::ComPtr<IDXGISwapChain4>
{
	
		Microsoft::WRL::ComPtr	<IDXGISwapChain1> dummy_SwapChain;
		//DXGI_SWAP_CHAIN_DESC swapchaindesc{ 0 };
		//ZeroMemory(&swapchaindesc, sizeof(DXGI_SWAP_CHAIN_DESC));
		//swapchaindesc.BufferCount = c_frameCount;
		/*
		The default sampler mode, with no anti-aliasing, has a count of 1 and a quality level of 0.
		*/
		//swapchaindesc.SampleDesc.Count = 1;
		//swapchaindesc.SampleDesc.Quality = 0;
		//swapchaindesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		//swapchaindesc.Stereo = FALSE;
		//swapchaindesc.Scaling = DXGI_SCALING_NONE;
		//swapchaindesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
		//swapchaindesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		//swapchaindesc.Flags = (DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);
		//swapchaindesc.Width = m_Width;
		//swapchaindesc.Height = m_Height;
		//swapchaindesc.BufferCount = c_frameCount;
		//DB::ThrowIfFailed(m_factory->CreateSwapChainForComposition(m_commandQueue.Get(), &swapchaindesc, nullptr, &dummy_SwapChain));


	// Describe and create the swap chain.
	//	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	//	swapChainDesc.BufferCount = c_frameCount;
	//	swapChainDesc.Width = m_Width;
	//	swapChainDesc.Height = m_Height;
	//	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	//	swapChainDesc.SampleDesc.Count = 1;

	////	DB::ThrowIfFailed();

	
		
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = c_frameCount;
		swapChainDesc.Width = m_Width;
		swapChainDesc.Height = m_Height;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;

		ComPtr<IDXGISwapChain1> swapChain;
		DB::ThrowIfFailed(m_factory->CreateSwapChainForComposition(
			m_commandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
			&swapChainDesc,
			nullptr,
			&dummy_SwapChain
		));

		DB::ThrowIfFailed(dummy_SwapChain.As(&arg_swapChain));
		dummy_SwapChain->Release();
	
	
	return arg_swapChain.Detach();
}
		
void WDResources::GetHardwareAdapter(IDXGIAdapter4** ppAdapter)
{
	ComPtr<IDXGIAdapter1> adapter;
	ComPtr<IDXGIAdapter4> adapter4;
	*ppAdapter = nullptr;

	for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != m_factory->EnumAdapters1(adapterIndex, &adapter); adapterIndex++)
	{

		//Creating "DXGI_ADAPTER_DESC3" Instance to get "adapter" descriptor so we can do conditional check
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);
		// This check do we cannot selkkect Default Microsoft Basic  Render Driver adapter.
		if ((desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0) {
			continue;
		}

		// Check to see if the adapter supports Direct3D 12,
		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), m_featureLevel, __uuidof(ID3D12Device9), nullptr)))
		{


			adapter4.Attach((IDXGIAdapter4*)adapter.Get());
			*ppAdapter = adapter4.Detach();
			break;
		}

	}
	
}






_Use_decl_annotations_
void  WDResources::GetHardwareAdapter1(
		IDXGIFactory1 * pFactory,
		IDXGIAdapter4 * *ppAdapter,
		bool requestHighPerformanceAdapter)

{
		*ppAdapter = nullptr;

		ComPtr<IDXGIAdapter1> adapter;
		ComPtr<IDXGIAdapter4> adapter4;

		ComPtr<IDXGIFactory6> factory6;
		if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
		{
			for (
				UINT adapterIndex = 0;
				SUCCEEDED(factory6->EnumAdapterByGpuPreference(
					adapterIndex,
					requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
					IID_PPV_ARGS(&adapter)));
				++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					// Don't select the Basic Render Driver adapter.
					// If you want a software adapter, pass in "/warp" on the command line.
					continue;
				}

				// Check to see whether the adapter supports Direct3D 12, but don't create the
				// actual device yet.
				if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), m_featureLevel, _uuidof(ID3D12Device9), nullptr)))
				{
					break;
				}
			}
		}

		if (adapter.Get() == nullptr)
		{
			for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					// Don't select the Basic Render Driver adapter.
					// If you want a software adapter, pass in "/warp" on the command line.
					continue;
				}

				// Check to see whether the adapter supports Direct3D 12, but don't create the
				// actual device yet.
				if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), m_featureLevel, _uuidof(ID3D12Device9), nullptr)))
				{
					break;
				}
			}
		}


		//adapter4.Attach((IDXGIAdapter4*)adapter.Get());
		DB::ThrowIfFailed(adapter.As(&adapter4));
		*ppAdapter = adapter4.Get();
}





void WDResources::GetHardwareAdapter2(
	IDXGIFactory1* pFactory,
	IDXGIAdapter1** ppAdapter,
	bool requestHighPerformanceAdapter)
{
	*ppAdapter = nullptr;

	ComPtr<IDXGIAdapter1> adapter;

	ComPtr<IDXGIFactory6> factory6;
	DB::ThrowIfFailed(pFactory->QueryInterface(IID_PPV_ARGS(&factory6)));
	HRESULT hr;
	if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
	{
		for (
			UINT adapterIndex = 0; 
			 hr = (factory6->EnumAdapterByGpuPreference(
				adapterIndex,
				requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
				IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND);
			++adapterIndex) 
		{
			DB::ThrowIfFailed(hr);
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				// If you want a software adapter, pass in "/warp" on the command line.
				continue;
			}

		}
	}
	DB::ThrowIfFailed(hr);

	if (adapter.Get() == nullptr)
	{
		for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				// If you want a software adapter, pass in "/warp" on the command line.
				continue;
			}

			// Check to see whether the adapter supports Direct3D 12, but don't create the
			// actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}
	}

	*ppAdapter = adapter.Detach();
}

void WDResources::GetAdapter(IDXGIAdapter1** ppAdapter)
{

	*ppAdapter = nullptr;

	ComPtr<IDXGIAdapter1> adapter;

	ComPtr<IDXGIFactory6> factory6;
	HRESULT hr = m_factory.As(&factory6);
	if (SUCCEEDED(hr))
	{
		for (UINT adapterIndex = 0;
			SUCCEEDED(factory6->EnumAdapterByGpuPreference(
				adapterIndex,
				DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
				IID_PPV_ARGS(adapter.ReleaseAndGetAddressOf())));
			adapterIndex++)
		{
			DXGI_ADAPTER_DESC1 desc;
			DB::ThrowIfFailed(adapter->GetDesc1(&desc));

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				continue;
			}

			// Check to see if the adapter supports Direct3D 12, but don't create the actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), m_featureLevel, __uuidof(ID3D12Device), nullptr)))
			{
#ifdef _DEBUG
				wchar_t buff[256] = {};
				swprintf_s(buff, L"Direct3D Adapter (%u): VID:%04X, PID:%04X - %ls\n", adapterIndex, desc.VendorId, desc.DeviceId, desc.Description);
				OutputDebugStringW(buff);
#endif
				break;
			}
		}
	}

	if (!adapter)
	{
		for (UINT adapterIndex = 0;
			SUCCEEDED(m_factory->EnumAdapters1(
				adapterIndex,
				adapter.ReleaseAndGetAddressOf()));
			++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 desc;
	DB::ThrowIfFailed(adapter->GetDesc1(&desc));

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				continue;
			}

			// Check to see if the adapter supports Direct3D 12, but don't create the actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), m_featureLevel, __uuidof(ID3D12Device), nullptr)))
			{
#ifdef _DEBUG
				wchar_t buff[256] = {};
				swprintf_s(buff, L"Direct3D Adapter (%u): VID:%04X, PID:%04X - %ls\n", adapterIndex, desc.VendorId, desc.DeviceId, desc.Description);
				OutputDebugStringW(buff);
#endif
				break;
			}
		}
	}

#if !defined(NDEBUG)
	if (!adapter)
	{
		// Try WARP12 instead
		if (FAILED(m_factory->EnumWarpAdapter(IID_PPV_ARGS(adapter.ReleaseAndGetAddressOf()))))
		{
			throw std::runtime_error("WARP12 not available. Enable the 'Graphics Tools' optional feature");
		}

		OutputDebugStringA("Direct3D Adapter - WARP12\n");
	}
#endif

	if (!adapter)
	{
		throw std::runtime_error("No Direct3D 12 device found");
	}

	*ppAdapter = adapter.Detach();
}
