#pragma once

#include "pch.h"
#include <string>

struct aiMaterial;

struct VertexMaterial
{
    DirectX::XMFLOAT4 Diffuse = {1.0f, 0.0f, 0.0f, 1.0f};
};

class MaterialTexture
{
public:
    MaterialTexture();
    ~MaterialTexture();

    std::wstring m_filePath;
    ComPtr<ID3D11ShaderResourceView> m_textureRV = nullptr;
    void Create(const std::wstring& filePath);
    void Create(const DirectX::XMFLOAT4& value);
};

class Material
{
public:
    Material();
    ~Material();

private:
    std::wstring m_fileName;

public:
    shared_ptr<MaterialTexture> m_diffuseRV = nullptr;        // �ؽ�ó ���ҽ� ��
    shared_ptr<MaterialTexture> m_normalRV = nullptr;         // �ؽ�ó �븻�� ���ҽ� ��
    shared_ptr<MaterialTexture> m_specularRV = nullptr;       // �ؽ�ó ����ŧ���� ���ҽ� ��
    shared_ptr<MaterialTexture> m_emissiveRV = nullptr;       // �ؽ�ó �̹̽ú�� ���ҽ� ��
    shared_ptr<MaterialTexture> m_opacityRV = nullptr;        // �ؽ�ó ���Ľ�Ƽ�� ���ҽ� ��
    shared_ptr<MaterialTexture> m_metalicRV = nullptr;        // �ؽ�ó ���Ľ�Ƽ�� ���ҽ� ��
    shared_ptr<MaterialTexture> m_roughnessRV = nullptr;      // �ؽ�ó ���Ľ�Ƽ�� ���ҽ� ��

public:
    void Create(aiMaterial* material);
    void SetFileName(const std::wstring& fileName);
};

