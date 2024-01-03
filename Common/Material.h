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
    std::wstring m_fileName;

public:
    ID3D11ShaderResourceView* m_diffuseRV = nullptr;        // �ؽ�ó ���ҽ� ��
    ID3D11ShaderResourceView* m_normalRV = nullptr;         // �ؽ�ó �븻�� ���ҽ� ��
    ID3D11ShaderResourceView* m_specularRV = nullptr;       // �ؽ�ó ����ŧ���� ���ҽ� ��
    ID3D11ShaderResourceView* m_emissiveRV = nullptr;       // �ؽ�ó �̹̽ú�� ���ҽ� ��
    ID3D11ShaderResourceView* m_opacityRV = nullptr;        // �ؽ�ó ���Ľ�Ƽ�� ���ҽ� ��
    ID3D11ShaderResourceView* m_metalicRV = nullptr;        // �ؽ�ó ���Ľ�Ƽ�� ���ҽ� ��
    ID3D11ShaderResourceView* m_roughnessRV = nullptr;        // �ؽ�ó ���Ľ�Ƽ�� ���ҽ� ��

public:
    void Create(aiMaterial* material);
    void SetFileName(const std::wstring& fileName);
};

