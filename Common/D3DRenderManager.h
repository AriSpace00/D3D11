#pragma once
#include <dxgi1_4.h>
#include <DirectXtk/BufferHelpers.h>
#include <directxtk/SimpleMath.h>
#include <list>
#include <wrl/client.h>
using namespace DirectX::SimpleMath;
using namespace Microsoft::WRL;

struct CB_DirectionalLight
{
	DirectX::XMFLOAT4 Direction = { 0.0f,0.0f,1.0f, 1.0f };
	Vector4 Ambient = { 0.0f,0.0f,0.0f,1.0f };
	Vector4 Diffuse = { 1.0f,1.0f,1.0f,1.0f };
	Vector4 Specular = { 1.0f,1.0f,1.0f,1.0f };
	Vector3 EyePosition;
	float DirectionalLight_pad0;
};

struct CB_Transform
{
	Matrix WorldMatrix;
	Matrix ViewMatrix;
	Matrix ProjectionMatrix;
	Matrix ShadowViewMatrix;
	Matrix ShadowProjectionMatrix;
};

struct CB_Material
{
	Vector4 Ambient = { 1.0f,1.0f,1.0f,1.0f };
	Vector4 Diffuse = { 1.0f,1.0f,1.0f,1.0f };
	Vector4 Specular = { 1.0f,1.0f,1.0f,1.0f };
	Vector4 Emissive = { 1.0f,1.0f,1.0f,1.0f };
	float Metalic;
	float Roughness;
	float SpecularPower = 80;
	float UseDiffuseMap;
	float UseNormalMap;
	float UseSpecularMap;
	float UseEmissiveMap;
	float UseOpacityMap;
	float UseMetalicMap;
	float UseRoughnessMap;
	float UseIBL = false;
	float AmbientOcclusion = 0.5f;
};

struct CB_MatrixPalette
{
	Matrix Array[128];
};

class Material;
class StaticMeshInstance;
class SkeletalMeshInstance;
class StaticMeshComponent;
class SkeletalMeshComponent;
class EnvironmentActor;
class EnvironmentMeshComponent;

class D3DRenderManager
{
public:
	D3DRenderManager();
	~D3DRenderManager();

	static D3DRenderManager* m_instance;

public:
	HWND m_hWnd = nullptr;

	/// 비디오 메모리와 시스템 메모리 위한 객체
	IDXGIFactory4* m_dxgiFactory = nullptr;
	IDXGIAdapter3* m_dgxiAdapter = nullptr;

	/// 필수 인터페이스
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	ID3D11DepthStencilView* m_depthStencilView = nullptr;
	IDXGISwapChain* m_swapChain = nullptr;

	ID3D11VertexShader* m_staticMeshVS = nullptr;
	ID3D11InputLayout* m_staticMeshIL = nullptr;
	ID3D11VertexShader* m_skeletalMeshVS = nullptr;
	ID3D11InputLayout* m_skeletalMeshIL = nullptr;

	ID3D11PixelShader* m_pixelShader = nullptr;
	ID3D11SamplerState* m_samplerLinear = nullptr;
	ID3D11SamplerState* m_samplerClamp = nullptr;
	ID3D11BlendState* m_alphaBlendState = nullptr;

	/// IBL을 위한 객체
	ID3D11RasterizerState* m_rasterizerStateCW = nullptr;
	ID3D11RasterizerState* m_rasterizerStateCCW = nullptr;

	ID3D11VertexShader* m_environmentVS = nullptr;
	ID3D11PixelShader* m_environmentPS = nullptr;
	ID3D11InputLayout* m_environmentIL = nullptr;

	/// 상수 버퍼
	ID3D11Buffer* m_directionalLightCB = nullptr;
	ID3D11Buffer* m_transformCB = nullptr;
	ID3D11Buffer* m_materialCB = nullptr;
	ID3D11Buffer* m_matrixPaletteCB = nullptr;

	CB_DirectionalLight m_light;
	CB_Transform m_transform;
	CB_Material m_material;
	CB_MatrixPalette m_matrixPalette;

	/// 그림자를 위한 객체
	ComPtr<ID3D11Texture2D> m_shadowMap;
	ComPtr<ID3D11DepthStencilView> m_shadowMapDSV;
	ComPtr<ID3D11ShaderResourceView> m_shadowMapSRV;
	D3D11_VIEWPORT m_viewport;
	D3D11_VIEWPORT m_shadowViewport;
	ID3D11PixelShader* m_shadowPS = nullptr;

	/// 오브젝트 저장용 객체
	UINT m_vertexBufferStride = 0;
	UINT m_vertexBufferOffset = 0;
	int m_indices = 0;

	std::list<StaticMeshInstance*> m_staticMeshInstance;
	std::list<StaticMeshComponent*> m_staticMeshComponents;

	std::list<SkeletalMeshInstance*> m_skeletalMeshInstance;
	std::list<SkeletalMeshComponent*> m_skeletalMeshComponents;

	EnvironmentMeshComponent* m_environmentMeshComponent;

	/// 잡다한거 (나중에 지워도 될 것들)
	const float m_clearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };

	/*DirectX::XMVECTOR m_eye;
	DirectX::XMVECTOR m_at;
	DirectX::XMVECTOR m_up;*/

	Vector3 m_eye;
	Vector3 m_at;
	Vector3 m_up;

	float m_roll = 0.0f;
	float m_pitch = 0.0f;
	float m_yaw = 0.0f;
	float m_scale = 1.0f;

	float m_camRoll = 0.0f;
	float m_camPitch = 0.0f;
	float m_camYaw = 0.0f;

	bool m_UseIBL = true;

public:
	bool Initialize(HWND hWnd, UINT width, UINT height);
	void UnInitialize();
	void Update();
	void Render();

	void SetEnvironment(EnvironmentActor* actor);

private:
	bool InitImGUI();
	void UnInitImGUI();
	void RenderImGUI();

	void ApplyMaterial(Material* material);

	void CreateStaticMesh_VS_IL();
	void CreateSkeletalMesh_VS_IL();
	void CreateEnvironmentVS();
	void CreatePS();
	void CreateEnvironmentPS();

	void AddMeshInstance(StaticMeshComponent* staticMesh);
	void AddMeshInstance(SkeletalMeshComponent* skeletalMesh);

	void RenderStaticMeshInstance();
	void RenderSkeletalMeshInstance();
	void RenderEnvironment();

	void GetVideoMemoryInfo(std::string& string);
	void GetSystemMemoryInfo(std::string& string);

};