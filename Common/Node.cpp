#include "pch.h"
#include "Node.h"
#include "Bone.h"
#include "Model.h"
#include "Helper.h"

#include <assimp/scene.h>

Node::Node()
    : m_parentNode(nullptr)
    , m_animation(nullptr)
{
}

Node::~Node()
{
}

void Node::Create(ID3D11Device* device, Model* model, const aiScene* scene, const aiNode* node)
{
    m_nodeName = node->mName.C_Str();
    m_nodeLocalTM = ConvertaiMatrixToXMMatrix(node->mTransformation);

    if (node->mParent != nullptr)
    {
        for (int i = 0; i < model->m_nodes.size(); i++)
        {
            if (node->mParent->mName.C_Str() == model->m_nodes[i]->m_nodeName)
            {
                m_parentNode = new Node();
                m_parentNode = model->m_nodes[i];
                break;
            }
        }
    }

    FindNodeAnimation(scene, node);

    model->m_animations.push_back(m_animation);
    model->m_nodes.push_back(this);

    for (int i = 0; i < model->m_meshes.size(); i++)
    {
        if (model->m_meshes[i].m_bones.size() > 0)
        {
            for (int j = 0; j < model->m_meshes[i].m_bones.size(); j++)
            {
                if (model->m_meshes[i].m_bones[j]->m_boneName == m_nodeName)
                {
                    model->m_meshes[i].m_bones[j]->m_nodeWorldMatrixPtr = &m_nodeWorldTM;
                }
            }
        }
    }

    if (node->mNumChildren > 0)
    {
        m_children.resize(node->mNumChildren);
        for (int i = 0; i < node->mNumChildren; i++)
        {
            m_children[i].Create(device, model, scene, node->mChildren[i]);
        }
    }
}

void Node::Update(float deltaTime, Model* model)
{
    m_animation->Update(deltaTime);

    if (!m_animation->m_animationKeys.empty())
    {
        if (model->m_isEvaluate)
        {
            m_animation->Evaluate();
            m_nodeLocalTM = m_animation->m_interpolationTM;
        }
        else
        {
            Matrix position = Matrix::CreateTranslation(m_animation->m_animationKeys[m_animation->m_curKeyIndex]->Position);
            Matrix rotation = Matrix::CreateFromQuaternion(m_animation->m_animationKeys[m_animation->m_curKeyIndex]->Rotation);
            Matrix scale = Matrix::CreateScale(m_animation->m_animationKeys[m_animation->m_curKeyIndex]->Scale);

            m_nodeLocalTM = scale * rotation * position;
        }
    }

    if (m_parentNode != nullptr)
    {
        m_nodeWorldTM = m_nodeLocalTM * GetParentWorldTransform(m_parentNode);
    }
    else
    {
        m_nodeWorldTM = m_nodeLocalTM;
    }
}

void Node::Render(ID3D11DeviceContext* deviceContext, Model* model)
{
    model->m_transform.WorldMatrix = XMMatrixTranspose(m_nodeWorldTM);
    deviceContext->UpdateSubresource(model->m_transformCB, 0, nullptr, &model->m_transform, 0, 0);
    deviceContext->VSSetConstantBuffers(0, 1, &model->m_transformCB);
    deviceContext->PSSetConstantBuffers(0, 1, &model->m_transformCB);

    for (size_t i = 0; i < model->m_meshes.size(); i++)
    {
        size_t mi = model->m_meshes[i].m_materialIndex;

        deviceContext->PSSetShaderResources(0, 1, &model->m_materials[mi].m_diffuseRV);
        deviceContext->PSSetShaderResources(1, 1, &model->m_materials[mi].m_normalRV);
        deviceContext->PSSetShaderResources(2, 1, &model->m_materials[mi].m_specularRV);
        deviceContext->PSSetShaderResources(3, 1, &model->m_materials[mi].m_emissiveRV);
        deviceContext->PSSetShaderResources(4, 1, &model->m_materials[mi].m_opacityRV);
        deviceContext->PSSetShaderResources(5, 1, &model->m_materials[mi].m_metalicRV);
        deviceContext->PSSetShaderResources(6, 1, &model->m_materials[mi].m_roughnessRV);

        model->m_material.UseDiffuseMap = model->m_materials[mi].m_diffuseRV != nullptr ? true : false;
        model->m_material.UseNormalMap = model->m_materials[mi].m_normalRV != nullptr ? true : false;
        model->m_material.UseSpecularMap = model->m_materials[mi].m_specularRV != nullptr ? true : false;
        model->m_material.UseEmissiveMap = model->m_materials[mi].m_emissiveRV != nullptr ? true : false;
        model->m_material.UseOpacityMap = model->m_materials[mi].m_opacityRV != nullptr ? true : false;
        model->m_material.UseMetalicMap = model->m_materials[mi].m_metalicRV != nullptr ? true : false;
        model->m_material.UseRoughnessMap = model->m_materials[mi].m_roughnessRV != nullptr ? true : false;

        if (model->m_material.UseOpacityMap)
        {
            deviceContext->OMSetBlendState(model->m_alphaBlendState, nullptr, 0xffffffff);
        }
        else
        {
            deviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
        }
        deviceContext->UpdateSubresource(model->m_materialCB, 0, nullptr, &model->m_material, 0, 0);

        model->m_meshes[i].UpdateMatrixPalette(model->m_matrixPalette.Array);

        for (const auto& matrix : model->m_matrixPalette.Array)
        {
            assert(matrix._41 == 0.f);
            assert(matrix._42 == 0.f);
            assert(matrix._43 == 0.f);
            assert(matrix._44 == 1.f);
        }

        deviceContext->UpdateSubresource(model->m_matrixPaletteCB, 0, nullptr, &model->m_matrixPalette, 0, 0);
        deviceContext->VSSetConstantBuffers(3, 1, &model->m_matrixPaletteCB);
        deviceContext->PSSetConstantBuffers(3, 1, &model->m_matrixPaletteCB);

        deviceContext->IASetVertexBuffers(0, 1, &model->m_meshes[i].m_vertexBuffer, &model->m_meshes[i].m_vertexBufferStride, &model->m_meshes[i].m_vertexBufferOffset);
        deviceContext->IASetIndexBuffer(model->m_meshes[i].m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
        deviceContext->DrawIndexed(model->m_meshes[i].m_indexCount, 0, 0);
    }
}

Matrix Node::GetParentWorldTransform(const Node* parentNode)
{
    Matrix parentLocalTM = parentNode->m_nodeLocalTM;

    if (parentNode->m_parentNode != nullptr)
    {
        Matrix parentWorldTM = parentLocalTM * GetParentWorldTransform(parentNode->m_parentNode);
        return parentWorldTM;
    }
    else
    {
        return parentLocalTM;
    }
}

void Node::FindNodeAnimation(const aiScene* scene, const aiNode* node)
{
    m_animation = new Animation();

    if (scene->mNumAnimations > 0)
    {
        for (int i = 0; i < scene->mNumAnimations; i++)
        {
            if (scene->mAnimations[i]->mNumChannels > 0)
            {
                for (int j = 0; j < scene->mAnimations[i]->mNumChannels; j++)
                {
                    if (m_nodeName == scene->mAnimations[i]->mChannels[j]->mNodeName.C_Str())
                    {
                        aiNodeAnim* nodeAnimPtr = scene->mAnimations[i]->mChannels[j];
                        m_animation->Create(nodeAnimPtr);
                    }
                }
            }
        }
    }
}
