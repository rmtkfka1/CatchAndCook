﻿#include "pch.h"
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
	auto& [min, max] = collider->GetMinMax();
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

vector<vec3> ColliderManager::GetOccupiedCells(const Ray& ray) const
{
	


	
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
		}
	}

	if (_colliderLinkTable.find(collider) == _colliderLinkTable.end()) {
		_colliderLinkTable[collider] = {};
	}
}

void ColliderManager::AddColliderForRay(const std::shared_ptr<Collider>& collider)
{
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

std::unordered_set<std::shared_ptr<Collider>> ColliderManager::GetPotentialCollisions(const std::shared_ptr<Collider>& collider) const
{
	std::unordered_set<std::shared_ptr<Collider>> potentialCollisions;
	auto occupiedCells = GetOccupiedCells(collider);

	for (const auto& cell : occupiedCells)
	{
		//static Coliider 은 다이나믹 객체들만 후보군에 오름.
		if (collider->GetOwner()->GetType() == GameObjectType::Static)
		{
			auto it = _dynamicColliderGrids.find(cell);

			if (it != _dynamicColliderGrids.end())
			{
				auto& colliders = it->second;
				potentialCollisions.insert(colliders.begin(), colliders.end());
			}

		}
		else if (collider->GetOwner()->GetType() == GameObjectType::Dynamic)
		{
			//dynamic Coliider 은 스태틱,다이나믹 객체들이 후보군에 오름.
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
		}
	}

	return potentialCollisions;
}

void ColliderManager::Update()
{
	//Static 객체와 Dynamic 객체의 충돌 체크
	for (auto& [cell, colliders] : _staticColliderGrids)
	{
		for (auto& collider : colliders)
		{
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

	//Dynamic 객체와 Dynamic 객체의 충돌 체크
	for (auto& [cell, colliders] : _dynamicColliderGrids)
	{
		for (auto& collider : colliders)
		{
			auto& potentialCollisions = GetPotentialCollisions(collider);

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

	std::unordered_map<vec3, std::vector<std::shared_ptr<Collider>>, PositionHash> _newDynamicColliderGrids;
	_newDynamicColliderGrids.reserve(_dynamicColliderGrids.size());

	for (auto& collider : _colliderDynamic)
	{
		auto occupiedCells = GetOccupiedCells(collider);
		for (const auto& cell : occupiedCells)
		{
			_newDynamicColliderGrids[cell].push_back(collider);
		}
	}

	_dynamicColliderGrids =  std::move(_newDynamicColliderGrids);
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
	closestHit.distance = dis;  
	bool hitFound = false;
#pragma region 
	for(const auto& collider : _collidersForRay)
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
	return closestHit;
}


