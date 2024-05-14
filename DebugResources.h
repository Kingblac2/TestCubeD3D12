#pragma once
#include<d3d12.h>
#include <d3dcompiler.h>



// Naming helper function for ComPtr<T>.
// Assigns the name of the variable as the name of the object.

namespace DB {


#if defined(_DEBUG)
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
	{
					// Set a breakpoint on this line to catch Win32 API errors.
	//		throw Platform::Exception::CreateException(hr);
			throw std::exception();
		}
	}

	static inline void SetName(ID3D12Object* pObject, LPCWSTR name)
	{
		 
		pObject->SetName(name);
	}

	static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
		const std::wstring& filename,
		const D3D_SHADER_MACRO* defines,
		const std::string& entrypoint,
		const std::string& target)
	{
		
		UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;


		HRESULT hr = S_OK;

		Microsoft::WRL::ComPtr<ID3DBlob> byteCode = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> errors;
		hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

		if (errors != nullptr)
			OutputDebugStringA((char*)errors->GetBufferPointer());

		ThrowIfFailed(hr);

		return byteCode;
	}
	
#endif // defined(_DEBUG)
}


#define DebugName(x) DB::SetName(x.Get(),L#x)
								