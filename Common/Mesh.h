#pragma once

struct Vertex
{
    Vector3 Position;
    Vector2 Texcoord;
    Vector3 Normal;
    Vector3 Tangent;
};

struct BoneVertexWeight
{
    Vector3 Position;
    Vector2 Texcoord;
    Vector3 Normal;
    Vector3 Tangent;

    int BlendIndices[4] = {};
    float BlendWeights[4] = {};

    void AddBoneData(unsigned int boneIndex, float weight)
    {
        assert(BlendWeights[0] == 0.0f || BlendWeights[1] == 0.0f || BlendWeights[2] == 0.0f || BlendWeights[3] == 0.0f);
        for (int i = 0; i < 4; i++)
        {
            if (BlendWeights[i] == 0.0f)
            {
                BlendIndices[i] = boneIndex;
                BlendWeights[i] = weight;
                return;
            }
        }
    }
};

struct aiMesh;

class Model;
class Bone;

class Mesh
{
public:
    Mesh();
    ~Mesh();

public:
    ID3D11Buffer* m_VertexBuffer;
    ID3D11Buffer* m_IndexBuffer;

    UINT m_VertexBufferStride = 0;    // 버텍스 하나의 크기
    UINT m_VertexBufferOffset = 0;    // 버텍스 버퍼의 오프셋
    UINT m_VertexCount = 0;           // 버텍스 개수 
    UINT m_IndexCount = 0;            // 인덱스 개수
    UINT m_MaterialIndex = 0;         // 머테리얼 인덱스

    vector<Vertex> m_Vertices;
    vector<Bone*> m_Bones;
    vector<BoneVertexWeight> m_BoneWeightVertices;
    vector<WORD> m_Indices;

private:
    void CreateVertexBuffer(ID3D11Device* device, const vector<Vertex>& vertices, UINT vertexCount);
    void CreateBoneVertexWeightBuffer(ID3D11Device* device, const vector<BoneVertexWeight>& vertices, UINT vertexCount);
    void CreateIndexBuffer(ID3D11Device* device, const vector<WORD>& indices, UINT indexCount);

public:
    void Create(ID3D11Device* device, aiMesh* mesh);
    void UpdateMatrixPalette(Matrix* matrixPalettePtr);
};

