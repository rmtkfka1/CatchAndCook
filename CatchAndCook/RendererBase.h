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

	void AddSetter(const std::shared_ptr<RenderObjectSetter>& setter);
	void RemoveSetter(const shared_ptr<RenderObjectSetter>& object);

	std::vector<std::shared_ptr<RenderObjectSetter>> _setters;
};

