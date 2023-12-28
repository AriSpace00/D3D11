#pragma once

struct Vertex
{
    Vector3 Position;
    Vector2 Texcoord;
    Vector3 Normal;
    Vector3 Tangent;
};

struct aiMesh;
class Material;

class StaticMesh
{
public:
    StaticMesh();
    ~StaticMesh();

public:
    ID3D11Buffer* m_vertexBuffer;
    ID3D11Buffer* m_indexBuffer;

    UINT m_vertexBufferStride = 0;    // 버텍스 하나의 크기
    UINT m_vertexBufferOffset = 0;    // 버텍스 버퍼의 오프셋
    UINT m_vertexCount = 0;           // 버텍스 개수 
    UINT m_indexCount = 0;            // 인덱스 개수
    UINT m_materialIndex = 0;         // 머테리얼 인덱스

    vector<Vertex> m_vertices;
    vector<UINT> m_indices;

private:
    void CreateVertexBuffer(ID3D11Device* device, const vector<Vertex>& vertices, UINT vertexCount);
    void CreateIndexBuffer(ID3D11Device* device, const vector<UINT>& indices, UINT indexCount);

public:
    void Create(ID3D11Device* device, aiMesh* mesh);
};

class StaticMeshResource
{
public:
    StaticMeshResource();
    ~StaticMeshResource();

public:
    std::wstring m_fileName;
    std::vector<StaticMesh> m_meshes;
    std::vector<Material> m_materials;

public:
    void CreateResource(ID3D11Device* device, const std::string& path);
};

