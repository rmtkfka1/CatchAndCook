#pragma once

#include "StructuredBuffer.h"

class RendererBase;
class Material;
class Transform;

struct RenderObjectStrucutre
{
	Material* material;
	Mesh* mesh;
	RendererBase* renderer;
	GameObject* object;
};

class RenderObjectSetter
{
public:
	virtual void PushData() =0;
	virtual void SetData(Material* material =nullptr) = 0;
};

class RendererBase
{
public:
	virtual ~RendererBase() = default;
	virtual void Rendering(Material* material, Mesh* mesh) = 0;
	virtual void DebugRendering()=0;

	void AddSetter(const std::shared_ptr<RenderObjectSetter>& setter);
	void RemoveSetter(const shared_ptr<RenderObjectSetter>& object);

	std::vector<std::shared_ptr<RenderObjectSetter>> _setters;

	BoundingBox& GetBound() {
		return _bound;
	};
	void SetBound(const BoundingBox& bound) {
		_bound = bound;
	};
	void SetCulling(bool isCulling) {
		_isCulling = isCulling;
	};
	bool IsCulling() const
	{
		return _isCulling;
	};

	BoundingBox _bound;
	bool _isCulling = true;
};

