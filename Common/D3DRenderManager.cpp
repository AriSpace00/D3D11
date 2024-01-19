#include "pch.h"
#include "D3DRenderManager.h"
#include "Helper.h"

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#include "Material.h"
#include "StaticMeshInstance.h"
#include "SkeletalMeshInstance.h"
#include "StaticMeshComponent.h"
#include "SkeletalMeshComponent.h"


D3DRenderManager* D3DRenderManager::m_instance = nullptr;

D3DRenderManager::D3DRenderManager()
{
	assert(m_instance == nullptr);
	m_instance = this;
}

D3DRenderManager::~D3DRenderManager()
{
	UnInitialize();
}

bool D3DRenderManager::Initialize(HWND hWnd, UINT width, UINT height)
{
	m_hWnd = hWnd;

	// 결과값
	HRESULT hr = 0;

	// 스왑체인 속성 설정 구조체 생성
	DXGI_SWAP_CHAIN_DESC swapDesc = {};

	swapDesc.BufferCount = 1;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = m_hWnd;                         // 스왑체인 출력할 창 핸들 값
	swapDesc.Windowed = true;                               // 창 모드 여부 설정
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// 백 버퍼(텍스처)의 가로/세로 크기 설정
	swapDesc.BufferDesc.Width = width;
	swapDesc.BufferDesc.Height = height;

	// 화면 주사율 설정
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;

	// 샘플링 관련 설정
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;

	UINT creationFlags = 0;

#ifdef _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	// 1. 장치 생성, 2. 스왑체인 생성, 3. 장치 컨텍스트 생성
	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, creationFlags, NULL, NULL, D3D11_SDK_VERSION, &swapDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);

	// 4. 렌더 타겟 뷰 생성 (백 버퍼를 이용하는 렌더 타겟 뷰)
	ID3D11Texture2D* BackBufferTexture = nullptr;

	m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBufferTexture);
	m_device->CreateRenderTargetView(BackBufferTexture, NULL, &m_renderTargetView);   // 텍스처는 내부 참조 증가
	SAFE_RELEASE(BackBufferTexture);                                                             // 외부 참조 카운트를 감소시킨다

	// 렌더 타겟을 최종 출력 파이프라인에 바인딩
	// Flip Mode가 아닐 때에는 최초 한번만 설정하면 됨
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, NULL);

	// 5. 뷰포트 설정
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)width;
	viewport.Height = (float)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	m_deviceContext->RSSetViewports(1, &viewport);

	// 6. 뎁스 & 스텐실 뷰 생성
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	ID3D11Texture2D* textureDepthStencil = nullptr;
	m_device->CreateTexture2D(&descDepth, nullptr, &textureDepthStencil);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	m_device->CreateDepthStencilView(textureDepthStencil, &descDSV, &m_depthStencilView);
	SAFE_RELEASE(textureDepthStencil);

	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	// 7. 상수 버퍼 생성
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CB_Transform);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	m_device->CreateBuffer(&bd, nullptr, &m_transformCB);
	m_deviceContext->VSSetConstantBuffers(0, 1, &m_transformCB);
	m_deviceContext->PSSetConstantBuffers(0, 1, &m_transformCB);

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CB_DirectionalLight);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	m_device->CreateBuffer(&bd, nullptr, &m_directionalLightCB);
	m_deviceContext->PSSetConstantBuffers(1, 1, &m_directionalLightCB);

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CB_Material);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	m_device->CreateBuffer(&bd, nullptr, &m_materialCB);
	m_deviceContext->VSSetConstantBuffers(2, 1, &m_materialCB);
	m_deviceContext->PSSetConstantBuffers(2, 1, &m_materialCB);

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CB_MatrixPalette);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	m_device->CreateBuffer(&bd, nullptr, &m_matrixPaletteCB);
	m_deviceContext->VSSetConstantBuffers(3, 1, &m_matrixPaletteCB);
	m_deviceContext->PSSetConstantBuffers(3, 1, &m_matrixPaletteCB);

	// 8. Sample state 생성 및 설정
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	m_device->CreateSamplerState(&sampDesc, &m_samplerLinear);
	m_deviceContext->PSSetSamplers(0, 1, &m_samplerLinear);

	// 9. 투명 처리를 위한 블렌드 상태 생성
	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	m_device->CreateBlendState(&blendDesc, &m_alphaBlendState);

	// 10. VertexShader, PixelShader, InputLayout 생성
	CreateStaticMesh_VS_IL();
	CreateSkeletalMesh_VS_IL();
	CreatePS();

	// 11. View, Projection 매트릭스 초기화
	m_eye = DirectX::XMVectorSet(0.0f, 300.0f, -500.0f, 0.0f);
	m_at = DirectX::XMVectorSet(0.0f, 0.0f, 0.1f, 0.0f);
	m_up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	m_transform.ViewMatrix = DirectX::XMMatrixLookToLH(m_eye, m_at, m_up);
	m_transform.ProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, width / (FLOAT)height, 0.01f, 20000.0f);

	m_transform.ViewMatrix = DirectX::XMMatrixTranspose(m_transform.ViewMatrix);
	m_transform.ProjectionMatrix = DirectX::XMMatrixTranspose(m_transform.ProjectionMatrix);

	// 12. ImGUI 초기화
	InitImGUI();

	return true;
}

void D3DRenderManager::UnInitialize()
{
	SAFE_RELEASE(m_directionalLightCB);
	SAFE_RELEASE(m_transformCB);
	SAFE_RELEASE(m_materialCB);
	SAFE_RELEASE(m_matrixPaletteCB);
	SAFE_RELEASE(m_samplerLinear);
	SAFE_RELEASE(m_alphaBlendState);

	SAFE_RELEASE(m_renderTargetView);
	SAFE_RELEASE(m_depthStencilView);
	SAFE_RELEASE(m_deviceContext);
	SAFE_RELEASE(m_swapChain);
	SAFE_RELEASE(m_device);

	UnInitImGUI();
}

void D3DRenderManager::Update()
{
	/// TODO : 카메라 업데이트
	/// TODO : 메쉬 컬링

	// light 초기화
	m_deviceContext->UpdateSubresource(m_directionalLightCB, 0, nullptr, &m_light, 0, 0);

	for (auto& staticMeshComponent : m_staticMeshComponents)
	{
		AddMeshInstance(staticMeshComponent);
	}
	for (auto& skeletalMeshComponent : m_skeletalMeshComponents)
	{
		AddMeshInstance(skeletalMeshComponent);
	}
}

void D3DRenderManager::Render()
{
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, m_clearColor);
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	RenderStaticMeshInstance();
	RenderSkeletalMeshInstance();

	RenderImGUI();

	m_swapChain->Present(0, 0);
}

bool D3DRenderManager::InitImGUI()
{
	// ImGui 초기화
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// ImGui 스타일 설정
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// 플랫폼, 렌더러 설정
	ImGui_ImplWin32_Init(m_hWnd);
	ImGui_ImplDX11_Init(m_device, m_deviceContext);

	return true;
}

void D3DRenderManager::UnInitImGUI()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void D3DRenderManager::RenderImGUI()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	/// Camera Properties
	{
		ImGui::Begin("Camera Properties");
		ImGui::Text("World Transform");
		float x = DirectX::XMVectorGetX(m_eye);
		float y = DirectX::XMVectorGetY(m_eye);
		float z = DirectX::XMVectorGetZ(m_eye);
		ImGui::Text("X");
		ImGui::SameLine();
		ImGui::SliderFloat("##cwx", &x, -1000.0f, 10000.0f);
		ImGui::Text("Y");
		ImGui::SameLine();
		ImGui::SliderFloat("##cwy", &y, -1000.0f, 10000.0f);
		ImGui::Text("Z");
		ImGui::SameLine();
		ImGui::SliderFloat("##cwz", &z, -10000.0f, 0.0f);
		m_eye = DirectX::XMVectorSet(x, y, z, 0.0f);
		m_transform.ViewMatrix = DirectX::XMMatrixLookToLH(m_eye, m_at, m_up);
		m_transform.ViewMatrix = DirectX::XMMatrixTranspose(m_transform.ViewMatrix);
		ImGui::End();
	}

	/// Actor Properties
	{
		ImGui::Begin("Actor Properties");
		ImGui::Text("World Transform");
		ImGui::Text("Roll(X)");
		ImGui::SameLine();
		ImGui::SliderFloat("##roll", &m_roll, -360.0f, 360.0f);
		ImGui::Text("Pitch(Y)");
		ImGui::SameLine();
		ImGui::SliderFloat("##pitch", &m_pitch, -360.0f, 360.0f);
		ImGui::Text("Yaw(Z)");
		ImGui::SameLine();
		ImGui::SliderFloat("##yaw", &m_yaw, -360.0f, 360.0f);
		ImGui::Text("Scale");
		ImGui::SameLine();
		ImGui::SliderFloat("##scale", &m_scale, 1.0f, 100.0f);
		ImGui::End();
	}

	{
		ImGui::Begin("Light Properties");
		ImGui::Text("Directional Light");
		ImGui::SliderFloat("##dx", (float*)&m_light.Direction.x, 1.f, -1.f);
		ImGui::SliderFloat("##dy", (float*)&m_light.Direction.y, 1.f, -1.f);
		ImGui::SliderFloat("##dz", (float*)&m_light.Direction.z, 1.f, -1.f);
		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void D3DRenderManager::ApplyMaterial(Material* material)
{
	ID3D11ShaderResourceView* nullSRV[7] = {
		material->m_diffuseRV != nullptr ? material->m_diffuseRV->m_textureRV.Get() : nullptr,
		material->m_normalRV != nullptr ? material->m_normalRV->m_textureRV.Get() : nullptr,
		material->m_specularRV != nullptr ? material->m_specularRV->m_textureRV.Get() : nullptr,
		material->m_emissiveRV != nullptr ? material->m_emissiveRV->m_textureRV.Get() : nullptr,
		material->m_opacityRV != nullptr ? material->m_opacityRV->m_textureRV.Get() : nullptr,
		material->m_metalicRV != nullptr ? material->m_metalicRV->m_textureRV.Get() : nullptr,
		material->m_roughnessRV != nullptr ? material->m_roughnessRV->m_textureRV.Get() : nullptr
	};

	m_deviceContext->PSSetShaderResources(0, 7, nullSRV);

	m_material.UseDiffuseMap = material->m_diffuseRV != nullptr ? true : false;
	m_material.UseNormalMap = material->m_normalRV != nullptr ? true : false;
	m_material.UseSpecularMap = material->m_specularRV != nullptr ? true : false;
	m_material.UseEmissiveMap = material->m_emissiveRV != nullptr ? true : false;
	m_material.UseOpacityMap = material->m_opacityRV != nullptr ? true : false;
	m_material.UseMetalicMap = material->m_metalicRV != nullptr ? true : false;
	m_material.UseRoughnessMap = material->m_roughnessRV != nullptr ? true : false;

	if (m_material.UseOpacityMap && m_alphaBlendState != nullptr)
		m_deviceContext->OMSetBlendState(m_alphaBlendState, nullptr, 0xffffffff);
	else
		m_deviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);

	m_deviceContext->UpdateSubresource(m_materialCB, 0, nullptr, &m_material, 0, 0);
}

void D3DRenderManager::CreateStaticMesh_VS_IL()
{
	HRESULT hr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	hr = CompileShaderFromFile(L"StaticMeshVS.hlsl", "main", "vs_5_0", &vertexShaderBuffer);
	if (FAILED(hr))
	{
		hr = D3DReadFileToBlob(L"StaticMeshVS.cso", &vertexShaderBuffer);
	}

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"BlendIndices",    0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BlendWeights",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	hr = m_device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_staticMeshIL);

	m_device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_staticMeshVS);
	SAFE_RELEASE(vertexShaderBuffer);
}

void D3DRenderManager::CreateSkeletalMesh_VS_IL()
{
	HRESULT hr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	hr = CompileShaderFromFile(L"SkeletalMeshVS.hlsl", "main", "vs_5_0", &vertexShaderBuffer);
	if (FAILED(hr))
	{
		hr = D3DReadFileToBlob(L"SkeletalMeshVS.cso", &vertexShaderBuffer);
	}

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"BlendIndices",    0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BlendWeights",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	hr = m_device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_skeletalMeshIL);

	m_device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_skeletalMeshVS);
	SAFE_RELEASE(vertexShaderBuffer);
}

void D3DRenderManager::CreatePS()
{
	HRESULT hr;
	ID3D10Blob* pixelShaderBuffer = nullptr;
	hr = CompileShaderFromFile(L"PBRPixelShader.hlsl", "main", "ps_5_0", &pixelShaderBuffer);
	if (FAILED(hr))
	{
		hr = D3DReadFileToBlob(L"PBRPixelShader.cso", &pixelShaderBuffer);
	}

	m_device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	SAFE_RELEASE(pixelShaderBuffer);
}

void D3DRenderManager::AddMeshInstance(SkeletalMeshComponent* skeletalMesh)
{
	for (int i = 0; i < skeletalMesh->m_meshInstances.size(); i++)
	{
		m_skeletalMeshInstance.push_back(&skeletalMesh->m_meshInstances[i]);
	}
}

void D3DRenderManager::AddMeshInstance(StaticMeshComponent* staticMesh)
{
	for (int i = 0; i < staticMesh->m_meshInstances.size(); i++)
	{
		m_staticMeshInstance.push_back(&staticMesh->m_meshInstances[i]);
	}
}

void D3DRenderManager::RenderStaticMeshInstance()
{
	m_deviceContext->IASetInputLayout(m_staticMeshIL);
	m_deviceContext->VSSetShader(m_staticMeshVS, nullptr, 0);
	m_deviceContext->PSSetShader(m_pixelShader, nullptr, 0);

	//파이프라인에 설정하는 머터리얼의 텍스쳐 변경을 최소화 하기위해 머터리얼 별로 정렬한다.
	m_staticMeshInstance.sort([](const StaticMeshInstance* lhs, const StaticMeshInstance* rhs)
		{
			return lhs->m_material < rhs->m_material;
		});

	Material* pPrevMaterial = nullptr;
	for (const auto& meshInstance : m_staticMeshInstance)
	{
		// 머터리얼이 이전 머터리얼과 다를때만 파이프라인에 텍스쳐를 변경한다.
		if (pPrevMaterial != meshInstance->m_material)
		{
			ApplyMaterial(meshInstance->m_material);	// 머터리얼 적용
			pPrevMaterial = meshInstance->m_material;
		}

		m_transform.WorldMatrix = meshInstance->m_nodeWorldTM->Transpose();

		// Scale, Rotation 조정
		Matrix scale = Matrix::CreateScale(m_scale, m_scale, m_scale);
		
		Matrix spin = DirectX::XMMatrixRotationRollPitchYaw(
			DirectX::XMConvertToRadians(m_pitch),
			DirectX::XMConvertToRadians(m_yaw),
			DirectX::XMConvertToRadians(m_roll));

		m_transform.WorldMatrix *= scale * spin;

		m_deviceContext->UpdateSubresource(m_transformCB, 0, nullptr, &m_transform, 0, 0);

		// Draw
		meshInstance->Render(m_deviceContext);
	}
	m_staticMeshInstance.clear();
}

void D3DRenderManager::RenderSkeletalMeshInstance()
{
	m_deviceContext->IASetInputLayout(m_skeletalMeshIL);
	m_deviceContext->VSSetShader(m_skeletalMeshVS, nullptr, 0);
	m_deviceContext->PSSetShader(m_pixelShader, nullptr, 0);

	//파이프라인에 설정하는 머터리얼의 텍스쳐 변경을 최소화 하기위해 머터리얼 별로 정렬한다.
	m_skeletalMeshInstance.sort([](const SkeletalMeshInstance* lhs, const SkeletalMeshInstance* rhs)
		{
			return lhs->m_material < rhs->m_material;
		});

	Material* pPrevMaterial = nullptr;
	for (const auto& meshInstance : m_skeletalMeshInstance)
	{
		// 머터리얼이 이전 머터리얼과 다를때만 파이프라인에 텍스쳐를 변경한다.
		if (pPrevMaterial != meshInstance->m_material)
		{
 			ApplyMaterial(meshInstance->m_material);	// 머터리얼 적용
			pPrevMaterial = meshInstance->m_material;
		}

		// Scale, Rotation 조정
		m_transform.WorldMatrix = meshInstance->m_nodeWorldTM->Transpose();
		Matrix scale = Matrix::CreateScale(m_scale, m_scale, m_scale);
		Matrix spin = DirectX::XMMatrixRotationRollPitchYaw(
			DirectX::XMConvertToRadians(m_pitch),
			DirectX::XMConvertToRadians(m_yaw),
			DirectX::XMConvertToRadians(m_roll));
		m_transform.WorldMatrix *= scale * spin;

		meshInstance->UpdateMatrixPalette(&m_matrixPalette);
		m_deviceContext->UpdateSubresource(m_transformCB, 0, nullptr, &m_transform, 0, 0);
		m_deviceContext->UpdateSubresource(m_matrixPaletteCB, 0, nullptr, &m_matrixPalette, 0, 0);

		// Draw
		meshInstance->Render(m_deviceContext);
	}
	m_skeletalMeshInstance.clear();
}
