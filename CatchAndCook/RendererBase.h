#pragma once


class RendererBase;
class Material;

struct RenderObjectStrucutre
{
	std::shared_ptr<Material> material;
	RendererBase* renderer;
};

class RendererBase
{
public:
	virtual void Rendering(const std::shared_ptr<Material>& material) = 0;
};

