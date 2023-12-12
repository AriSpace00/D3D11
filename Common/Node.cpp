#include "pch.h"
#include "Node.h"
#include "Bone.h"
#include "Model.h"
#include "Helper.h"

#include <assimp/scene.h>

Node::Node()
    : m_ParentNode(nullptr)
    , m_Animation(nullptr)
{
}

Node::~Node()
{
}

void Node::Create(ID3D11Device* device, Model* model, const aiScene* scene, const aiNode* node)
{
    m_NodeName = node->mName.C_Str();
    m_NodeLocalTM = ConvertaiMatrixToXMMatrix(node->mTransformation);

    if (node->mParent != nullptr)
    {
        for (int i = 0; i < model->m_Nodes.size(); i++)
        {
            if (node->mParent->mName.C_Str() == model->m_Nodes[i]->m_NodeName)
            {
                m_ParentNode = new Node();
                m_ParentNode = model->m_Nodes[i];
                break;
            }
        }
    }

    FindNodeAnimation(scene, node);

    model->m_Animations.push_back(m_Animation);
    model->m_Nodes.push_back(this);

    for (int i = 0; i < model->m_Meshes.size(); i++)
    {
        if (model->m_Meshes[i].m_Bones.size() > 0)
        {
            for (int j = 0; j < model->m_Meshes[i].m_Bones.size(); j++)
            {
                if (model->m_Meshes[i].m_Bones[j]->m_BoneName == m_NodeName)
                {
                    model->m_Meshes[i].m_Bones[j]->m_NodeWorldMatrixPtr = &m_NodeWorldTM;
                }
            }
        }
    }

    if (node->mNumChildren > 0)
    {
        m_Children.resize(node->mNumChildren);
        for (int i = 0; i < node->mNumChildren; i++)
        {
            m_Children[i].Create(device, model, scene, node->mChildren[i]);
        }
    }
}

void Node::Update(float deltaTime, Model* model)
{
    m_Animation->Update(deltaTime);

    if (!m_Animation->m_AnimationKeys.empty())
    {
        if (model->m_IsEvaluate)
        {
            m_Animation->Evaluate();
            m_NodeLocalTM = m_Animation->m_InterpolationTM;
        }
        else
        {
            Matrix position = Matrix::CreateTranslation(m_Animation->m_AnimationKeys[m_Animation->m_CurKeyIndex]->Position);
            Matrix rotation = Matrix::CreateFromQuaternion(m_Animation->m_AnimationKeys[m_Animation->m_CurKeyIndex]->Rotation);
            Matrix scale = Matrix::CreateScale(m_Animation->m_AnimationKeys[m_Animation->m_CurKeyIndex]->Scale);

            m_NodeLocalTM = scale * rotation * position;
        }
    }

    if (m_ParentNode != nullptr)
    {
        m_NodeWorldTM = m_NodeLocalTM * GetParentWorldTransform(m_ParentNode);
    }
    else
    {
        m_NodeWorldTM = m_NodeLocalTM;
    }
}

void Node::Render(ID3D11DeviceContext* deviceContext, Model* model)
{
    model->m_Transform.WorldMatrix = XMMatrixTranspose(m_NodeWorldTM);
    deviceContext->UpdateSubresource(model->m_CBTransform, 0, nullptr, &model->m_Transform, 0, 0);
    deviceContext->VSSetConstantBuffers(0, 1, &model->m_CBTransform);
    deviceContext->PSSetConstantBuffers(0, 1, &model->m_CBTransform);

    for (size_t i = 0; i < model->m_Meshes.size(); i++)
    {
        size_t mi = model->m_Meshes[i].m_MaterialIndex;

        deviceContext->PSSetShaderResources(0, 1, &model->m_Materials[mi].m_DiffuseRV);
        deviceContext->PSSetShaderResources(1, 1, &model->m_Materials[mi].m_NormalRV);
        deviceContext->PSSetShaderResources(2, 1, &model->m_Materials[mi].m_SpecularRV);
        deviceContext->PSSetShaderResources(3, 1, &model->m_Materials[mi].m_EmissiveRV);
        deviceContext->PSSetShaderResources(4, 1, &model->m_Materials[mi].m_OpacityRV);

        model->m_Material.UseDiffuseMap = model->m_Materials[mi].m_DiffuseRV != nullptr ? true : false;
        model->m_Material.UseNormalMap = model->m_Materials[mi].m_NormalRV != nullptr ? true : false;
        model->m_Material.UseSpecularMap = model->m_Materials[mi].m_SpecularRV != nullptr ? true : false;
        model->m_Material.UseEmissiveMap = model->m_Materials[mi].m_EmissiveRV != nullptr ? true : false;
        model->m_Material.UseOpacityMap = model->m_Materials[mi].m_OpacityRV != nullptr ? true : false;

        if (model->m_Material.UseOpacityMap)
        {
            deviceContext->OMSetBlendState(model->m_AlphaBlendState, nullptr, 0xffffffff);
        }
        else
        {
            deviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
        }
        deviceContext->UpdateSubresource(model->m_CBMaterial, 0, nullptr, &model->m_Material, 0, 0);

        model->m_Meshes[i].UpdateMatrixPalette(model->m_MatrixPalette.Array);

        for (const auto& matrix : model->m_MatrixPalette.Array)
        {
            assert(matrix._41 == 0.f);
            assert(matrix._42 == 0.f);
            assert(matrix._43 == 0.f);
            assert(matrix._44 == 1.f);
        }

        deviceContext->UpdateSubresource(model->m_CBMatrixPalette, 0, nullptr, &model->m_MatrixPalette, 0, 0);
        deviceContext->VSSetConstantBuffers(3, 1, &model->m_CBMatrixPalette);
        deviceContext->PSSetConstantBuffers(3, 1, &model->m_CBMatrixPalette);

        deviceContext->IASetVertexBuffers(0, 1, &model->m_Meshes[i].m_VertexBuffer, &model->m_Meshes[i].m_VertexBufferStride, &model->m_Meshes[i].m_VertexBufferOffset);
        deviceContext->IASetIndexBuffer(model->m_Meshes[i].m_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
        deviceContext->DrawIndexed(model->m_Meshes[i].m_IndexCount, 0, 0);
    }
}

Matrix Node::GetParentWorldTransform(const Node* parentNode)
{
    Matrix parentLocalTM = parentNode->m_NodeLocalTM;

    if (parentNode->m_ParentNode != nullptr)
    {
        Matrix parentWorldTM = parentLocalTM * GetParentWorldTransform(parentNode->m_ParentNode);
        return parentWorldTM;
    }
    else
    {
        return parentLocalTM;
    }
}

void Node::FindNodeAnimation(const aiScene* scene, const aiNode* node)
{
    m_Animation = new Animation();

    if (scene->mNumAnimations > 0)
    {
        for (int i = 0; i < scene->mNumAnimations; i++)
        {
            if (scene->mAnimations[i]->mNumChannels > 0)
            {
                for (int j = 0; j < scene->mAnimations[i]->mNumChannels; j++)
                {
                    if (m_NodeName == scene->mAnimations[i]->mChannels[j]->mNodeName.C_Str())
                    {
                        aiNodeAnim* nodeAnimPtr = scene->mAnimations[i]->mChannels[j];
                        m_Animation->Create(nodeAnimPtr);
                    }
                }
            }
        }
    }
}
