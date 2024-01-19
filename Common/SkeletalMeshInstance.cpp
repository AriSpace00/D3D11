#include "pch.h"
#include "SkeletalMeshInstance.h"
#include "Bone.h"
#include "SkeletalMesh.h"
#include "D3DRenderManager.h"

SkeletalMeshInstance::SkeletalMeshInstance()
	: m_mesh(nullptr)
	, m_material(nullptr)
	, m_nodeWorldTM(nullptr)
{
}

SkeletalMeshInstance::~SkeletalMeshInstance()
{
}

void SkeletalMeshInstance::Create(SkeletalMesh* mesh, Material* material, Bone* rootBone)
{
	m_mesh = mesh;
	m_material = material;

	int meshBoneCount = m_mesh->m_boneReferences.size();
	m_boneReferences.resize(meshBoneCount);
	for (int i = 0; i < meshBoneCount; i++)
	{
		std::string& name = m_mesh->m_boneReferences[i].NodeName;
		Bone* bone = rootBone->FindBone(name);
		assert(bone != nullptr);
		m_boneReferences[i] = &bone->m_worldTM;
	}
	m_nodeWorldTM = &rootBone->m_worldTM;
}

void SkeletalMeshInstance::UpdateMatrixPalette(CB_MatrixPalette* matrixPalette)
{
	assert(m_boneReferences.size() == m_mesh->m_boneReferences.size());
	int meshBoneCount = m_mesh->m_boneReferences.size();
	for (int i = 0; i < meshBoneCount; i++)
	{
		Matrix& boneNodeWorldTM = *m_boneReferences[i];
		BoneReference& br = m_mesh->m_boneReferences[i];
		matrixPalette->Array[br.BoneIndex] = (br.OffsetMatrix * boneNodeWorldTM).Transpose();
	}
}

void SkeletalMeshInstance::Render(ID3D11DeviceContext* deviceContext)
{
	deviceContext->IASetVertexBuffers(0, 1, &m_mesh->m_vertexBuffer, &m_mesh->m_vertexBufferStride, &m_mesh->m_vertexBufferOffset);
	deviceContext->IASetIndexBuffer(m_mesh->m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->DrawIndexed(m_mesh->m_indexCount, 0, 0);
}
