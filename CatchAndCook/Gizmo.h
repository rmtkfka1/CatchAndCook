#pragma once
#include "Vertex.h"

class Gizmo : public RendererBase
{
public:
	void Init();
	void Clear();

	static void Width(float width = 0.05f){_width = width;};
	static void Line(Vector3 worldStart,Vector3 worldEnd, Vector4 Color = Vector4(1,0,0,1));
	static void Ray(Vector3 worldStart, Vector3 dir,float dis, Vector4 Color = Vector4(1,0,0,1));
	static void Box(BoundingOrientedBox box, Vector4 Color = Vector4(1,0,0,1));
	static void Sphere(BoundingSphere box,Vector4 Color = Vector4(1,0,0,1));
	void RenderBegin();
public:
	void Rendering(Material* material, Mesh* mesh) override;
	void DebugRendering() override;


public:
	static std::unique_ptr<Gizmo> main;
private:
	std::vector<Instance_Gizmo> lineDatas;
	std::shared_ptr<Material> material;
	InstanceBufferContainer* container;

	float _width = 0.05f;
};

