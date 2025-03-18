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

void NavMeshManager::CalculatePath(const Vector3& startPosition, const Vector3& endPosition, const std::vector<NavMeshData>& datas)
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
	std::vector<NavMeshPathData> resultPath;

	const Vector3& targetPos = endNode.data->position;

	while (!openQueue.empty())
	{
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

		auto a = pathDataList[currentNode.id];
		//pathDataList[currentNode.id] = currentNode;
		if (pathDataList[currentNode.id].IsClose())
			continue;
		pathDataList[currentNode.id].SetClose();
		
		
		for (const auto& adjId : currentNode.data->adjectIndexs)
		{
			auto& adjNode = pathDataList[adjId];

			float nextG = pathDataList[currentNode.id].g + Vector3::Distance(currentNode.data->position, adjNode.data->position);
			float nextH = Vector3::Distance(adjNode.data->position, endNode.data->position);
			float nextF = nextG + nextH;

			if (adjNode.IsOpen())
			{
				if (adjNode.GetF() >= nextF)
				{
					adjNode.g = nextG;
					adjNode.h = nextH;
					adjNode.parentId = currentNode.id;
					//openQueue.push(pathDataList[adjId]);
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
		Gizmo::Width(0.2f);
		// 결과 경로 그리기
		for (std::size_t i = 1; i < resultPath.size(); ++i)
		{
			const auto& currNode = resultPath[i];
			const auto& prevNode = resultPath[i - 1];
			Gizmo::Line(currNode.data->position, prevNode.data->position, Vector4(1, 0, 0, 1));
		}
		Gizmo::WidthRollBack();
	}
	
	//float a = queue.top().value;
	//std::cout << queue.top().value << "\n";
}

void NavMeshManager::Init()
{

}

