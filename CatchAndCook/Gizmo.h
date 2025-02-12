#pragma once
#include "Vertex.h"

class Gizmo : public RendererBase
{
public:
	void Init();
	void Clear();

	static void WidthRollBack(){ main->_width = main->_prevWidth; }
	static void Width(float width = 0.05f) { main->_prevWidth = main->_width; main->_width = width; };
	static void Line(const Vector3& worldStart, const Vector3& worldEnd, const Vector4& Color = Vector4(1, 0, 0, 1));
	static void Ray(const Vector3& worldStart, const Vector3& dir, float dis, const Vector4& Color = Vector4(1, 0, 0, 1));
	static void Box(const BoundingOrientedBox& box, const Vector4& Color = Vector4(1, 0, 0, 1));
	static void Frustum(const BoundingFrustum& frustum, const Vector4& Color = Vector4(1,0,0,1));
	static void Sphere(const BoundingSphere& sphere, const Vector4& Color = Vector4(1, 0, 0, 1));
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

	float _width = 0.02f;
	float _prevWidth = 0.05f;
};

