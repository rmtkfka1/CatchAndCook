#pragma once

#include "IGuid.h"
#include "Material.h"
#include "RendererBase.h"


class RendererBase;
class GameObject;

class Scene : public IGuid
{
	std::string _name;

	std::vector<std::shared_ptr<GameObject>> _dont_destroy_gameObjects;
	std::vector<std::shared_ptr<GameObject>> _gameObjects;

private:
	void AddGameObject(const std::shared_ptr<GameObject> gameObject);
	bool RemoveGameObject(const std::shared_ptr<GameObject>& gameObject);
	bool RemoveAtGameObject(int index);

public:
	void SetName(const std::string& name) { _name = name; };
	void Init(const std::string& name);

	virtual void Update();
	virtual void RenderBegin();
	virtual void Rendering();
	virtual void RenderEnd();
	virtual void Finish();


	std::shared_ptr<GameObject> CreateGameObject(const std::wstring& name);
	std::shared_ptr<GameObject> Find(const std::wstring& name, bool includeDestroy = false);
	int Finds(const std::wstring& name, std::vector<std::shared_ptr<GameObject>>& vec, bool includeDestroy = false);


	void AddRenderer(std::shared_ptr<Material> material, shared_ptr<RendererBase> data);
	void AddRenderer(shared_ptr<RendererBase> data, RENDER_PASS::PASS pass);

	std::array<std::vector<RenderObjectStrucutre>, RENDER_PASS::Count> _passObjects;

	void Release();
	friend class SceneManager;
};


