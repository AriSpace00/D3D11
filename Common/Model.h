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
    std::vector<Node*> m_Nodes;
    std::vector<Mesh> m_Meshes;
    std::vector<Material> m_Materials;
    std::vector<Animation*> m_Animations;

    std::wstring m_FileName;
    bool IsFileLoad;

    CB_Transform m_Transform;
    CB_Material m_Material;
    CB_MatrixPalette m_MatrixPalette;

    ID3D11Buffer* m_CBTransform = nullptr;                  
    ID3D11Buffer* m_CBMaterial = nullptr;                   
    ID3D11Buffer* m_CBMatrixPalette = nullptr;              

    ID3D11BlendState* m_AlphaBlendState = nullptr;          

    Matrix m_Position;
    Matrix m_Rotation;
    Matrix m_Scale;

    bool m_IsEvaluate;

public:
    void ReadFile(ID3D11Device* device, const std::string& path);

    void Update(const float& deltaTime);
    void Render(ID3D11DeviceContext* deviceContext);

    void SetTransform(Matrix position, Matrix rotation, Matrix scale);
};

