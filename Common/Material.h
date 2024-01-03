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
    ID3D11ShaderResourceView* m_diffuseRV = nullptr;        // ÅØ½ºÃ³ ¸®¼Ò½º ºä
    ID3D11ShaderResourceView* m_normalRV = nullptr;         // ÅØ½ºÃ³ ³ë¸»¸Ê ¸®¼Ò½º ºä
    ID3D11ShaderResourceView* m_specularRV = nullptr;       // ÅØ½ºÃ³ ½ºÆåÅ§·¯¸Ê ¸®¼Ò½º ºä
    ID3D11ShaderResourceView* m_emissiveRV = nullptr;       // ÅØ½ºÃ³ ÀÌ¹Ì½Ãºê¸Ê ¸®¼Ò½º ºä
    ID3D11ShaderResourceView* m_opacityRV = nullptr;        // ÅØ½ºÃ³ ¿ÀÆÄ½ÃÆ¼¸Ê ¸®¼Ò½º ºä
    ID3D11ShaderResourceView* m_metalicRV = nullptr;        // ÅØ½ºÃ³ ¿ÀÆÄ½ÃÆ¼¸Ê ¸®¼Ò½º ºä
    ID3D11ShaderResourceView* m_roughnessRV = nullptr;        // ÅØ½ºÃ³ ¿ÀÆÄ½ÃÆ¼¸Ê ¸®¼Ò½º ºä

public:
    void Create(aiMaterial* material);
    void SetFileName(const std::wstring& fileName);
};

