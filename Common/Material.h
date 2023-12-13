#pragma once

#include "pch.h"
#include <string>

struct aiMaterial;

class Material
{
public:
    Material();
    ~Material();

private:
    std::wstring FBXFileName;

public:
    ID3D11ShaderResourceView* m_DiffuseRV = nullptr;        // �ؽ�ó ���ҽ� ��
    ID3D11ShaderResourceView* m_NormalRV = nullptr;         // �ؽ�ó �븻�� ���ҽ� ��
    ID3D11ShaderResourceView* m_SpecularRV = nullptr;       // �ؽ�ó ����ŧ���� ���ҽ� ��
    ID3D11ShaderResourceView* m_EmissiveRV = nullptr;       // �ؽ�ó �̹̽ú�� ���ҽ� ��
    ID3D11ShaderResourceView* m_OpacityRV = nullptr;        // �ؽ�ó ���Ľ�Ƽ�� ���ҽ� ��
    ID3D11ShaderResourceView* m_RoughnessView = nullptr;        // �ؽ�ó ���Ľ�Ƽ�� ���ҽ� ��

public:
    void Create(ID3D11Device* device, aiMaterial* material);
    void SetFileName(const std::wstring& fileName);
};

