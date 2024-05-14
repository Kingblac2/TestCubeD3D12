#include "pch.h"
#include "DebugResources.h"
#include "ExecuteResources.h"
#include  <dxcapi.h>
using namespace DirectX;

//RootDesc::RootDesc(UINT NumParameters, D3D12_STATIC_SAMPLER_DESC staticsampler[], UINT NumOfStaticSamplers, D3D12_ROOT_SIGNATURE_FLAGS Flags)
//{
//	Signature_Initialize(NumParameters, staticsampler, NumOfStaticSamplers);
//}
ExecuteResources::ExecuteResources(UINT Width,UINT Height) : m_res(new WDResources(Width,Height)),m_rootsignature(nullptr),m_pipelineState(nullptr),m_cbvHeap(nullptr),m_vertexBuffer(nullptr),m_constantBuffer(nullptr),m_indexBuffer(nullptr),m_errorblob(nullptr),m_commandList()
{
//  DB::ThrowIfFailed(m_commandList->Reset(m_res->m_commandAllocator->Get(),m_pipelineState.Get() ));
	ResourceLoading();
	GPUResourceLoading();
	Draw();
	
	
		
	

	//m_res->GetCmdQueue()->ExecuteCommandLists(1, (ID3D12CommandList* const*)(m_commandList.GetAddressOf()));
}
ExecuteResources::~ExecuteResources()
{
	 m_res.~shared_ptr();
	 m_rootsignature->Release();
	 m_pipelineState->Release();
	 m_cbvHeap->Release();
	 m_vertexBuffer->Release();
	 m_constantBuffer->Release();
	 m_indexBuffer->Release();
	 m_blob->Release();
	 if (m_errorblob != nullptr)
	 {
		 m_errorblob->Release();

	 }
//	 m_commandList->Release();


}
void ExecuteResources::ResourceLoading()
{
	//Create a root Signature
	{
	
		//D3DCompile2()
		struct CD3DX12_STATIC_SAMPLER_DESC staticSampler {};
		staticSampler.Init(0);
//		RootDesc rootdescriptor;
	
	//	rootdescriptor.as_uav(D3D12_SHADER_VISIBILITY_ALL, 0u);
		//Use "sizeof(staticSampler) / sizeof(staticSampler[0])" to know number of elements in array of structs; 
		//rootdescriptor.Signature_Initialize(3, &staticSampler, 1);
		
		CD3DX12_ROOT_PARAMETER slotRootParameter[1];
		CD3DX12_ROOT_PARAMETER1 slotRootParameter1[1]{};
		CD3DX12_DESCRIPTOR_RANGE cbvTable;
		CD3DX12_DESCRIPTOR_RANGE1 cbvTable1;
		cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
		cbvTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

		//slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);
		slotRootParameter1[0].InitAsDescriptorTable(1, &cbvTable1);
		// rootSigDesc(1, slotRootParameter, 0, nullptr,
		//	D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSigDesc1(1, slotRootParameter1, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		
		

		
		//TODO: Replce With D3D12SerializeVersionedRootSignature for enabling support of (or using) D3D12_ROOT_SIGNATURE_DESC1 descriptor for our code.
		HRESULT hr = 	D3D12SerializeVersionedRootSignature(&rootSigDesc1,m_blob.GetAddressOf(), m_errorblob.GetAddressOf());
	DB::ThrowIfFailed(hr);
	
	if (m_errorblob != nullptr)
		{
			::OutputDebugStringA((char*)m_errorblob->GetBufferPointer());
		}
	HRESULT her;
	DXCall(her = m_res->GetDevice()->GetDeviceRemovedReason());
    
	DB::ThrowIfFailed(her);
	// Error: Dxgi_invalid_parameters and DXGI_ERROR_DEVICE_REMOVED.  
	// https://mwomercs.com/forums/topic/173303-dxgi-error-device/
		HRESULT here = m_res->GetDevice()->CreateRootSignature(0, m_blob->GetBufferPointer(), m_blob->GetBufferSize(), IID_PPV_ARGS(&m_rootsignature));
		DB::ThrowIfFailed(here);
	}
	/*@Name: This is the name of the macro.
	  @Definition: This is the value associated with the macro.
	  Note- 'NULL' is indcation the end of the array. 
	*/
	Microsoft::WRL::ComPtr<ID3DBlob> m_ps;
	const D3D_SHADER_MACRO defines[] =
	{
		"EXAMPLE_DEFINE", "1",
		{NULL, NULL }
	};
	{
		/*		// attempted to use DIrect Compiler DXC 
	 //Direct Compiler resources :-
	 //https://www.wihlidal.com/blog/pipeline/2018-09-16-dxil-signing-post-compile/
	 //https://github.com/microsoft/DirectXShaderCompiler/wiki/D3DCompiler-DXC-Bridge
	 //https://posts.tanki.ninja/2019/07/11/Using-DXC-In-Practice/
	 //https://learn.microsoft.com/en-us/archive/blogs/marcelolr/directx-compiler-apis
	 //https://www.reedbeta.com/blog/custom-toolchain-with-msbuild/
	 //https://simoncoenen.com/blog/programming/graphics/DxcCompiling
		const char* pSource = "VertexShader.hlsl";
	    Microsoft::WRL::ComPtr<IDxcUtils> pUtils;
        DB::ThrowIfFailed(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(pUtils.GetAddressOf())));
		Microsoft::WRL::ComPtr<IDxcBlobEncoding> pblobEncode;
		IDxcCompiler3* pCompiler;
		pUtils->CreateBlob(pSource, UINT32(sizeof(pSource) / sizeof(pSource[0])),CP_UTF8,pblobEncode.GetAddressOf());
		DxcBuffer bufftosrc;
		bufftosrc.Ptr = pblobEncode->GetBufferPointer();
		bufftosrc.Size = pblobEncode->GetBufferSize();
		bufftosrc.Encoding = 0;

		IDxcResult* temres;


		pCompiler->Compile(&bufftosrc, nullptr, NULL, nullptr, IID_PPV_ARGS(&temres));
        */

		/*@pEntrypoint This Says to (HLSL compiler) From Which Method in shader(VertexShader.hlsl) file.Starts to Execute(Compile) or Run the code.It's like our "int main()"*/
		/*@pDefines    Defines A Macro to use in hlsl file.*/
	/*	DB::ThrowIfFailed(D3DCompileFromFile(L"\\VertexShader.hlsl",defines
	   ,D3D_COMPILE_STANDARD_FILE_INCLUDE, "VSMain", "vs_4_0_level_9_1", D3DCOMPILE_ENABLE_STRICTNESS, D3DCOMPILE_EFFECT_ALLOW_SLOW_OPS,&m_blob,&m_errorblob));
	*/
		m_blob = DB::CompileShader(L"VertexShader.hlsl", defines, "VSMain", "vs_5_0");
		m_ps = DB::CompileShader(L"VertexShader.hlsl", defines, "PS", "ps_5_0");
		
		static constexpr D3D12_INPUT_ELEMENT_DESC inputLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};	

		//TODO: Create a Pipiline state in thread.
		
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
		psoDesc.pRootSignature = m_rootsignature.Get();
		psoDesc.VS = {
			reinterpret_cast<BYTE*>(m_blob->GetBufferPointer()),
			m_blob->GetBufferSize()
		};
		psoDesc.PS = {
			reinterpret_cast<BYTE*>(m_ps->GetBufferPointer()),
			m_ps->GetBufferSize()
		};
		psoDesc.NodeMask = 0;
		psoDesc.InputLayout = { inputLayout,2 };
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		psoDesc.SampleDesc.Count = 1;
		psoDesc.SampleDesc.Quality = 0;
		
		DXCall(m_res->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));
	} 
	
	
}

void ExecuteResources::GPUResourceLoading()
{
	//DB::ThrowIfFailed(m_commandList->Reset(m_res->m_commandAllocator->Get(),m_pipelineState.Get() ));
	auto d3device = m_res->GetDevice();
	for (int i = 1; i<2 ;i++)
	{
		DB::ThrowIfFailed(d3device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_res->m_commandAllocator[i].Get(), m_pipelineState.Get(), IID_PPV_ARGS(&m_commandList)));
	}

	VertexPositionColor cubeVertices[] =
	{
		{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(0.5f, -0.5f,  0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(0.5f,  0.5f, -0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(0.5f,  0.5f,  0.5f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
	};

	 const UINT vertexbuffersize = sizeof(cubeVertices);
	
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBufferUpload;
	CD3DX12_HEAP_PROPERTIES defaultHeapProperties(D3D12_HEAP_TYPE_DEFAULT);

	CD3DX12_RESOURCE_DESC vertexBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexbuffersize);

	DB::ThrowIfFailed(d3device->CreateCommittedResource(&defaultHeapProperties,D3D12_HEAP_FLAG_NONE,&vertexBufferDesc,D3D12_RESOURCE_STATE_COPY_DEST,nullptr,IID_PPV_ARGS(&m_vertexBuffer)));
	m_vertexBuffer.Get()->SetName(L"M_VERTEX BUFFER");
	
	CD3DX12_HEAP_PROPERTIES uploadHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
	DB::ThrowIfFailed(d3device->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&vertexBufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertexBufferUpload)));
	{

		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = reinterpret_cast<BYTE*>(cubeVertices);
		vertexData.RowPitch = vertexbuffersize;
		vertexData.SlicePitch = vertexData.RowPitch;
	
			UpdateSubresources(m_commandList.Get(), m_vertexBuffer.Get(), vertexBufferUpload.Get(), 0, 0, 1, &vertexData);
			
			CD3DX12_RESOURCE_BARRIER vertexBufferResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
				m_vertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
			m_commandList->ResourceBarrier(1, &vertexBufferResourceBarrier);
		
	}
	// Load mesh indices. Each trio of indices represents a triangle to be rendered on the screen.
		// For example: 0,2,1 means that the vertices with indexes 0, 2 and 1 from the vertex buffer compose the
		// first triangle of this mesh.
	unsigned short cubeIndices[] =
	{
		0, 2, 1, // -x
		1, 2, 3,

		4, 5, 6, // +x
		5, 7, 6,

		0, 1, 5, // -y
		0, 5, 4,

		2, 6, 7, // +y
		2, 7, 3,

		0, 4, 6, // -z
		0, 6, 2,

		1, 3, 7, // +z
		1, 7, 5,
	};

	const UINT indexBufferSize = sizeof(cubeIndices);
	indicesbuffersizeGlobal = (UINT)std::size(cubeIndices);
	// Create the index buffer resource in the GPU's default heap and copy index data into it using the upload heap.
	// The upload resource must not be released until after the GPU has finished using it.
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBufferUpload;
	CD3DX12_RESOURCE_DESC indexBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize);
	DB::ThrowIfFailed(d3device->CreateCommittedResource(
		&defaultHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&indexBufferDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&m_indexBuffer)));
	m_indexBuffer.Get()->SetName(L"m_indexbuffer");
	DB::ThrowIfFailed(d3device->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&indexBufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBufferUpload)));
	indexBufferUpload.Get()->SetName(L"Buffer");

	{
		D3D12_SUBRESOURCE_DATA indexData = {};
		indexData.pData = reinterpret_cast<BYTE*>(cubeIndices);
		indexData.RowPitch = indexBufferSize;
		indexData.SlicePitch = indexData.RowPitch;
		
			UpdateSubresources(m_commandList.Get(), m_indexBuffer.Get(), indexBufferUpload.Get(), 0, 0, 1, &indexData);

			CD3DX12_RESOURCE_BARRIER indexBufferResourceBarrier =
				CD3DX12_RESOURCE_BARRIER::Transition(m_indexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
			m_commandList->ResourceBarrier(1, &indexBufferResourceBarrier);
		
	}
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.NumDescriptors = 3;
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		// This flag indicates that this descriptor heap can be bound to the pipeline and that descriptors contained in it can be referenced by a root table.
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		DB::ThrowIfFailed(d3device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_cbvHeap)));

	}

	static const UINT c_alignedConstantBufferSize = (sizeof(ModelViewProjectionConstantBuffer) + 255) & ~255;

	CD3DX12_RESOURCE_DESC constantBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(3 * c_alignedConstantBufferSize);
	DB::ThrowIfFailed(d3device->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&constantBufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_constantBuffer)));

	D3D12_GPU_VIRTUAL_ADDRESS cbvGpuAddress = m_constantBuffer->GetGPUVirtualAddress();
	CD3DX12_CPU_DESCRIPTOR_HANDLE cbvCpuHandle(m_cbvHeap->GetCPUDescriptorHandleForHeapStart());
	UINT m_cbvDescriptorSize = d3device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	for (int n = 0; n < c_frameCount; n++)
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
		desc.BufferLocation = cbvGpuAddress;
		desc.SizeInBytes = c_alignedConstantBufferSize;
		d3device->CreateConstantBufferView(&desc, cbvCpuHandle);


		cbvGpuAddress += desc.SizeInBytes;
		cbvCpuHandle.Offset(m_cbvDescriptorSize);
	}

	CD3DX12_RANGE readRange(0, 0);
	UINT8* m_mappedConstantBuffer;
	{
			CD3DX12_RESOURCE_BARRIER vertexBufferResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
				m_vertexBuffer.Get(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_COPY_DEST);
			m_commandList->ResourceBarrier(1, &vertexBufferResourceBarrier);
		
		CD3DX12_RESOURCE_BARRIER indexBufferResourceBarrier =
				CD3DX12_RESOURCE_BARRIER::Transition(m_indexBuffer.Get(), D3D12_RESOURCE_STATE_INDEX_BUFFER, D3D12_RESOURCE_STATE_COPY_DEST);
			m_commandList->ResourceBarrier(1, &indexBufferResourceBarrier);
		
	}
	DB::ThrowIfFailed(m_constantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_mappedConstantBuffer)));
	ZeroMemory(m_mappedConstantBuffer, c_frameCount * c_alignedConstantBufferSize);
DB::ThrowIfFailed(m_commandList->Close());
	

	//m_commandList->Get()->CopyResource()
	// Error Starts from here	
	m_res->GetCmdQueue()->ExecuteCommandLists(1, (ID3D12CommandList* const*)(m_commandList.GetAddressOf()));
	m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.StrideInBytes = sizeof(VertexPositionColor);
	m_vertexBufferView.SizeInBytes = sizeof(cubeVertices);

	m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.SizeInBytes = sizeof(cubeIndices);
	m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
	

	m_res->WaitForGPU();
}

void ExecuteResources::Draw()
{
	D3D12_VIEWPORT vp;
	vp.TopLeftX = vp.TopLeftY = 0;
	vp.Width = m_res->GetWidth();
	vp.Height = m_res->GetHeight();
	vp.MinDepth = 1.f;
	vp.MaxDepth = 0.f;
	RECT scRect;
	scRect.left = scRect.top = 0;
	scRect.right = m_res->GetWidth();
	scRect.bottom = m_res->GetHeight();

	m_res->WaitForGPU();
//	m_res->m_commandAllocator[0].Get()->Reset();
;
	
DB::ThrowIfFailed(m_commandList->Reset(m_res->m_commandAllocator[0].Get(), m_pipelineState.Get()));



		PIXBeginEvent(m_commandList.Get(), 0, "Draw  the thot");
		{
			m_commandList->SetPipelineState(m_pipelineState.Get());
			m_commandList->SetGraphicsRootSignature(m_rootsignature.Get());
			ID3D12DescriptorHeap* ppHeaps[] = { m_cbvHeap.Get() };
			m_commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

			CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(m_cbvHeap->GetGPUDescriptorHandleForHeapStart(), m_res->currentframebackbufferindex, m_res->GetDevice().Get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
			//CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(m_cbvHeap->GetGPUDescriptorHandleForHeapStart());
		
			m_commandList->SetGraphicsRootDescriptorTable(0, m_cbvHeap->GetGPUDescriptorHandleForHeapStart());
		
			m_commandList->RSSetViewports(1, &vp);
			m_commandList->RSSetScissorRects(1,& scRect);

			CD3DX12_RESOURCE_BARRIER renderTargetResourceBarrier =
				CD3DX12_RESOURCE_BARRIER::Transition(m_res->GetRenderTargetView().Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

			m_commandList->ResourceBarrier(1, &renderTargetResourceBarrier);

			D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = m_res->CurrentDSVHandle();
			D3D12_CPU_DESCRIPTOR_HANDLE renderTargetView = m_res->m_rtvheap.Get()->GetCPUDescriptorHandleForHeapStart();
			renderTargetView.ptr += m_res->currentframebackbufferindex * m_res->m_rtvDescreptorSize;
			
			m_commandList->OMSetRenderTargets(1, &renderTargetView, true, &depthStencilView);
			
			// Transition depth buffer to D3D12_RESOURCE_STATE_DEPTH_WRITE state
			D3D12_RESOURCE_BARRIER depthBufferBarrier = {};
			depthBufferBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			depthBufferBarrier.Transition.pResource = m_res->m_depthStencilView.Get();
			depthBufferBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			depthBufferBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
			depthBufferBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

			m_commandList->ResourceBarrier(1, &depthBufferBarrier);
			m_commandList->ClearRenderTargetView(renderTargetView, DirectX::Colors::CornflowerBlue, 1,&scRect);
			m_commandList->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 1,&scRect);

			m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			m_commandList->SetGraphicsRootDescriptorTable(0, m_cbvHeap->GetGPUDescriptorHandleForHeapStart());
			m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
			m_commandList->IASetIndexBuffer(&m_indexBufferView);
			m_commandList->DrawIndexedInstanced(3, 1, 0, 0, 0);
			depthBufferBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_DEPTH_WRITE;
			depthBufferBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
			m_commandList->ResourceBarrier(1, &depthBufferBarrier);
			
			
			CD3DX12_RESOURCE_BARRIER presentResourceBuffer =
				CD3DX12_RESOURCE_BARRIER::Transition(m_res->GetRenderTargetView().Get(),
					D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
			m_commandList->ResourceBarrier(1, &presentResourceBuffer);
			// Indicate that the render target

		}
		PIXEndEvent(m_commandList.Get());
	

		DB::ThrowIfFailed(m_commandList->Close());
		
		//	m_commandList->Reset(m_res->GetCmdAlloc().Get(), m_pipelineState.Get());
	
	//for (int i = 0; i < 1; i++) {
	//	commandListArray[i] = m_commandList->Get();
	//
		m_res->GetCmdQueue()->ExecuteCommandLists(1, (ID3D12CommandList * const*)(m_commandList.GetAddressOf()));
	
	
		DB::ThrowIfFailed(m_res->GetSwapChain()->Present(1, 0));
		m_res->WaitForGPU();

}


