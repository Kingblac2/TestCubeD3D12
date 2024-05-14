#pragma once

#include <wrl.h>
#include <wrl/client.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <pix.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <memory>
#include <vector>
#include <concrt.h>
#include <ppltasks.h>
#if defined(_DEBUG)
#include <dxgidebug.h>
#define DXCall(x)                                \
if (FAILED(x)) {                                 \
	char line_break[32];						 \
	sprintf_s(line_break, "%u", __LINE__);		 \
	OutputDebugStringA("Error in: ");			 \
	OutputDebugStringA(__FILE__);				 \
	OutputDebugStringA("\nLine: ");				 \
	OutputDebugStringA(line_break);			     \
	OutputDebugStringA("\n");					 \
	OutputDebugStringA(#x);						 \
	OutputDebugStringA("\n");					 \
	__debugbreak();								 \
												 \
}				
#define PRINT(text) cout << text << "\n" << flush; 
#define VERIFY_SUCCEEDED(hr) {if(FAILED(hr)) {PRINT("Error at: " << __FILE__ << ", line: " << __LINE__ << hex << hr); throw E_FAIL;} }

#endif
