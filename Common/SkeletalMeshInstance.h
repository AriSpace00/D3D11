#pragma once

class Material;
class SkeletalMesh;

class SkeletalMeshInstance
{
	friend class D3DRenderManager;

public:
	SkeletalMeshInstance();
	~SkeletalMeshInstance();

public:
	SkeletalMesh* m_mesh;
	Material* m_material;
	Matrix* m_nodeWorldTM;

public:
	void Create(SkeletalMesh* mesh, Matrix* worldTM, Material* material);
	virtual void Render(ID3D11DeviceContext* deviceContext);
};

