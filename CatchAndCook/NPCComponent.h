#pragma once


enum class NPCStateType
{
	stay,
	goto_any,
	goto_shop_in,
	goto_shop_out,
};

class NPCComponent : public Component
{
public:
	~NPCComponent() override;
	bool IsExecuteAble() override;
	void Init() override;
	void Start() override;
	void Update() override;
	void MoveControl();
	void Update2() override;
	void Enable() override;
	void Disable() override;
	void RenderBegin() override;
	void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current) override;
	void SetDestroy() override;
	void Destroy() override;

	NPCStateType type = NPCStateType::stay;

	std::vector<Vector3> gotoPoints;
	std::vector<Vector3> paths;
	Vector3 velocity;
	Vector3 lookDirection = Vector3::Forward;
	bool isGround = true;
};

