#include "pch.h"
#include "Gizmo.h"


std::unique_ptr<Gizmo> Gizmo::main = nullptr;

void Gizmo::Init()
{
	material = std::make_shared<Material>();
	material->SetShader(ResourceManager::main->Get<Shader>(L"Gizmo"));
	material->SetPass(RENDER_PASS::Debug);
	container = Core::main->GetBufferManager()->GetInstanceBufferPool(BufferType::GizmoInstanceParam)->Alloc();
}

void Gizmo::Clear()
{
	container->Clear();
	lineDatas.clear();
}

void Gizmo::Line(const Vector3& worldStart, const Vector3& worldEnd, const Vector4& Color)
{
	Instance_Gizmo giz;
	giz.color = Color;
	giz.position = worldStart;
	giz.position2 = worldEnd;
	giz.strock = main->_width;
	main->container->AddData(giz);
	main->lineDatas.push_back(giz);
}

void Gizmo::Ray(const Vector3& worldStart, const Vector3& dir, float dis, const Vector4& Color)
{
	Vector3 normalDir;
	dir.Normalize(normalDir);
	Instance_Gizmo giz;
	giz.color = Color;
	giz.position = worldStart;
	giz.position2 = worldStart + normalDir * dis;
	giz.strock = main->_width;
	main->container->AddData(giz);
	main->lineDatas.push_back(giz);
}

void Gizmo::Box(const BoundingOrientedBox& box, const Vector4& Color)
{

}

void Gizmo::Sphere(const BoundingSphere& box, const Vector4& Color)
{

}

void Gizmo::RenderBegin()
{
	SceneManager::main->GetCurrentScene()->AddRenderer(material.get(),nullptr,this);
}

void Gizmo::Rendering(Material* material, Mesh* mesh)
{
	auto& cmdList = Core::main->GetCmdList();

	cmdList->SetPipelineState(material->GetShader()->_pipelineState.Get());

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	cmdList->IASetVertexBuffers(1,1,&container->_bufferView);
	cmdList->DrawInstanced(2, lineDatas.size(),0,0);
}

void Gizmo::DebugRendering()
{

}
