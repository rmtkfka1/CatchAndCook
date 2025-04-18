﻿#pragma once

#include "GameObjectSetting.h"
#include "IGuid.h"
#include "RendererBase.h"
#include "SceneType.h"

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

public:
	void AddGameObject(const std::shared_ptr<GameObject>& gameObject);
	bool RemoveGameObject(const std::shared_ptr<GameObject>& gameObject);
	bool RemoveAtGameObject(int index);
	void CameraControll();


	void ExecuteDestroyGameObjects();
	void GlobalSetting();

private:
	void UiPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> & cmdList);
	void TransparentPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> & cmdList);
	void ForwardPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> & cmdList);
	void DeferredPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> & cmdList);
	void ShadowPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> & cmdList);
	void FinalRender(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> & cmdList);
	void ComputePass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> & cmdList);

private:
	virtual void SettingPrevData(RenderObjectStrucutre& data, const RENDER_PASS::PASS& pass);

public:
	void SetName(const std::string& name) { _name = name; };

	virtual void Init();
	virtual void Update();
	virtual void RenderBegin();
	virtual void Rendering();
	virtual void DebugRendering();
	virtual void RenderEnd();
	virtual void Finish();

	std::shared_ptr<GameObject> CreateGameObject(const std::wstring& name,GameObjectType type = GameObjectType::Dynamic);
	std::shared_ptr<GameObject> Find(const std::wstring& name, bool includeDestroy = false);
	int Finds(const std::wstring& name, std::vector<std::shared_ptr<GameObject>>& vec, bool includeDestroy = false);
	void AddChangeTypeQueue(const std::shared_ptr<GameObject>& gmaeObject, GameObjectType type);
	void AddStartQueue(const std::shared_ptr<GameObject>& gameObject);
	void AddDestroyQueue(const std::shared_ptr<GameObject>& gameObject);

	void AddRenderer(Material* material, Mesh* mesh, RendererBase* renderBase);
	void AddRenderer(Mesh* mesh, RendererBase* renderBase, RENDER_PASS::PASS pass);
	SceneType GetSceneType() { return _type; }

	void Release();
	friend class SceneManager;
	friend class SceneLoader;

protected:
	std::array<unordered_map<shared_ptr<Shader>,std::vector<RenderObjectStrucutre>>, RENDER_PASS::Count> _passObjects;
	std::vector<std::shared_ptr<GameObject>> _dont_destroy_gameObjects;

public:
	std::vector<std::shared_ptr<GameObject>> _gameObjects; 
	std::vector<std::shared_ptr<GameObject>> _gameObjects_deactivate;

	std::queue<std::shared_ptr<GameObject>> _destroyQueue;
	std::queue<std::shared_ptr<Component>> _destroyComponentQueue;
	std::queue<std::shared_ptr<GameObject>> _startQueue;
	std::queue<std::pair<std::shared_ptr<GameObject>, GameObjectType>> _changeTypeQueue;

	std::string _name;
	SceneType _type;

	GlobalParam _globalParam;
	std::shared_ptr<Material> _finalShader;
};



