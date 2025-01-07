#pragma once


class Material;

class RendererBase
{
public:
	virtual void Rendering(const std::shared_ptr<Material>& material) = 0;
};

