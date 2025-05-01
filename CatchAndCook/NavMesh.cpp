#include "pch.h"
#include "NavMesh.h"

#include "Gizmo.h"

NavMesh::~NavMesh()
{
}

bool NavMesh::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void NavMesh::Init()
{
	Component::Init();
}

void NavMesh::Start()
{
	Component::Start();
	NavMeshManager::main->SetNavMeshData(_datas);

	startPos = Vector3(250, 0, 220);
}

void NavMesh::Update()
{
	Component::Update();
	startPos += Vector3((Input::main->GetKey(KeyCode::L)?1:0) - (Input::main->GetKey(KeyCode::J) ? 1 : 0),
		0,
		(Input::main->GetKey(KeyCode::I) ? 1 : 0) - (Input::main->GetKey(KeyCode::K) ? 1 : 0)
		)
	* Time::main->GetDeltaTime() * 40;
	//NavMeshManager::main->CalculatePath(Vector3(350, 0, 150), startPos,NavMeshManager::main->GetNavMeshData(), _edges);
	NavMeshManager::main->CalculatePath_Funnel(Vector3(350, 0, 150), startPos,
		NavMeshManager::main->GetNavMeshData(), _tris);
	Gizmo::Width(0.4f);
	Gizmo::Box(BoundingBox(startPos, Vector3(1, 100, 1)));

	for (auto edge : _edges)
		Gizmo::Line(edge[0], edge[1], Vector4(0, 0, 1, 1));
	Gizmo::WidthRollBack();
}

void NavMesh::Update2()
{
	Component::Update2();
}

void NavMesh::Enable()
{
	Component::Enable();
}

void NavMesh::Disable()
{
	Component::Disable();
}

void NavMesh::RenderBegin()
{
	Component::RenderBegin();
}

void NavMesh::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void NavMesh::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void NavMesh::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void NavMesh::SetDestroy()
{
	Component::SetDestroy();
}

void NavMesh::Destroy()
{
	Component::Destroy();
}

std::vector<NavMeshData>& NavMesh::GetNavMeshData()
{
	return _datas;
}

void NavMesh::SetNavMeshData(const std::vector<NavMeshData>& data)
{
	_datas = data;
}

std::vector< std::array<Vector3, 2>>& NavMesh::GetNavMeshEdgeData()
{
	return _edges;
}

void NavMesh::SetNavMeshEdgeData(const std::vector< std::array<Vector3, 2>>& data)
{
	_edges = data;
}

void NavMesh::SetTris(const std::vector<int>& data)
{
	_tris = data;
}

