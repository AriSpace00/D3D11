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
    shared_ptr<MaterialTexture> m_diffuseRV = nullptr;        // ÅØ½ºÃ³ ¸®¼Ò½º ºä
    shared_ptr<MaterialTexture> m_normalRV = nullptr;         // ÅØ½ºÃ³ ³ë¸»¸Ê ¸®¼Ò½º ºä
    shared_ptr<MaterialTexture> m_specularRV = nullptr;       // ÅØ½ºÃ³ ½ºÆåÅ§·¯¸Ê ¸®¼Ò½º ºä
    shared_ptr<MaterialTexture> m_emissiveRV = nullptr;       // ÅØ½ºÃ³ ÀÌ¹Ì½Ãºê¸Ê ¸®¼Ò½º ºä
    shared_ptr<MaterialTexture> m_opacityRV = nullptr;        // ÅØ½ºÃ³ ¿ÀÆÄ½ÃÆ¼¸Ê ¸®¼Ò½º ºä
    shared_ptr<MaterialTexture> m_metalicRV = nullptr;        // ÅØ½ºÃ³ ¿ÀÆÄ½ÃÆ¼¸Ê ¸®¼Ò½º ºä
    shared_ptr<MaterialTexture> m_roughnessRV = nullptr;      // ÅØ½ºÃ³ ¿ÀÆÄ½ÃÆ¼¸Ê ¸®¼Ò½º ºä

public:
    void Create(aiMaterial* material);
    void SetFileName(const std::wstring& fileName);
};

