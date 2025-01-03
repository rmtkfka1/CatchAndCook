#pragma once
#include "Component.h"
#include "IDelayDestroy.h"
#include "IGuid.h"

class Transform;
class Scene;

enum class ObjectTag
{
	defualt = 1 << 0,
};

class GameObject : public IGuid, public IDelayDestroy
{
public:
	GameObject();
	GameObject(std::wstring name);
	virtual ~GameObject() override;

	void Init();
	void Start(); // 첫 프레임 // 첫 프레임 시.
	void Update(); //
	void Update2(); // 
	void Enable(); // 
	void Disable(); //
	void Destroy(); //

	void RenderBegin(); // CBuffer <- 갱신
	void Rendering(); // CBuffer <- 갱신

	// 이벤트 함수
	void Collision(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other);
	void DebugRendering(); // CBuffer <- 갱신

public:



public:


	void SetName(const std::wstring name){_name = name;};
	std::wstring& GetName() { return _name; };

	std::shared_ptr<Scene> GetScene() { assert(!_scene.expired()); return _scene.lock(); };
	void SetScene(const std::shared_ptr<Scene>& scene) { _scene = scene; };

	std::vector<std::shared_ptr<Component>>& GetComponentAll() { return _component; };

public:
	ObjectTag tag = ObjectTag::defualt;
	std::shared_ptr<Transform> transform;

private:
	std::wstring _name = L"none";
	std::weak_ptr<Scene> _scene;
	std::vector<std::shared_ptr<Component>> _component;
};

