#pragma once

class Bone;
class Skeleton;
class Material;
class SkeletalMesh;
struct CB_MatrixPalette;

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
	std::vector<Matrix*> m_boneReferences;	// Bone에 해당되는 NodeMatrix 포인터 저장

public:
	void Create(SkeletalMesh* mesh, Material* material, Bone* rootBone);
	void UpdateMatrixPalette(CB_MatrixPalette* matrixPalette);
	virtual void Render(ID3D11DeviceContext* deviceContext);
};

