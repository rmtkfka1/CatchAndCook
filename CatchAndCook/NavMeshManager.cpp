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

void NavMeshManager::CalculatePath(const Vector3& startPosition, const Vector3& endPosition, const std::vector<NavMeshData>& datas, const std::vector<std::array<Vector3, 2>>& edges)
{
	std::vector<NavMeshPathData> pathDataList;
	pathDataList.reserve(datas.size());
	for (const auto& data : datas)
		pathDataList.emplace_back(data, 0, 0);

	// Step 1. Find Near
	auto distanceSquaredXZ = [](const Vector3& pos, const NavMeshPathData& data) {
		return Vector3::DistanceSquared(pos, Vector3(data.data->position.x, 0, data.data->position.z));
		};

	NavMeshPathData startNode;
	NavMeshPathData endNode;
	float minNearDis = std::numeric_limits<float>::max();
	float minFarDis = std::numeric_limits<float>::max();

	for (const auto& data : pathDataList) {
		const float currentNearDis = distanceSquaredXZ(startPosition, data);
		const float currentFarDis = distanceSquaredXZ(endPosition, data);

		if (currentNearDis < minNearDis) {
			minNearDis = currentNearDis;
			startNode = data;
		}
		if (currentFarDis < minFarDis) {
			minFarDis = currentFarDis;
			endNode = data;
		}
	}

	Gizmo::Width(0.2f);
	Gizmo::Box(BoundingBox(startNode.data->position, Vector3(1, 1, 1)));
	Gizmo::Box(BoundingBox(endNode.data->position, Vector3(1, 1, 1)));
	Gizmo::WidthRollBack();

	Gizmo::Width(0.08f);
	for (auto& data : datas)
		for (auto& adj : data.adjectIndexs)
			Gizmo::Line(data.position, datas[adj].position, Vector4(0,1,0,1));
	Gizmo::WidthRollBack();




	std::priority_queue<NavMeshPathData> openQueue;
	openQueue.push(startNode);
	bool foundPath = false;
	std::deque<NavMeshPathData> resultPath;

	const Vector3& targetPos = endNode.data->position;
	while (!openQueue.empty())
	{
		if (pathDataList[openQueue.top().id].IsClose())
		{
			openQueue.pop();
			continue;
		}
		const auto currentNode = openQueue.top();
		openQueue.pop();

		if (currentNode == endNode)
		{
			foundPath = true;
			for (NavMeshPathData node = currentNode; node.id != startNode.id; node = pathDataList[node.parentId])
				resultPath.push_back(node);
			resultPath.push_back(startNode);
			break;
		}
		pathDataList[currentNode.id].SetClose();
		
		
		for (const auto& adjId : currentNode.data->adjectIndexs)
		{
			auto& adjNode = pathDataList[adjId];

			float nextG = pathDataList[currentNode.id].g + Vector3::Distance(currentNode.data->position, adjNode.data->position);
			float nextH = Vector3::Distance(adjNode.data->position, endNode.data->position);
			float nextF = nextG + nextH;

			if (adjNode.IsOpen())
			{
				if ((nextF + 0.1f) < adjNode.GetF())
				{
					adjNode.g = nextG;
					adjNode.h = nextH;
					adjNode.parentId = currentNode.id;
					openQueue.push(pathDataList[adjId]);
				}
			}
			else
			{
				adjNode.g = nextG;
				adjNode.h = nextH;
				adjNode.parentId = currentNode.id;
				adjNode.SetOpen();
				openQueue.push(pathDataList[adjId]);
			}
		}
	}
	if (foundPath)
	{
		std::ranges::reverse(resultPath);
		NavMeshData startPoint, endPoint;
		startPoint.position = Vector3(startPosition.x, resultPath[0].data->position.y, startPosition.z);
		endPoint.position = Vector3(endPosition.x, resultPath[resultPath.size() - 1].data->position.y, endPosition.z);
		resultPath.push_front({ startPoint });
		resultPath.push_back({ endPoint });

		/*

		std::deque<NavMeshPathData> resultPath2;

		resultPath2.push_back(resultPath.front());
		size_t currentIndex = 0;
		const size_t n = resultPath.size();

		while (currentIndex < n - 1)
		{
			size_t nextIndex = currentIndex;
			// 현재 노드 다음부터 가까운 노드부터 검사
			for (size_t i = currentIndex + 1; i < n; ++i)
			{
				// 현재 노드와 resultPath[i] 사이에 line-of-sight가 있다면 갱신
				if (HasLineOfSight(resultPath[currentIndex].data->position, resultPath[i].data->position, edges))
					nextIndex = i;
				else
					// 가까운 순서대로 검사하므로, 처음으로 line-of-sight가 깨지는 지점에서 중단
					break;
			}
			// 진전이 없는 경우 강제적으로 다음 노드를 선택
			if (nextIndex == currentIndex)
				nextIndex = currentIndex + 1;

			resultPath2.push_back(resultPath[nextIndex]);
			currentIndex = nextIndex;
		}
		*/
		std::deque<NavMeshPathData> rawPath = resultPath;

		// 1차 Forward 스무딩
		auto pass1 = SmoothPath(rawPath, edges);

		// 2차 Backward 스무딩
		// 역순으로 만들고
		std::deque<NavMeshPathData> rev1(pass1.rbegin(), pass1.rend());
		auto pass2 = SmoothPath(rev1, edges);

		// 다시 정방향으로 복원
		std::deque<NavMeshPathData> resultPath2(pass2.rbegin(), pass2.rend());

		Gizmo::Width(0.2f);
		// 결과 경로 그리기
		for (std::size_t i = 1; i < resultPath2.size(); ++i)
		{
			const auto& currNode = resultPath2[i];
			const auto& prevNode = resultPath2[i - 1];
			Gizmo::Line(currNode.data->position + Vector3::Up * 3, prevNode.data->position + Vector3::Up * 3, Vector4(1, 0, 0, 1));
		}
		Gizmo::WidthRollBack();
	}
	
	//float a = queue.top().value;
	//std::cout << queue.top().value << "\n";
}

std::deque<NavMeshPathData> NavMeshManager::SmoothPath(const std::deque<NavMeshPathData>& rawPath,
	const std::vector<std::array<Vector3, 2>>& edges)
{
	std::deque<NavMeshPathData> out;
	size_t n = rawPath.size();
	if (n == 0) return out;

	out.push_back(rawPath.front());  // 시작점
	size_t current = 0;

	while (current < n - 1)
	{
		size_t nextIdx = current;

		// 1) 뒤에서부터 검사: 가장 먼 지점부터
		for (size_t i = n - 1; i > current; --i)
		{
			if (HasLineOfSight(
				rawPath[current].data->position,
				rawPath[i].data->position,
				edges))
			{
				nextIdx = i;
				break;  // farthest reachable point
			}
		}

		// 2) 혹시 시야가 전혀 뚫리지 않아서 nextIdx==current이면
		if (nextIdx == current)
			nextIdx = current + 1;

		out.push_back(rawPath[nextIdx]);
		current = nextIdx;
	}

	return out;
}

void NavMeshManager::Init()
{

}

