#pragma once


class RendererBase;
class Material;


struct RenderObjectStrucutre
{
	Material* material;
	Mesh* mesh;
	RendererBase* renderer;
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

	void AddSetter(std::shared_ptr<RenderObjectSetter> setter) { setters.push_back(setter); };

	std::vector<std::shared_ptr<RenderObjectSetter>> setters;

	void RemoveSetters(shared_ptr<RenderObjectSetter> object);

};

