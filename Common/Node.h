#pragma once
#include "Mesh.h"
#include "Animation.h"

#include <string>
#include <vector>

struct aiScene;
struct aiNode;

class Model;

class Node
{
public:
    Node();
    ~Node();

public:
    string m_NodeName;

    Matrix m_NodeLocalTM;
    Matrix m_NodeWorldTM;

    Animation* m_Animation;

    Node* m_ParentNode;

    vector<Node> m_Children;
    vector<Mesh> m_Meshes;

public:
    void Create(ID3D11Device* device, Model* model, const aiScene* scene, const aiNode* node);
    void Update(float deltaTime, Model* model);
    void Render(ID3D11DeviceContext* deviceContext, Model* model);

private:
    Matrix GetParentWorldTransform(const Node* parentNode);
    void FindNodeAnimation(const aiScene* scene, const aiNode* node);
};

