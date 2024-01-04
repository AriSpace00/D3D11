#pragma once

class Material;
class StaticMesh;

class StaticMeshInstance
{
	friend class D3DRenderManager;

public:
	StaticMeshInstance();
	~StaticMeshInstance();

public:
	StaticMesh* m_mesh;
	Material* m_material;
	Matrix* m_nodeWorldTM;

public:
	void Create(StaticMesh* mesh, Matrix* worldTM, Material* material);
	virtual void Render(ID3D11DeviceContext* deviceContext);
};

