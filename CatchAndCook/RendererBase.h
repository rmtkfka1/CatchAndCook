#pragma once


class RendererBase;
class Material;


struct RenderObjectStrucutre
{
	std::shared_ptr<Material> material;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<RendererBase> renderer;
};

class RenderObjectSetter
{
public:
	virtual void SetData() = 0;
};

class RendererBase
{
public:
	virtual void Rendering(const std::shared_ptr<Material>& material, const std::shared_ptr<Mesh>& mesh) = 0;
	virtual void DebugRendering()=0;

	void AddSetter(std::shared_ptr<RenderObjectSetter> setter) { setters.push_back(setter); };
	void ClearSetter() { setters.clear(); };
	std::vector<std::shared_ptr<RenderObjectSetter>> setters;
};

