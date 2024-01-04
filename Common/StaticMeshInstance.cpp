#include "pch.h"
#include "StaticMeshInstance.h"
#include "StaticMesh.h"

StaticMeshInstance::StaticMeshInstance()
	: m_mesh(nullptr)
	, m_material(nullptr)
	, m_nodeWorldTM(nullptr)
{
}

StaticMeshInstance::~StaticMeshInstance()
{
}

void StaticMeshInstance::Create(StaticMesh* mesh, Matrix* worldTM, Material* material)
{
	m_mesh = mesh;
	m_nodeWorldTM = worldTM;
	m_material = material;
}

void StaticMeshInstance::Render(ID3D11DeviceContext* deviceContext)
{
	deviceContext->IASetIndexBuffer(m_mesh->m_indexBuffer, )
}
