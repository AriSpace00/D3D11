#pragma once
#include "pch.h"

#include "Mesh.h"
#include "Material.h"
#include "Animation.h"

struct CB_Transform
{
    Matrix WorldMatrix;
    Matrix ViewMatrix;
    Matrix ProjectionMatrix;
};

struct CB_Material
{
    Vector4 Ambient = { 1.0f,1.0f,1.0f,1.0f };
    Vector4 Diffuse = { 1.0f,1.0f,1.0f,1.0f };
    Vector4 Specular = { 1.0f,1.0f,1.0f,1.0f };
    Vector4 Emissive = { 1.0f,1.0f,1.0f,1.0f };
    float Metalic;
    float Roughness;
    float SpecularPower = 80;
    float UseDiffuseMap;
    float UseNormalMap;
    float UseSpecularMap;
    float UseEmissiveMap;
    float UseOpacityMap;
    float UseMetalicMap;
    float UseRoughnessMap;
    Vector2 Material_pad0;
};

struct CB_MatrixPalette
{
    Matrix Array[128];
};

class Node;

class Model
{
public:
    Model();
    ~Model();

public:
    std::vector<Node*> m_nodes;
    std::vector<Mesh> m_meshes;
    std::vector<Material> m_materials;
    std::vector<Animation*> m_animations;

    std::wstring m_fileName;
    bool isFileLoad;

    CB_Transform m_transform;
    CB_Material m_material;
    CB_MatrixPalette m_matrixPalette;

    ID3D11Buffer* m_transformCB = nullptr;                  
    ID3D11Buffer* m_materialCB = nullptr;                   
    ID3D11Buffer* m_matrixPaletteCB = nullptr;              

    ID3D11BlendState* m_alphaBlendState = nullptr;          

    Matrix m_position;
    Matrix m_rotation;
    Matrix m_scale;

    bool m_isEvaluate;

public:
    void ReadFile(ID3D11Device* device, const std::string& path);

    void Update(const float& deltaTime);
    void Render(ID3D11DeviceContext* deviceContext);

    void SetTransform(Matrix position, Matrix rotation, Matrix scale);
};

