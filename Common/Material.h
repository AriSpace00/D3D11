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
    ID3D11ShaderResourceView* m_DiffuseRV = nullptr;        // ÅØ½ºÃ³ ¸®¼Ò½º ºä
    ID3D11ShaderResourceView* m_NormalRV = nullptr;         // ÅØ½ºÃ³ ³ë¸»¸Ê ¸®¼Ò½º ºä
    ID3D11ShaderResourceView* m_SpecularRV = nullptr;       // ÅØ½ºÃ³ ½ºÆåÅ§·¯¸Ê ¸®¼Ò½º ºä
    ID3D11ShaderResourceView* m_EmissiveRV = nullptr;       // ÅØ½ºÃ³ ÀÌ¹Ì½Ãºê¸Ê ¸®¼Ò½º ºä
    ID3D11ShaderResourceView* m_OpacityRV = nullptr;        // ÅØ½ºÃ³ ¿ÀÆÄ½ÃÆ¼¸Ê ¸®¼Ò½º ºä
    ID3D11ShaderResourceView* m_RoughnessView = nullptr;        // ÅØ½ºÃ³ ¿ÀÆÄ½ÃÆ¼¸Ê ¸®¼Ò½º ºä

public:
    void Create(ID3D11Device* device, aiMaterial* material);
    void SetFileName(const std::wstring& fileName);
};

