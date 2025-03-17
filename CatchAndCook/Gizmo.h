#pragma once
#include "Vertex.h"


class TextHandle;

class GizmoTexture : public RendererBase
{
public:
	std::shared_ptr<Material> material;

	std::shared_ptr<Mesh> _mesh;
	std::vector<std::shared_ptr<Texture>> textures;
	std::vector<std::shared_ptr<Texture>> textTextures;
	std::vector<shared_ptr<TextHandle>> handles;
	std::vector<BYTE*> sysMemorys;
	std::vector<TransformParam> transforms;

	int textAllocator = 0;

	void Clear();

	~GizmoTexture() override;
	void Rendering(Material* material,Mesh* mesh,int instanceCount) override;
	void DebugRendering() override;
};

class Gizmo : public RendererBase
{
public:
	void Init();
	void Clear();
	void SetDebugOn(bool ON) { _ON = ON; }
	bool GetDebugOn() { return _ON; }

	static void WidthRollBack(){ main->_width = main->_prevWidth; }
	static void Width(float width = 0.05f) { main->_prevWidth = main->_width; main->_width = width; };
	static void Line(const Vector3& worldStart, const Vector3& worldEnd, const Vector4& Color = Vector4(1, 0, 0, 1));
	static void Ray(const Vector3& worldStart, const Vector3& dir, float dis, const Vector4& Color = Vector4(1, 0, 0, 1));
	static void Box(const BoundingOrientedBox& box, const Vector4& Color = Vector4(1, 0, 0, 1));
	static void Box(const BoundingBox& box,const Vector4& Color = Vector4(1,0,0,1));
	static void Frustum(const BoundingFrustum& frustum, const Vector4& Color = Vector4(1,0,0,1));
	static void Sphere(const BoundingSphere& sphere, const Vector4& Color = Vector4(1, 0, 0, 1));
	static void Text(const wstring& text, int fontSize, const Vector3& worldPos, const Vector3& worldDir,const Vector3& Up, const Vector4& Color = Vector4(1,0,0,1));
	static void Image(const std::shared_ptr<Texture>& texture,const Vector3& worldPos,const Vector3& worldDir,const Vector3& Up,const Vector4& Color = Vector4(1,0,0,1));

	void RenderBegin();
public:
	void Rendering(Material* material, Mesh* mesh,int instanceCount) override;
	void DebugRendering() override;


public:
	static std::unique_ptr<Gizmo> main;
private:
	std::vector<Instance_Gizmo> lineDatas;
	std::shared_ptr<Material> material;
	InstanceBufferContainer* container;

	float _width = 0.02f;
	float _prevWidth = 0.05f;

	GizmoTexture textureGizmo;
	bool _ON = false;
};

