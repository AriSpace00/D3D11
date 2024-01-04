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
    string m_nodeName;

    Matrix m_nodeLocalTM;
    Matrix m_nodeWorldTM;

    Animation* m_animation;

    Node* m_parentNode;

    vector<Node> m_children;
    vector<Mesh> m_meshes;

public:
    void Create(ID3D11Device* device, Model* model, const aiScene* scene, const aiNode* node);
    void Update(float deltaTime, Model* model);
    void Render(ID3D11DeviceContext* deviceContext, Model* model);

private:
    Matrix GetParentWorldTransform(const Node* parentNode);
    void FindNodeAnimation(const aiScene* scene, const aiNode* node);
};

    