#pragma once
#include "Component.h"
#include "IDelayDestroy.h"
#include "IGuid.h"
#include "Transform.h"

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
	void Start(); // ù ������ // ù ������ ��.
	void Update(); //
	void Update2(); // 
	void Enable(); // 
	void Disable(); //
	void Destroy(); //

	void RenderBegin(); // CBuffer <- ����
	void Rendering(); // CBuffer <- ����

	// �̺�Ʈ �Լ�
	void Collision(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other);
	void DebugRendering(); // CBuffer <- ����

public:
	template <class T, class = std::enable_if_t<std::is_base_of_v<Component, T>>>
	std::shared_ptr<T> AddComponent()
	{
		std::shared_ptr<T> component = std::make_shared<T>();
		component->SetOwner(GetCast<GameObject>());
		_components.push_back(component);
		component->Init();
		return component;
	};

	template <class T, class = std::enable_if_t<std::is_base_of_v<Component, T>>>
	void AddComponent(const std::shared_ptr<T>& component)
	{
		if (component == nullptr)
			return;
		std::shared_ptr<Component> parentComponent = std::static_pointer_cast<Component>(component);
		auto iter = std::find(_components.begin(), _components.end(), parentComponent);
		if (iter != _components.end())
			return;
		if (component->GetOwner() != nullptr)
			component->GetOwner()->DisconnectComponent(component);

		component->SetOwner(GetCast<GameObject>());
		_components.push_back(parentComponent);
		component->Init();
	}

	template <class T, class = std::enable_if_t<std::is_base_of_v<Component, T>>>
	bool DisconnectComponent(const std::shared_ptr<T>& component)
	{
		if (component == nullptr)
			return false;
		std::shared_ptr<Component> parentComponent = std::static_pointer_cast<Component>(component);
		auto iter = std::find(_components.begin(), _components.end(), parentComponent);
		if (iter != _components.end())
		{
			_components.erase(iter);
			return true;
		}
		return false;
	};
	bool DisconnectAtComponent(int index)
	{
		_components.erase(_components.begin() + index);
		return true;
	};

	template <class T, class = std::enable_if_t<std::is_base_of_v<Component, T>>>
	bool RemoveComponent(const std::shared_ptr<T>& component)
	{
		return false;
	};
	bool RemoveAtComponent(int index)
	{
		return false;
	};

	template <class T, class = std::enable_if_t<std::is_base_of_v<Component, T>>>
	std::shared_ptr<T> GetComponent()
	{
		auto iter = std::find_if(_components.begin(), _components.end(), 
			[&](const std::shared_ptr<Component>& component) {
				return std::dynamic_pointer_cast<T>(component) != nullptr;
			});
		if (iter == _components.end())
			return nullptr;
		return std::static_pointer_cast<T>(*iter);
	}

	template <class T, class = std::enable_if_t<std::is_base_of_v<Component, T>>>
	int GetComponents(std::vector<std::shared_ptr<T>>& vec)
	{
		int prevCount = vec.size();
		for (auto& component : _components)
		{
			std::shared_ptr<T> downCast = std::dynamic_pointer_cast<T>(component);
			if (downCast != nullptr)
				vec.push_back(vec);
		}
		return vec.size() - prevCount;
	}

	template <class T, class = std::enable_if_t<std::is_base_of_v<Component, T>>>
	int GetComponentsWithChilds(std::vector<std::shared_ptr<T>>& vec)
	{
		int count = vec.size();
		for (auto& component : _components)
		{
			std::shared_ptr<T> downCast = std::dynamic_pointer_cast<T>(component);
			if (downCast != nullptr)
				vec.push_back(vec);
		}
		count = vec.size() - count;
		for (auto& child : _childs)
			if (!child.expired())
				count += child.lock()->GetComponentsWithChilds<T>(vec);

		return count;
	}




private:
	bool AddChild(const std::shared_ptr<GameObject>& obj);
	bool RemoveChild(const std::shared_ptr<GameObject>& obj);
	bool RemoveAtChild(int index);
	void SetRootParent(const std::shared_ptr<GameObject>& rootParent);

public:
	bool SetParent(const std::shared_ptr<GameObject>& nextParentObj);
	bool IsInParent(const std::shared_ptr<GameObject>& obj);

	std::shared_ptr<GameObject> GetChild(int index);
	std::shared_ptr<GameObject> GetChildByName(const std::wstring& name);
	int GetChildAll(std::vector<std::shared_ptr<GameObject>>& vec);
	int GetChildsByName(const std::wstring& name, std::vector<std::shared_ptr<GameObject>>& vec);
	int GetChildsAll(std::vector<std::shared_ptr<GameObject>>& vec);
	int GetChildsAllByName(const std::wstring& name, std::vector<std::shared_ptr<GameObject>>& vec);




private:
	bool _active_self = true;
	bool _active_total_prev = false;
	bool _active_total = true;
public:
	bool GetActive(); //_active_total
	bool GetActiveSelf(); // _active_self
	bool SetActiveSelf(bool _active); //_active_self
private:
	void SyncActivePrev();
	void SetActivePrev(bool activeTotalPrev);
	bool CheckActiveUpdated();
	void ActiveUpdateChain(bool _active_total);


public:


	void SetName(const std::wstring name){_name = name;};
	std::wstring& GetName() { return _name; };

	std::shared_ptr<Scene> GetScene() { assert(!_scene.expired()); return _scene.lock(); };
	void SetScene(const std::shared_ptr<Scene>& scene) { _scene = scene; };

	std::vector<std::shared_ptr<Component>>& GetComponentAll() { return _components; };

public:
	ObjectTag tag = ObjectTag::defualt;
	std::shared_ptr<Transform> transform;

private:
	std::wstring _name = L"none";
	std::weak_ptr<Scene> _scene;
	std::vector<std::shared_ptr<Component>> _components;

	std::weak_ptr<GameObject> parent;
	std::weak_ptr<GameObject> rootParent;
	std::vector<std::weak_ptr<GameObject>> _childs;

	friend class Transform;
};
