#pragma once

#include "IGuid.h"
#include "Material.h"


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


	void AddRenderObject(std::pair<std::shared_ptr<Material>, RendererBase*> data);

	std::array<std::vector<std::pair<std::shared_ptr<Material>, RendererBase*>>, RENDER_PASS::Count> _passObjects;

	void Release();
	friend class SceneManager;
};


