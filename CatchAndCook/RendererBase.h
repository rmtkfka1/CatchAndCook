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
};

class RenderStructuredSetter
{
public:
	virtual void SetData(StructuredBuffer* buffer = nullptr) = 0;
};

class RenderCBufferSetter
{
public:
	virtual void SetData(Material* material =nullptr) = 0;
};

class RendererBase
{
public:
	virtual ~RendererBase() = default;
	virtual void Rendering(Material* material, Mesh* mesh, int instanceCount = 1) = 0;
	virtual void DebugRendering()=0;

	void AddStructuredSetter(const std::shared_ptr<RenderStructuredSetter>& setter, BufferType type);
	void RemoveStructuredSetter(const shared_ptr<RenderStructuredSetter>& object);

	void AddCbufferSetter(const std::shared_ptr<RenderCBufferSetter>& setter);
	void RemoveCbufferSetter(const shared_ptr<RenderCBufferSetter>& object);

	std::shared_ptr<RenderStructuredSetter>& FindStructuredSetter(const BufferType& type);

	std::vector<std::shared_ptr<RenderCBufferSetter>> _cbufferSetters;

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
	void SetInstancing(bool instancing) {
		_isInstancing = instancing;
	};
	bool IsInstancing() const
	{
		return _isInstancing;
	};

	BoundingBox _bound;
	bool _isCulling = true;
	bool _isInstancing = true;

private:
	std::unordered_map<BufferType,std::shared_ptr<RenderStructuredSetter>> _structuredSetters;
};


