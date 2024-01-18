#pragma once
class Transform
{
public:
	Transform();
	~Transform();

public:
	Transform* m_parent;

	Matrix m_localTM;
	Matrix m_worldTM;

public:
	void SetParent(Transform* parent);
	virtual void Update(float deltaTime);
};

