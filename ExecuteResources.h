#pragma once
 
#include"Res.h"
#include<d3dcompiler.h>
#include <stdlib.h>
struct VertexPositionColor
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 color;
};
struct ModelViewProjectionConstantBuffer
{
	DirectX::XMFLOAT4X4 model;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};

//struct RootDesc
//{
//public:
//	//RootDesc(UINT NumParameters, D3D12_STATIC_SAMPLER_DESC staticsampler[], UINT NumOfStaticSamplers, D3D12_ROOT_SIGNATURE_FLAGS Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE);
//	constexpr inline D3D12_VERSIONED_ROOT_SIGNATURE_DESC& GetRootSignature() { return pRTSignature; };
//	constexpr inline D3D12_ROOT_PARAMETER1     GetRootParameters() { return pRTParameters; };
//	constexpr void Signature_Initialize(UINT NumParameters, D3D12_STATIC_SAMPLER_DESC1 staticsampler[], UINT NumOfStaticSamplers, D3D12_ROOT_SIGNATURE_FLAGS Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE) {
//		
//		pRTSignature.Init_1_2(pRTSignature, NumParameters, &pRTParameters, NumOfStaticSamplers, staticsampler, Flags);
//	}
//
//	constexpr void as_constant(UINT num_constant, D3D12_SHADER_VISIBILITY shadervisibility, UINT shader_register, UINT space = 0)
//	{
//		if (shadervisibility)
//			ImplementRootParam(D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS, shadervisibility);
//
//
//
//		pRTParameters.Constants.Num32BitValues = num_constant;
//		pRTParameters.Constants.ShaderRegister = shader_register;
//		pRTParameters.Constants.RegisterSpace = space;
//	}
//
//	constexpr RootDesc& as_cbv(D3D12_SHADER_VISIBILITY shadervisibility, UINT shader_register
//		, UINT space = 0, D3D12_ROOT_DESCRIPTOR_FLAGS flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE)
//	{
//		ImplementRootParam(D3D12_ROOT_PARAMETER_TYPE_CBV, shadervisibility);
//		pRTParameters.Descriptor.ShaderRegister = shader_register;
//		pRTParameters.Descriptor.RegisterSpace = space;
//		return *this;
//	}
//	constexpr void as_srv(D3D12_SHADER_VISIBILITY shadervisibility, UINT shader_register
//		, UINT space = 0, D3D12_ROOT_DESCRIPTOR_FLAGS flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE)
//	{
//		ImplementRootParam(D3D12_ROOT_PARAMETER_TYPE_SRV, shadervisibility);
//		pRTParameters.Descriptor.ShaderRegister = shader_register;
//		pRTParameters.Descriptor.RegisterSpace = space;
//	}
//	
//	constexpr void as_uav(D3D12_SHADER_VISIBILITY shadervisibility, UINT shader_register
//		, UINT space = 0, D3D12_ROOT_DESCRIPTOR_FLAGS flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE)
//	{
//		ImplementRootParam(D3D12_ROOT_PARAMETER_TYPE_UAV, shadervisibility);
//		//pRTParameters.InitAsUnorderedAccessView(shader_register, space, shadervisibility);
//		pRTParameters.Descriptor.ShaderRegister = shader_register;
//		pRTParameters.Descriptor.RegisterSpace = space;
//	}
//
//	/*
//	 @shadervisibility       which rendering pipline statge can access by this shader.
//	 @RangeType              Type of  resources in descriptor table.
//	 @NumOfDescriptorRanges  The number of descriptor ranges in the table layout.
//	 @desc_count_in_range    The number of descriptors in the range. Use -1 or UINT_MAX to specify an unbounded size.
//	 @from_register_to_access from which register this root signature is bind,e.g, if the parameter is 2 then you can access :register(t2).
//	 @offset_from_table_start   D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND, which indicates this range should immediately follow the previous range.
//	 */
//	void as_descriptortable(D3D12_SHADER_VISIBILITY shadervisibility, D3D12_DESCRIPTOR_RANGE_TYPE RangeType, UINT NumOfDescriptorRanges, UINT desc_count_in_range = -1, UINT from_register_to_access = 0u, UINT offset_from_table_start = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND)
//	{
//		if (shadervisibility)
//			ImplementRootParam(D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE, shadervisibility);
//
//		pRTParameters.DescriptorTable.NumDescriptorRanges = NumOfDescriptorRanges;
//		pRTParameters.DescriptorTable.pDescriptorRanges = &pRTRange;
//	}
//
//	RootDesc& root_range(
//		D3D12_DESCRIPTOR_RANGE_TYPE rangeType,
//		UINT numDescriptors,
//		UINT baseShaderRegister,
//		UINT registerSpace = 0,
//		D3D12_DESCRIPTOR_RANGE_FLAGS flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE,
//		UINT offsetInDescriptorsFromTableStart =
//		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND)
//	{
//
//		pRTRange.RangeType = rangeType;
//		pRTRange.NumDescriptors = numDescriptors;
//		pRTRange.RegisterSpace = registerSpace;
//		pRTRange.Flags = flags;
//		pRTRange.OffsetInDescriptorsFromTableStart = offsetInDescriptorsFromTableStart;
//		return *this;
//	}
//private:
//	constexpr void ImplementRootParam(D3D12_ROOT_PARAMETER_TYPE RootParameterType, D3D12_SHADER_VISIBILITY shaderVisible = D3D12_SHADER_VISIBILITY_ALL)
//	{
//		pRTParameters.ParameterType = RootParameterType;
//		pRTParameters.ShaderVisibility = shaderVisible;
//
//	}
//private:
//	CD3DX12_DESCRIPTOR_RANGE1 pRTRange;
//	CD3DX12_ROOT_PARAMETER1  pRTParameters;
//	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC pRTSignature;
//};

/// <summary>
/// 
/// </summary>

class ExecuteResources
{
public:
	ExecuteResources(UINT Width, UINT Height);
	~ExecuteResources();
	void ResourceLoading();
	void GPUResourceLoading();
	void Draw();

private:
	 UINT indicesbuffersizeGlobal;
	std::shared_ptr<WDResources> m_res;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootsignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_cbvHeap;

	Microsoft::WRL::ComPtr<ID3D12Resource> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_constantBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_indexBuffer;
	Microsoft::WRL::ComPtr<ID3DBlob> m_blob;
	Microsoft::WRL::ComPtr<ID3DBlob> m_errorblob;

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4>		m_commandList;

	D3D12_VERTEX_BUFFER_VIEW							m_vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW								m_indexBufferView;

};
