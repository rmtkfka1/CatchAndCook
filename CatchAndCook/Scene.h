#pragma once

#include "IGuid.h"
#include "RendererBase.h"


class RendererBase;
class GameObject;

struct GlobalParam
{
	vec2 window_size;
	float Time;
	float padding;
};

class Scene : public IGuid
{


private:
	void AddGameObject(const std::shared_ptr<GameObject> gameObject);
	bool RemoveGameObject(const std::shared_ptr<GameObject>& gameObject);
	bool RemoveAtGameObject(int index);


public:
	void SetName(const std::string& name) { _name = name; };

	virtual void Init();
	virtual void Update();
	virtual void RenderBegin();
	virtual void Rendering();
	virtual void DebugRendering();
	virtual void RenderEnd();
	virtual void Finish();

	std::shared_ptr<GameObject> CreateGameObject(const std::wstring& name);
	std::shared_ptr<GameObject> Find(const std::wstring& name, bool includeDestroy = false);
	int Finds(const std::wstring& name, std::vector<std::shared_ptr<GameObject>>& vec, bool includeDestroy = false);
	void AddDestroyQueue(const std::shared_ptr<GameObject>& gameObject);

	void AddRenderer(Material* material, Mesh* mesh, RendererBase* renderBase);
	void AddRenderer(Mesh* mesh, RendererBase* renderBase, RENDER_PASS::PASS pass);

	void Release();
	friend class SceneManager;

protected:
	std::array<std::vector<RenderObjectStrucutre>, RENDER_PASS::Count> _passObjects;
	std::vector<std::shared_ptr<GameObject>> _dont_destroy_gameObjects;

	std::vector<std::shared_ptr<GameObject>> _gameObjects;
	std::queue<std::shared_ptr<GameObject>> _destroyQueue;


	std::string _name;

	GlobalParam _globalParam;
};


