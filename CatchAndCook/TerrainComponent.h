#pragma once
#include "Component.h"

class Terrain
{
public:
	//void Init(std::wstring path, )

	Vector2 _heightSize = Vector2(513,513); // 2의 배수 + 1 (예:513x513)
	std::vector<float> _heightRawData;
	float& GetHeightXY(const int& x, const int& y) { return _heightRawData[y * _heightSize.x + x]; };

};

class TerrainComponent : public Component
{
public:
	~TerrainComponent() override;
	bool IsExecuteAble() override;
	void Init() override;
	void Start() override;
	void Update() override;
	void Update2() override;
	void Enable() override;
	void Disable() override;
	void RenderBegin() override;
	void Collision(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void SetDestroy() override;
	void Destroy() override;

	float GetHeight(Vector3 localPosition);
	float& GetHeightXY(int x, int y);

private:
	std::shared_ptr<Terrain> _terrain;
};

