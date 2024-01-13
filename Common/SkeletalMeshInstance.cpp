#include "pch.h"
#include "SkeletalMeshInstance.h"
#include "SkeletalMesh.h"

SkeletalMeshInstance::SkeletalMeshInstance()
	: m_mesh(nullptr)
	, m_material(nullptr)
	, m_nodeWorldTM(nullptr)
{
}

SkeletalMeshInstance::~SkeletalMeshInstance()
{
}

void SkeletalMeshInstance::Create(SkeletalMesh* mesh, Matrix* worldTM, Material* material)
{
	m_mesh = mesh;
	m_nodeWorldTM = worldTM;
	m_material = material;
}

void SkeletalMeshInstance::Render(ID3D11DeviceContext* deviceContext)
{
	deviceContext->IASetVertexBuffers(0, 1, &m_mesh->m_vertexBuffer, &m_mesh->m_vertexBufferStride, &m_mesh->m_vertexBufferOffset);
	deviceContext->IASetIndexBuffer(m_mesh->m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->DrawIndexed(m_mesh->m_indexCount, 0, 0);
}
