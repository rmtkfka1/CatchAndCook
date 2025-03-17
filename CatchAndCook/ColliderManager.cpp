#include "pch.h"
#include "ColliderManager.h"

#include <algorithm>

#include "Collider.h"


unique_ptr<ColliderManager> ColliderManager::main = nullptr;

vec3 ColliderManager::GetGridCell(const vec3& position) const
{
	return vec3(
		std::floor(position.x / _cellSize),
		std::floor(position.y / _cellSize),
		std::floor(position.z / _cellSize)
	);
}

vector<vec3> ColliderManager::GetOccupiedCells(const shared_ptr<Collider>& collider) const
{
	auto& [min,max] = collider->GetMinMax();

	vec3 minCell = GetGridCell(min);
	vec3 maxCell = GetGridCell(max);

	vector<vec3> occupiedCells;

	for (float x = minCell.x; x <= maxCell.x; ++x)
	{
		for (float y = minCell.y; y <= maxCell.y; ++y)
		{
			for (float z = minCell.z; z <= maxCell.z; ++z)
			{
				occupiedCells.push_back(vec3(x, y, z));
			}
		}
	}

	return occupiedCells;

}



void ColliderManager::AddCollider(const std::shared_ptr<Collider>& collider)
{
	if (collider->GetOwner()->GetType() == GameObjectType::Static) {
		auto& occupiedCells = GetOccupiedCells(collider);
		for (const auto& cell : occupiedCells)
		{
			_staticColliderGrids[cell].push_back(collider);
		}
	}

	if (collider->GetOwner()->GetType() == GameObjectType::Dynamic)
	{
		auto& occupiedCells = GetOccupiedCells(collider);
		for (const auto& cell : occupiedCells)
		{
			_dynamicColliderGrids[cell].push_back(collider);
		}
	}

	_colliderLinkTable[collider] = {};
}

void ColliderManager::RemoveCollider(const std::shared_ptr<Collider>& collider)
{
	if (collider->GetOwner()->GetType() == GameObjectType::Static)
	{
		auto& occupiedCells = GetOccupiedCells(collider);
		for (const auto& cell : occupiedCells)
		{
			auto& colliders = _staticColliderGrids[cell];
			auto it = std::ranges::find(colliders, collider);
			if (it != colliders.end())
				colliders.erase(it);
		}
	}

	if (collider->GetOwner()->GetType() == GameObjectType::Dynamic)
	{
		auto& occupiedCells = GetOccupiedCells(collider);
		for (const auto& cell : occupiedCells)
		{
			auto& colliders = _dynamicColliderGrids[cell];
			auto it = std::ranges::find(colliders, collider);
			if (it != colliders.end())
				colliders.erase(it);
		}
	}

	for(auto& other : _colliderLinkTable[collider])
	{
		CallBackEnd(collider,other);
		CallBackEnd(other,collider);
		_colliderLinkTable[other].erase(collider);
	}

	_colliderLinkTable.erase(collider);
	
}

std::unordered_set<std::shared_ptr<Collider>>& ColliderManager::GetPotentialCollisions(const std::shared_ptr<Collider>& collider) const
{
	std::unordered_set<std::shared_ptr<Collider>> potentialCollisions;

	//Static 객체면  은 Dynamic 객체와 충돌 체크
	if (collider->GetOwner()->GetType() == GameObjectType::Static)
	{
		auto& occupiedCells = GetOccupiedCells(collider);

		for (const auto& cell : occupiedCells)
		{
			auto& colliders = _staticColliderGrids.at(cell);
			potentialCollisions.insert(colliders.begin(), colliders.end());
		}

	}

	//Dynamic 객체면  은 Static 객체 와 dynamic 충돌 체크
	if (collider->GetOwner()->GetType() == GameObjectType::Dynamic)
	{
		auto& occupiedCells = GetOccupiedCells(collider);
		std::unordered_set<std::shared_ptr<Collider>> potentialCollisions;

		for (const auto& cell : occupiedCells)
		{
			auto& colliders = _dynamicColliderGrids.at(cell);
			potentialCollisions.insert(colliders.begin(), colliders.end());
		}
	}

	return potentialCollisions;
}

void ColliderManager::Update()
{


	for(int i = 0;i<_dynamicColliders.size();++i)
	{
		auto& src = _dynamicColliders[i];

		for(int j = 0;j<_staticColliders.size();++j)
		{
			auto& dest = _staticColliders[j];

			if(TotalCheckCollision(src,dest))
			{
				if(_colliderLinkTable[src].contains(dest) == false)
				{
					_colliderLinkTable[src].insert(dest);
					_colliderLinkTable[dest].insert(src);
					CallBackBegin(src,dest);
					CallBackBegin(dest,src);
				}
			}

			else
			{
				if(_colliderLinkTable[src].contains(dest))
				{
					_colliderLinkTable[src].erase(dest);
					_colliderLinkTable[dest].erase(src);
					CallBackEnd(src,dest);
					CallBackEnd(dest,src);
				}
			}
		}

		for(int j = i + 1; j< _dynamicColliders.size(); ++j)
		{
			auto& dest = _dynamicColliders[j];

			if(TotalCheckCollision(src,dest))
			{
				if(_colliderLinkTable[src].contains(dest) == false)
				{
					_colliderLinkTable[src].insert(dest);
					_colliderLinkTable[dest].insert(src);
					CallBackBegin(src,dest);
					CallBackBegin(dest,src);
				}
			}

			else
			{
				if(_colliderLinkTable[src].contains(dest))
				{
					_colliderLinkTable[src].erase(dest);
					_colliderLinkTable[dest].erase(src);
					CallBackEnd(src,dest);
					CallBackEnd(dest,src);
				}
			}
		}
	}
} 

//무언가와 충돌하고 있는지 체크
bool ColliderManager::IsCollision(const std::shared_ptr<Collider>& src)
{
	return _colliderLinkTable.find(src)->second.empty() == false;
}

//특정누군가와 충돌하고 있는지 체크
bool ColliderManager::IsCollision(const std::shared_ptr<Collider>& src,const std::shared_ptr<Collider>& dest)
{
	auto it = _colliderLinkTable.find(src);
	if(it != _colliderLinkTable.end())
		return it->second.contains(dest);

	return IsCollision(dest);
}

std::unordered_set<std::shared_ptr<Collider>>& ColliderManager::GetCollisionList(const std::shared_ptr<Collider>& src)
{
	return _colliderLinkTable.find(src)->second;
}

bool ColliderManager::TotalCheckCollision(const std::shared_ptr<Collider>& src, const std::shared_ptr<Collider>& dest)
{
	return (src->groupId != dest->groupId) &&
		src->GetOwner()->GetActive() &&
		dest->GetOwner()->GetActive() && src->CheckCollision(dest);
}

void ColliderManager::CallBackBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	auto& components = collider->GetOwner()->GetComponentAll();
	for(auto& component : components)
		component->CollisionBegin(collider,other);
}

void ColliderManager::CallBackEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	auto& components = collider->GetOwner()->GetComponentAll();

	for(auto& component : components)
		component->CollisionEnd(collider,other);
}

RayHit ColliderManager::RayCast(const Ray& ray, const float& dis) const
{
	RayHit closestHit;
	closestHit.distance = dis;  // 최대 충돌 거리(dis)로 초기화
	bool hitFound = false;

	// Static 콜라이더 목록 처리
	for(const auto& collider : _staticColliders)
	{
		RayHit currentHit;
		currentHit.distance = dis;  // 최대 거리로 초기화
		if(collider->RayCast(ray,dis,currentHit))
		{
			if(!hitFound || currentHit.distance < closestHit.distance)
			{
				closestHit = currentHit;
				hitFound = true;
			}
		}
	}

	for(const auto& collider : _dynamicColliders)
	{
		RayHit currentHit;
		currentHit.distance = dis;

		if(collider->RayCast(ray,dis,currentHit))
		{
			if(!hitFound || currentHit.distance < closestHit.distance)
			{
				closestHit = currentHit;
				hitFound = true;
			}
		}
	}

	return closestHit;
}


