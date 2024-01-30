#include "pch.h"
#include "D3DRenderManager.h"
#include "Helper.h"

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <Psapi.h>

#include "EnvironmentActor.h"
#include "EnvironmentMeshComponent.h"
#include "Material.h"
#include "StaticMeshInstance.h"
#include "SkeletalMeshInstance.h"
#include "StaticMeshComponent.h"
#include "SkeletalMeshComponent.h"

#pragma comment(lib,"dxgi.lib")

const int SHADOWMAP_SIZE = 16384;

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

	// �����
	HRESULT hr = 0;

	// Create DXGI factory
	CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)(&m_dxgiFactory));
	m_dxgiFactory->EnumAdapters(0, reinterpret_cast<IDXGIAdapter**>(&m_dgxiAdapter));

	// ����ü�� �Ӽ� ���� ����ü ����
	DXGI_SWAP_CHAIN_DESC swapDesc = {};

	swapDesc.BufferCount = 1;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = m_hWnd;                         // ����ü�� ����� â �ڵ� ��
	swapDesc.Windowed = true;                               // â ��� ���� ����
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// �� ����(�ؽ�ó)�� ����/���� ũ�� ����
	swapDesc.BufferDesc.Width = width;
	swapDesc.BufferDesc.Height = height;

	// ȭ�� �ֻ��� ����
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;

	// ���ø� ���� ����
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;

	UINT creationFlags = 0;

#ifdef _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	// 1. ��ġ ����, 2. ����ü�� ����, 3. ��ġ ���ؽ�Ʈ ����
	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, creationFlags, NULL, NULL, D3D11_SDK_VERSION, &swapDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);

	// 4. ���� Ÿ�� �� ���� (�� ���۸� �̿��ϴ� ���� Ÿ�� ��)
	ID3D11Texture2D* BackBufferTexture = nullptr;

	m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBufferTexture);
	m_device->CreateRenderTargetView(BackBufferTexture, NULL, &m_renderTargetView);   // �ؽ�ó�� ���� ���� ����
	SAFE_RELEASE(BackBufferTexture);                                                             // �ܺ� ���� ī��Ʈ�� ���ҽ�Ų��

	// ���� Ÿ���� ���� ��� ���������ο� ���ε�
	// Flip Mode�� �ƴ� ������ ���� �ѹ��� �����ϸ� ��
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, NULL);

	// 5-1. ����Ʈ ����
	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
	m_viewport.Width = (float)width;
	m_viewport.Height = (float)height;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;

	// 5-2. �׸��ڸ� ���� ����Ʈ ����
	m_shadowViewport.TopLeftX = 0;
	m_shadowViewport.TopLeftY = 0;
	m_shadowViewport.Width = (float)SHADOWMAP_SIZE;
	m_shadowViewport.Height = (float)SHADOWMAP_SIZE;
	m_shadowViewport.MinDepth = 0.0f;
	m_shadowViewport.MaxDepth = 1.0f;

	// 6-1. ���� & ���ٽ� �� ����
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = (UINT)m_viewport.Width;
	descDepth.Height = (UINT)m_viewport.Height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
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

	// 6-2. �׸��ڸ� ���� ���� & ���ٽ� �� ����
	descDepth = {};
	descDepth.Width = (UINT)m_shadowViewport.Width;
	descDepth.Height = (UINT)m_shadowViewport.Height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.Format = DXGI_FORMAT_R32_TYPELESS;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	m_device->CreateTexture2D(&descDepth, nullptr, m_shadowMap.GetAddressOf());

	descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	m_device->CreateDepthStencilView(m_shadowMap.Get(), &descDSV, m_shadowMapDSV.GetAddressOf());

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	m_device->CreateShaderResourceView(m_shadowMap.Get(), &srvDesc, m_shadowMapSRV.GetAddressOf());

	// 7. ��� ���� ����
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
	m_deviceContext->VSSetConstantBuffers(1, 1, &m_directionalLightCB);
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

	// 8. Sample state ���� �� ����
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

	sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.MaxAnisotropy = (sampDesc.Filter == D3D11_FILTER_ANISOTROPIC) ? D3D11_REQ_MAXANISOTROPY : 1;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	m_device->CreateSamplerState(&sampDesc, &m_samplerClamp);
	m_deviceContext->PSSetSamplers(1, 1, &m_samplerClamp);

	// 9. ���� ó���� ���� ���� ���� ����
	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = true;
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

	// 10. �ø��� ���� RasterizerState ����
	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.AntialiasedLineEnable = true;
	rasterizerDesc.MultisampleEnable = true;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.FrontCounterClockwise = true;
	rasterizerDesc.DepthClipEnable = true;
	m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerStateCCW);

	rasterizerDesc.FrontCounterClockwise = false;
	m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerStateCW);

	// 11. Shader ����
	CreateStaticMesh_VS_IL();
	CreateSkeletalMesh_VS_IL();
	CreatePS();
	CreateEnvironmentVS();
	CreateEnvironmentPS();

	// 12-1. View, Projection �ʱ�ȭ
	m_eye = DirectX::XMVectorSet(0.0f, 300.0f, -500.0f, 0.0f);
	m_at = DirectX::XMVectorSet(0.0f, 0.0f, 0.1f, 0.0f);
	m_up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	/*m_eye = Vector3(0, 300, -500);
	m_at = m_eye + -m_eye.Forward;
	m_up = m_eye.Up;*/

	m_transform.ViewMatrix = DirectX::XMMatrixLookToLH(m_eye, m_at, m_up);
	m_transform.ProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, width / (FLOAT)height, 0.01f, 20000.0f);

	m_transform.ViewMatrix = DirectX::XMMatrixTranspose(m_transform.ViewMatrix);
	m_transform.ProjectionMatrix = DirectX::XMMatrixTranspose(m_transform.ProjectionMatrix);

	// 13. ImGUI �ʱ�ȭ
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
	/// TODO : ī�޶� ������Ʈ
	/// TODO : �޽� �ø�

	m_deviceContext->UpdateSubresource(m_directionalLightCB, 0, nullptr, &m_light, 0, 0);

	for (auto& staticMeshComponent : m_staticMeshComponents)
	{
		AddMeshInstance(staticMeshComponent);
	}
	for (auto& skeletalMeshComponent : m_skeletalMeshComponents)
	{
		AddMeshInstance(skeletalMeshComponent);
	}

	// Shadow View, Shadow Projection ����
	Matrix shadowProjection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, m_shadowViewport.Width / (float)m_shadowViewport.Height, 0.01f, 20000.0f);
	Vector3 shadowLookAt = m_transform.ViewMatrix.Translation() + m_transform.ViewMatrix.Forward() * 1000;
	Vector4 lightDir = Vector4(m_light.Direction.x, m_light.Direction.y, m_light.Direction.z, m_light.Direction.w);
	Vector3 shadowPos = shadowLookAt + (-lightDir * 5000);
	Matrix shadowView = DirectX::XMMatrixLookAtLH(shadowPos, shadowLookAt, Vector3(0, 1, 0));

	m_transform.ShadowViewMatrix = shadowView.Transpose();
	m_transform.ShadowProjectionMatrix = shadowProjection.Transpose();
}

void D3DRenderManager::Render()
{
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_deviceContext->RSSetViewports(1, &m_shadowViewport);
	m_deviceContext->OMSetRenderTargets(0, NULL, m_shadowMapDSV.Get());
	m_deviceContext->ClearDepthStencilView(m_shadowMapDSV.Get(), D3D11_CLEAR_DEPTH, 1.f, 0.f);
	m_deviceContext->PSSetShader(NULL, NULL, 0);
	m_deviceContext->VSSetConstantBuffers(0, 1, &m_transformCB);
	m_deviceContext->UpdateSubresource(m_transformCB, 0, nullptr, &m_transform, 0, 0);

	m_deviceContext->ClearRenderTargetView(m_renderTargetView, m_clearColor);
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	m_deviceContext->RSSetViewports(1, &m_viewport);
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
	m_deviceContext->PSSetShaderResources(11, 1, m_shadowMapSRV.GetAddressOf());

	if (m_environmentMeshComponent)
	{
		RenderEnvironment();
	}

	RenderStaticMeshInstance();
	RenderSkeletalMeshInstance();

	RenderImGUI();

	m_swapChain->Present(0, 0);
}

void D3DRenderManager::SetEnvironment(EnvironmentActor* actor)
{
	if (actor)
	{
		auto component = actor->GetEnvironmentMeshComponent();
		assert(component != nullptr);
		m_environmentMeshComponent = component;

		m_deviceContext->PSSetShaderResources(7, 1, component->m_envTexture->m_textureRV.GetAddressOf());
		m_deviceContext->PSSetShaderResources(8, 1, component->m_envIBLDiffuseTexture->m_textureRV.GetAddressOf());
		m_deviceContext->PSSetShaderResources(9, 1, component->m_envIBLSpecularTexture->m_textureRV.GetAddressOf());
		m_deviceContext->PSSetShaderResources(10, 1, component->m_envIBLBRDFTexture->m_textureRV.GetAddressOf());
		//m_material.UseIBL = true;
		m_deviceContext->UpdateSubresource(m_materialCB, 0, nullptr, &m_material, 0, 0);
	}
	else
	{
		m_environmentMeshComponent = nullptr;
		//m_material.UseIBL = false;
		m_deviceContext->UpdateSubresource(m_materialCB, 0, nullptr, &m_material, 0, 0);
	}
}

bool D3DRenderManager::InitImGUI()
{
	// ImGui �ʱ�ȭ
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// ImGui ��Ÿ�� ����
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// �÷���, ������ ����
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

		float x = DirectX::XMVectorGetX(m_eye);
		float y = DirectX::XMVectorGetY(m_eye);
		float z = DirectX::XMVectorGetZ(m_eye);
		ImGui::Text("Position");
		ImGui::Text("X");
		ImGui::SameLine();
		ImGui::SliderFloat("##cpx", &x, -1000.f, 10000.f);
		ImGui::Text("Y");
		ImGui::SameLine();
		ImGui::SliderFloat("##cpy", &y, -1000.f, 10000.f);
		ImGui::Text("Z");
		ImGui::SameLine();
		ImGui::SliderFloat("##cpz", &z, -1000.f, 10000.f);
		m_eye = DirectX::XMVectorSet(x, y, z, 0.0f);

		/*m_at = m_eye + -m_eye.Forward;
		m_up = m_eye.Up;*/

		m_transform.ViewMatrix = DirectX::XMMatrixLookToLH(m_eye, m_at, m_up);
		m_transform.ViewMatrix = DirectX::XMMatrixTranspose(m_transform.ViewMatrix);

		/*ImGui::Text("Rotation");
		ImGui::Text("X");
		ImGui::SameLine();
		ImGui::SliderFloat("##crx", &m_camRoll, -360.f, 360.f);
		ImGui::Text("Y");
		ImGui::SameLine();
		ImGui::SliderFloat("##cry", &m_camPitch, -360.f, 360.f);
		ImGui::Text("Z");
		ImGui::SameLine();
		ImGui::SliderFloat("##crz", &m_camYaw, -360.f, 360.f);
		Matrix rotation = DirectX::XMMatrixRotationRollPitchYaw(
			DirectX::XMConvertToRadians(m_camPitch),
			DirectX::XMConvertToRadians(m_camYaw),
			DirectX::XMConvertToRadians(m_camRoll));

		m_transform.ViewMatrix *= rotation;
		m_transform.ViewMatrix = DirectX::XMMatrixTranspose(m_transform.ViewMatrix);*/

		m_light.EyePosition = m_eye;
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
		ImGui::Text("Specular Intensity");
		ImGui::SliderFloat("##si", (float*)&m_material.SpecularPower, 2.f, 5000.f);
		ImGui::Text("Ambient Occlusion");
		ImGui::SliderFloat("##ao", (float*)&m_material.AmbientOcclusion, 0.f, 1.f);

		//ImGui::SliderFloat("##uIBL", (float*)&m_material.UseIBL, 0.f, 1.f);
		ImGui::Text("Use IBL");
		m_UseIBL == true ? m_material.UseIBL = true : m_material.UseIBL = false;
		ImGui::Checkbox("##uIBL", &m_UseIBL);

		 m_deviceContext->UpdateSubresource(m_materialCB, 0, nullptr, &m_material, 0, 0);
		ImGui::End();
	}

	{
		ImGui::Begin("Current State");
		ImGui::Text("Up Arrow : Create zelda static mesh");
		ImGui::Text("Left Arrow : Create character skeletal mesh");
		ImGui::Text("Down Arrow : Delete mesh");
		std::string videoMemoryInfo;
		GetVideoMemoryInfo(videoMemoryInfo);
		ImGui::Text("Video Memory : %s", videoMemoryInfo.c_str());
		std::string systemMemoryInfo;
		GetSystemMemoryInfo(systemMemoryInfo);
		ImGui::Text("System Memory : %s", systemMemoryInfo.c_str());
		ImGui::Image(m_shadowMapSRV.Get(), ImVec2(256, 256));
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

void D3DRenderManager::CreateEnvironmentVS()
{
	HRESULT hr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	hr = CompileShaderFromFile(L"EnvironmentVS.hlsl", "main", "vs_5_0", &vertexShaderBuffer);
	if (FAILED(hr))
	{
		hr = D3DReadFileToBlob(L"EnvironmentVS.cso", &vertexShaderBuffer);
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

	hr = m_device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_environmentIL);

	m_device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_environmentVS);
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

	pixelShaderBuffer = nullptr;
	hr = CompileShaderFromFile(L"ShadowPS.hlsl", "main", "ps_5_0", &pixelShaderBuffer);
	if (FAILED(hr))
	{
		hr = D3DReadFileToBlob(L"ShadowPS.cso", &pixelShaderBuffer);
	}

	m_device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_shadowPS);
	SAFE_RELEASE(pixelShaderBuffer);
}

void D3DRenderManager::CreateEnvironmentPS()
{
	HRESULT hr;
	ID3D10Blob* pixelShaderBuffer = nullptr;
	hr = CompileShaderFromFile(L"EnvironmentPS.hlsl", "main", "ps_5_0", &pixelShaderBuffer);
	if (FAILED(hr))
	{
		hr = D3DReadFileToBlob(L"EnvironmentPS.cso", &pixelShaderBuffer);
	}

	m_device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_environmentPS);
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
	m_deviceContext->RSSetState(m_rasterizerStateCW);
	m_deviceContext->OMSetBlendState(m_alphaBlendState, nullptr, 0xffffffff);

	//���������ο� �����ϴ� ���͸����� �ؽ��� ������ �ּ�ȭ �ϱ����� ���͸��� ���� �����Ѵ�.
	m_staticMeshInstance.sort([](const StaticMeshInstance* lhs, const StaticMeshInstance* rhs)
		{
			return lhs->m_material < rhs->m_material;
		});

	Material* pPrevMaterial = nullptr;
	for (const auto& meshInstance : m_staticMeshInstance)
	{
		// ���͸����� ���� ���͸���� �ٸ����� ���������ο� �ؽ��ĸ� �����Ѵ�.
		if (pPrevMaterial != meshInstance->m_material)
		{
			ApplyMaterial(meshInstance->m_material);	// ���͸��� ����
			pPrevMaterial = meshInstance->m_material;
		}

		// Scale, Rotation ����
		m_transform.WorldMatrix = meshInstance->m_nodeWorldTM->Transpose();
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
	m_deviceContext->RSSetState(m_rasterizerStateCW);
	m_deviceContext->OMSetBlendState(m_alphaBlendState, nullptr, 0xffffffff);

	//���������ο� �����ϴ� ���͸����� �ؽ��� ������ �ּ�ȭ �ϱ����� ���͸��� ���� �����Ѵ�.
	m_skeletalMeshInstance.sort([](const SkeletalMeshInstance* lhs, const SkeletalMeshInstance* rhs)
		{
			return lhs->m_material < rhs->m_material;
		});

	Material* pPrevMaterial = nullptr;
	for (const auto& meshInstance : m_skeletalMeshInstance)
	{
		// ���͸����� ���� ���͸���� �ٸ����� ���������ο� �ؽ��ĸ� �����Ѵ�.
		if (pPrevMaterial != meshInstance->m_material)
		{
			ApplyMaterial(meshInstance->m_material);	// ���͸��� ����
			pPrevMaterial = meshInstance->m_material;
		}

		meshInstance->UpdateMatrixPalette(&m_matrixPalette);
		m_deviceContext->UpdateSubresource(m_matrixPaletteCB, 0, nullptr, &m_matrixPalette, 0, 0);

		// Scale, Rotation ����
		m_transform.WorldMatrix = meshInstance->m_nodeWorldTM->Transpose();
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
	m_skeletalMeshInstance.clear();
}

void D3DRenderManager::RenderEnvironment()
{
	m_deviceContext->IASetInputLayout(m_environmentIL);
	m_deviceContext->VSSetShader(m_environmentVS, nullptr, 0);
	m_deviceContext->PSSetShader(m_environmentPS, nullptr, 0);
	m_deviceContext->RSSetState(m_rasterizerStateCCW);
	m_deviceContext->OMSetBlendState(m_alphaBlendState, nullptr, 0xffffffff);

	m_transform.WorldMatrix = m_environmentMeshComponent->m_worldTM.Transpose();
	m_deviceContext->UpdateSubresource(m_transformCB, 0, nullptr, &m_transform, 0, 0);
	m_environmentMeshComponent->m_meshInstance.Render(m_deviceContext);
}

void D3DRenderManager::GetVideoMemoryInfo(std::string& string)
{
	DXGI_QUERY_VIDEO_MEMORY_INFO videoMemoryInfo;
	m_dgxiAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &videoMemoryInfo);

	string = std::to_string(videoMemoryInfo.CurrentUsage / 1024 / 1024) + " MB" + " / " + std::to_string(videoMemoryInfo.Budget / 1024 / 1024) + " MB";
}

void D3DRenderManager::GetSystemMemoryInfo(std::string& string)
{
	HANDLE hProcess = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS_EX pmc;
	pmc.cb = sizeof(PROCESS_MEMORY_COUNTERS_EX);
	GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	string = std::to_string((pmc.PagefileUsage) / 1024 / 1024) + " MB";
}
