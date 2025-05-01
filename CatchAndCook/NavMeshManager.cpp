#include "pch.h"
#include "NavMeshManager.h"
#include <queue>

#include "Gizmo.h"


std::unique_ptr<NavMeshManager> NavMeshManager::main = nullptr;


NavMeshData::NavMeshData()
{
}

NavMeshData::~NavMeshData()
{
}

NavMeshPathData::NavMeshPathData(const NavMeshData& data, float g, float h)
{
	this->data = &const_cast<NavMeshData&>(data);
	this->id = data.index;
	this->g = g;
	this->h = h;
}

bool NavMeshPathData::operator<(const NavMeshPathData& other) const
{
	return GetF() > other.GetF();
}

bool NavMeshPathData::operator==(const NavMeshPathData& other) const
{
	return id == other.id;
}


NavMeshManager::NavMeshManager()
{
}

NavMeshManager::~NavMeshManager()
{
}

void NavMeshManager::Release()
{
}

std::vector<NavMeshData>& NavMeshManager::GetNavMeshData()
{
	return _datas;
}

void NavMeshManager::SetNavMeshData(const std::vector<NavMeshData>& data)
{
	_datas = data;
}


std::vector<Vector3> NavMeshManager::CalculatePath_Funnel(const Vector3& startPos, const Vector3& endPos,
                                                          const std::vector<NavMeshData>& datas,
                                                          const std::vector<int>& _tris)
{
    // 1) 삼각형 배열 만들기
    int triangleCount = _tris.size() / 3;
    std::vector<std::array<int, 3>> tris(triangleCount);
    for (int i = 0; i < triangleCount; ++i)
        tris[i] = { _tris[3 * i + 0], _tris[3 * i + 1], _tris[3 * i + 2] };

    auto makeIndexEdgeFunc = [&](int a, int b)
    {
	    return IndexEdge{ std::min(a,b), std::max(a,b) };
    };
    std::unordered_map<IndexEdge, std::vector<int>, IndexEdgeHash> edgeToTriangles;

    for (int t = 0; t < triangleCount; ++t) { // 트라이 앵글의 각 index를 엣지에 추가
        auto& tr = tris[t];
        edgeToTriangles[makeIndexEdgeFunc(tr[0], tr[1])].push_back(t);
        edgeToTriangles[makeIndexEdgeFunc(tr[1], tr[2])].push_back(t);
        edgeToTriangles[makeIndexEdgeFunc(tr[2], tr[0])].push_back(t);
    }

    // 3) 삼각형 인접 리스트 (A*)
    std::vector<std::vector<int>> triangleAdjects(triangleCount);
    for (auto& triangleIndexToEdges : edgeToTriangles)
    {
        if (triangleIndexToEdges.second.size() == 2) {
            int a = triangleIndexToEdges.second[0], b = triangleIndexToEdges.second[1];
            triangleAdjects[a].push_back(b);
            triangleAdjects[b].push_back(a);
        }
    }

    // 4) 시작/끝 삼각형 찾기
    int startTri = 0, endTri = 0;
    for (int t = 0; t < triangleCount; ++t) {
        auto& tr = tris[t];
        if (PointInTriangle3D(startPos,
            datas[tr[0]].position,
            datas[tr[1]].position,
            datas[tr[2]].position))
            startTri = t;
        if (PointInTriangle3D(endPos,
            datas[tr[0]].position,
            datas[tr[1]].position,
            datas[tr[2]].position))
            endTri = t;
    }

    // 5) 삼각형 A* (centroid 휴리스틱)
    std::vector<TriangleNode> triangleNodes(triangleCount);
    auto GetTriangleIdCenterPosition = [&](int ti) {
        auto& tr = tris[ti];
        return (datas[tr[0]].position
            + datas[tr[1]].position
            + datas[tr[2]].position) / 3.0f;
        };
    Vector3 centEnd = GetTriangleIdCenterPosition(endTri);

	for (int i = 0; i < triangleCount; ++i)
    {
        triangleNodes[i].g = FLT_MAX;
        triangleNodes[i].f = FLT_MAX;
        triangleNodes[i].open = false;
        triangleNodes[i].closed = false;
    }

    triangleNodes[startTri].g = 0;
    triangleNodes[startTri].f = Vector3::Distance(GetTriangleIdCenterPosition(startTri), centEnd);
    triangleNodes[startTri].parentIndex = startTri;
    triangleNodes[startTri].open = true;

    using PairFloatIndex = std::pair<float, int>;
    std::priority_queue<PairFloatIndex, std::vector<PairFloatIndex>, std::greater<PairFloatIndex>> priorityQueue;
    priorityQueue.push({ triangleNodes[startTri].f, startTri });

    bool isFound = false;
    while (!priorityQueue.empty())
    {
        auto [f, currentIndex] = priorityQueue.top();
    	priorityQueue.pop();

        if (triangleNodes[currentIndex].closed)
            continue;
        triangleNodes[currentIndex].closed = true;

        if (currentIndex == endTri)
        {
	        isFound = true;
        	break;
        }
        for (int nextAdjNodeIndex : triangleAdjects[currentIndex])
        {
            if (triangleNodes[nextAdjNodeIndex].closed) // 닫혀있으면 캔슬
                continue;

            float ng = triangleNodes[currentIndex].g + Vector3::Distance(GetTriangleIdCenterPosition(currentIndex), GetTriangleIdCenterPosition(nextAdjNodeIndex)); // 누적 가중치
            float nh = Vector3::Distance(GetTriangleIdCenterPosition(nextAdjNodeIndex), centEnd);  // 도착점까지 휴리스틱
            float nf = ng + nh;

            if (!triangleNodes[nextAdjNodeIndex].open || nf < triangleNodes[nextAdjNodeIndex].f) // 아직 openNode가 아니거나 nf가 더 작은거면 값 갱신
            {
                triangleNodes[nextAdjNodeIndex].g = ng;
            	triangleNodes[nextAdjNodeIndex].f = nf;
            	triangleNodes[nextAdjNodeIndex].parentIndex = currentIndex;

                if (!triangleNodes[nextAdjNodeIndex].open) // 열고 추가
                {
                    triangleNodes[nextAdjNodeIndex].open = true;
                    priorityQueue.push({ nf, nextAdjNodeIndex });
                }
            }
        }
    }
    if (!isFound)
        return { };

    // 6) triPath 복원
    std::vector<int> triPath;
    for (int t = endTri;; t = triangleNodes[t].parentIndex) // 부모를 거슬러 올라가며 갱신
    {
        triPath.push_back(t);
        if (t == startTri)
            break;
    }
    ranges::reverse(triPath);

    // 7) 포털 생성 (좌→우)
    std::vector<std::pair<Vector3, Vector3>> portals;
    portals.reserve(triPath.size() + 1);
    portals.emplace_back(startPos, startPos);
    for (int i = 0; i + 1 < (int)triPath.size(); ++i) {
        int t0 = triPath[i], t1 = triPath[i + 1];
        for (auto& kv : edgeToTriangles) {
            auto& v = kv.second;
            if (v.size() == 2 && ((v[0] == t0 && v[1] == t1) || (v[0] == t1 && v[1] == t0))) {
                auto a3 = datas[kv.first.u].position;
                auto b3 = datas[kv.first.v].position;

                // 이동 방향 기준 왼쪽/오른쪽 판정
                Vector2 c0 = Vector2(GetTriangleIdCenterPosition(t0).x, GetTriangleIdCenterPosition(t0).z);
                Vector2 c1 = Vector2(GetTriangleIdCenterPosition(t1).x, GetTriangleIdCenterPosition(t1).z);
                Vector2 dir = Normalize2D({ c1.x - c0.x,c1.y - c0.y });
                Vector2 mid = { (a3.x + b3.x) * 0.5f,(a3.z + b3.z) * 0.5f };

                if (Area2_2D(mid, { mid.x + dir.x,mid.y + dir.y }, Vector2(a3.x, a3.z)) >= 0)
                    portals.emplace_back(a3, b3);
                else
                    portals.emplace_back(b3, a3);
                break;
            }
        }
    }
    portals.emplace_back(endPos, endPos);

    // 8) Funnel 스무딩
    auto smooth = StringPull(startPos, endPos, portals);


    if (_gizmoDebug)
    {
        for (int t : triPath) {
            Vector3 c = GetTriangleIdCenterPosition(t);
            Gizmo::Sphere({ c , 0.2 });
        }
        for (auto& pp : portals) {
            Gizmo::Line(pp.first + Vector3::Up * 1, pp.second + Vector3::Up * 1, Vector4(1, 1, 0, 1));
        }

        // 9) 그리기
        Gizmo::Width(0.2f);
        for (int i = 1; i < (int)smooth.size(); ++i) {
            Gizmo::Line(
                smooth[i - 1] + Vector3::Up * 10,
                smooth[i] + Vector3::Up * 10,
                Vector4(1, 0, 0, 1));
        }
        Gizmo::WidthRollBack();
    }
    return smooth;
}

void NavMeshManager::Init()
{
    ImguiManager::main->navMesh = &_gizmoDebug;
}

