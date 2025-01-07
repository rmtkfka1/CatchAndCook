#pragma once


class Material;

struct RendererParam
{
	
};

class RendererBase
{
public:
	virtual void Rendering(RendererParam& param, const std::shared_ptr<Material>& material) = 0;
};

