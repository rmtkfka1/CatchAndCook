#pragma once


class RendererBase;
class Material;


struct RenderObjectStrucutre
{
	std::shared_ptr<Material> material;
	std::shared_ptr<RendererBase> renderer;
};

class RendererBase
{
public:
	virtual void Rendering(const std::shared_ptr<Material>& material) = 0;

	virtual void DebugRendering()=0;
};

