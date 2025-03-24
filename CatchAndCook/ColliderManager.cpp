#include "pch.h"
#include "ColliderManager.h"
#include <algorithm>
#include "Gizmo.h"
#include "Collider.h"
#include "Terrain.h"
#include "TerrainManager.h"


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
	auto& [min, max] = collider->GetMinMax();
	vec3 minCell = GetGridCell(min);
	vec3 maxCell = GetGridCell(max);

	vector<vec3> occupiedCells;

	for (int x = static_cast<int>(minCell.x); x <= static_cast<int>(maxCell.x); ++x)
	{
		for (int y = static_cast<int>(minCell.y); y <= static_cast<int>(maxCell.y); ++y)
		{
			for (int z = static_cast<int>(minCell.z); z <= static_cast<int>(maxCell.z); ++z)
			{
				occupiedCells.push_back(vec3(x, y, z));
			}
		}
	}

	return occupiedCells;
}



void ColliderManager::AddCollider(const std::shared_ptr<Collider>& collider)
{
	auto occupiedCells = GetOccupiedCells(collider);

	for (const auto& cell : occupiedCells)
	{
		if (collider->GetOwner()->GetType() == GameObjectType::Static)
		{
			_staticColliderGrids[cell].push_back(collider);
		}
		else if (collider->GetOwner()->GetType() == GameObjectType::Dynamic)
		{
			_dynamicColliderGrids[cell].push_back(collider);
			_dynamicColliderCashing[collider].push_back(cell);
		}
	}

	if (_colliderLinkTable.find(collider) == _colliderLinkTable.end()) {
		_colliderLinkTable[collider] = {};
	}
}

void ColliderManager::AddColliderForRay(const std::shared_ptr<Collider>& collider)
{
	auto it = std::ranges::find(_collidersForRay, collider);
	if (it == _collidersForRay.end())
		_collidersForRay.push_back(collider);
}

void ColliderManager::RemoveAColliderForRay(const std::shared_ptr<Collider>& collider)
{
	auto it = std::ranges::find(_collidersForRay, collider);
	if (it != _collidersForRay.end())
		_collidersForRay.erase(it);
}

void ColliderManager::RemoveCollider(const std::shared_ptr<Collider>& collider)
{
	auto occupiedCells = GetOccupiedCells(collider);
	for (const auto& cell : occupiedCells)
	{
		if (collider->GetOwner()->GetType() == GameObjectType::Static)
		{
			auto it = _staticColliderGrids.find(cell);

			if (it != _staticColliderGrids.end())
			{
				auto& colliders = it->second;
				auto colliderIt = std::ranges::find(colliders, collider);
				if (colliderIt != colliders.end())
					colliders.erase(colliderIt);
			}
		}

		else if (collider->GetOwner()->GetType() == GameObjectType::Dynamic)
		{
			auto it = _dynamicColliderGrids.find(cell);
			if (it != _dynamicColliderGrids.end())
			{
				auto& colliders = it->second;
				auto colliderIt = std::ranges::find(colliders, collider);
				if (colliderIt != colliders.end())
					colliders.erase(colliderIt);
			}
		}
	}

	auto colliderIt = _colliderLinkTable.find(collider);

	if (colliderIt != _colliderLinkTable.end())
	{
		for (auto& other : colliderIt->second)
		{
			CallBackEnd(collider, other);
			CallBackEnd(other, collider);
			_colliderLinkTable[other].erase(collider);
		}
		_colliderLinkTable.erase(collider);
	}
}

std::unordered_set<std::shared_ptr<Collider>> ColliderManager::GetPotentialCollisions(std::shared_ptr<Collider>& collider) 
{
	std::unordered_set<std::shared_ptr<Collider>> potentialCollisions;

	vector<vec3> occupiedCells;

	if (_dynamicColliderCashing.find(collider) != _dynamicColliderCashing.end())
	{
		occupiedCells = std::move(_dynamicColliderCashing[collider]);
	}
	else
	{
		assert(false);
		occupiedCells = GetOccupiedCells(collider);
	}

	for (const auto& cell : occupiedCells)
	{
		{
			auto it = _dynamicColliderGrids.find(cell);

			if (it != _dynamicColliderGrids.end())
			{
				auto& colliders = it->second;
				potentialCollisions.insert(colliders.begin(), colliders.end());
			}
		}

		{
			auto it = _staticColliderGrids.find(cell);

			if (it != _staticColliderGrids.end())
			{
				auto& colliders = it->second;
				potentialCollisions.insert(colliders.begin(), colliders.end());
			}
		}

	};

	return potentialCollisions;
}

void ColliderManager::Update()
{
	for (auto& [cell, colliders] : _staticColliderGrids)
	{
		for (auto& collider : colliders)
		{
			VisualizeOccupiedCells(cell,collider);
		}
	}

	for (auto& [cell, colliders] : _dynamicColliderGrids)
	{
		if (colliders.size() <= 1  && _staticColliderGrids[cell].size() ==0 ) continue;

		for (auto& collider : colliders)
		{
			VisualizeOccupiedCells(cell,collider);

			auto potentialCollisions = GetPotentialCollisions(collider);

			for (auto& other : potentialCollisions)
			{
				if (other == collider) continue;

				if (TotalCheckCollision(collider, other))
				{
					if (_colliderLinkTable[collider].contains(other) == false)
					{
						_colliderLinkTable[collider].insert(other);
						_colliderLinkTable[other].insert(collider);
						CallBackBegin(collider, other);
						CallBackBegin(other, collider);
					}
				}
				else
				{
					if (_colliderLinkTable[collider].contains(other))
					{
						_colliderLinkTable[collider].erase(other);
						_colliderLinkTable[other].erase(collider);
						CallBackEnd(collider, other);
						CallBackEnd(other, collider);
					}
				}
			}
		}
	}

	_dynamicColliderGrids.clear();
	_dynamicColliderCashing.clear();
}

//무언가와 충돌하고 있는지 체크
bool ColliderManager::IsCollision(const std::shared_ptr<Collider>& src)
{
	auto it = _colliderLinkTable.find(src);
	return (it != _colliderLinkTable.end() && !it->second.empty());
}
//특정누군가와 충돌하고 있는지 체크

bool ColliderManager::IsCollision(const std::shared_ptr<Collider>& src, const std::shared_ptr<Collider>& dest)
{
	auto it = _colliderLinkTable.find(src);
	if (it != _colliderLinkTable.end())
		return it->second.contains(dest);

	return IsCollision(dest);
}

std::unordered_set<std::shared_ptr<Collider>>& ColliderManager::GetCollisionList(const std::shared_ptr<Collider>& src)
{
	return _colliderLinkTable.find(src)->second;
}

bool ColliderManager::TotalCheckCollision(const std::shared_ptr<Collider>& src, const std::shared_ptr<Collider>& dest)
{
	if (src->groupId == dest->groupId) return false;
	if (!src->GetOwner()->GetActive() || !dest->GetOwner()->GetActive()) return false;
	return src->CheckCollision(dest);
}

void ColliderManager::VisualizeOccupiedCells(const vec3& cell, const shared_ptr<Collider>& collider)
{
	if (HasGizmoFlag(Gizmo::main->_flags, GizmoFlags::DivideSpace))
	{
		Gizmo::Width(0.5f);

		vec3 min = cell * _cellSize;
		vec3 max = min + vec3(_cellSize, _cellSize, _cellSize);

		if (collider->GetOwner()->GetType() == GameObjectType::Static)
			Gizmo::main->Box(min, max, vec4(0.0f, 1.0f, 1.0f, 1));
		else
			Gizmo::main->Box(min, max, vec4(0.5f, 0.1f, 0.1f, 1));

	}
}



void ColliderManager::CallBackBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	auto& components = collider->GetOwner()->GetComponentAll();
	for (auto& component : components)
		component->CollisionBegin(collider, other);
}

void ColliderManager::CallBackEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	auto& components = collider->GetOwner()->GetComponentAll();
	for (auto& component : components)
		component->CollisionEnd(collider, other);
}

RayHit ColliderManager::RayCast(const Ray& ray, const float& dis) const
{
	RayHit closestHit;
	closestHit.distance = dis;
	bool hitFound = false;

	for (const auto& collider : _collidersForRay)
	{
		RayHit currentHit;
		currentHit.distance = dis;  // 최대 거리로 초기화
		if (collider->RayCast(ray, dis, currentHit))
		{
			if (currentHit.distance < closestHit.distance)
			{
				closestHit = currentHit;
				hitFound = true;
			}
		}
	}
	for (auto& terrain : TerrainManager::main->_terrains)
	{
		RayHit currentHit;
		if (terrain->RayCast(ray, dis, currentHit))
		{
			if (currentHit.distance < closestHit.distance)
			{
				closestHit = currentHit;
				hitFound = true;
			}
		}
	}


	return closestHit;
}


