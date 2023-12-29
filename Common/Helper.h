#pragma once
#include <d3dcompiler.h>
#include <string>
#include <assimp/matrix4x4.h>

#define LOG_MESSAGEA(...) \
{ \
    char buffer[256]; \
    sprintf_s(buffer, 256, "[MESSAGE] %s:%d - ", __FUNCTION__, __LINE__); \
    char message[256]; \
    sprintf_s(message, 256, __VA_ARGS__); \
    strcat_s(buffer, message); \
    strcat_s(buffer, "\n"); \
    OutputDebugStringA(buffer); \
}

template <typename T>
void SAFE_RELEASE(T* p)
{
	if (p)
	{
		auto result = p->Release();
		p = nullptr;
	}
}

inline std::wstring ToWString(const std::string& s)
{
	std::wstring wsTmp(s.begin(), s.end());
	return wsTmp;
}

inline DirectX::XMMATRIX ConvertaiMatrixToXMMatrix(const aiMatrix4x4& aiMatrix)
{
	DirectX::XMMATRIX xmMatrix;
	xmMatrix.r[0] = DirectX::XMVectorSet(aiMatrix.a1, aiMatrix.b1, aiMatrix.c1, aiMatrix.d1);
	xmMatrix.r[1] = DirectX::XMVectorSet(aiMatrix.a2, aiMatrix.b2, aiMatrix.c2, aiMatrix.d2);
	xmMatrix.r[2] = DirectX::XMVectorSet(aiMatrix.a3, aiMatrix.b3, aiMatrix.c3, aiMatrix.d3);
	xmMatrix.r[3] = DirectX::XMVectorSet(aiMatrix.a4, aiMatrix.b4, aiMatrix.c4, aiMatrix.d4);
	return xmMatrix;
}

inline HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;

	// Disable optimizations to further improve shader debugging
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* pErrorBlob = nullptr;
	hr = D3DCompileFromFile(szFileName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			MessageBoxA(NULL, (char*)pErrorBlob->GetBufferPointer(), "CompileShaderFromFile", MB_OK);
			pErrorBlob->Release();
		}
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}

