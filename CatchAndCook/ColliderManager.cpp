#include "pch.h"
#include "ColliderManager.h"

#include <algorithm>

#include "Collider.h"


unique_ptr<ColliderManager> ColliderManager::main = nullptr;


void ColliderManager::AddCollider(const std::shared_ptr<Collider>& collider)
{
	if(collider->GetOwner()->GetType() == GameObjectType::Static)
		if(std::ranges::find(_staticColliders,collider) == _staticColliders.end())
			_staticColliders.push_back(collider);
	if(collider->GetOwner()->GetType() == GameObjectType::Dynamic)
		if(std::ranges::find(_dynamicColliders,collider) == _dynamicColliders.end())
			_dynamicColliders.push_back(collider);
	_colliderLinkTable.emplace(collider, unordered_set<std::shared_ptr<Collider>>{});
}

void ColliderManager::RemoveCollider(const std::shared_ptr<Collider>& collider)
{
	if(collider->GetOwner()->GetType() == GameObjectType::Static)
		if(auto it = std::ranges::find(_staticColliders,collider); it != _staticColliders.end())
			_staticColliders.erase(it);
	if(collider->GetOwner()->GetType() == GameObjectType::Dynamic)
		if(auto it = std::ranges::find(_dynamicColliders,collider); it != _dynamicColliders.end())
			_dynamicColliders.erase(it);

	for(auto& other : _colliderLinkTable[collider])
	{
		CallBackEnd(collider, other);
		CallBackEnd(other,collider);
		_colliderLinkTable[other].erase(collider);
	}
	_colliderLinkTable.erase(collider);
	
}

void ColliderManager::Update()
{
	for(int i = 0;i<_dynamicColliders.size();++i)
	{
		auto& src = _dynamicColliders[i];

		for(int j = 0;j<_staticColliders.size();++j)
		{
			auto& dest = _staticColliders[j];

			if(TotalCheckCollision(src, dest))
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

			if(TotalCheckCollision(src, dest))
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

bool ColliderManager::TotalCheckCollision(const std::shared_ptr<Collider>& a, const std::shared_ptr<Collider>& b)
{
	return (a->groupId != b->groupId) && a->CheckCollision(b.get());
}

//무언가와 충돌하고 있는지 체크
bool ColliderManager::IsCollisionStay(const std::shared_ptr<Collider>& a) const
{
	return !_colliderLinkTable.find(a)->second.empty();
}

//두개가 충돌중인지 체크
bool ColliderManager::CollisionStayContains(const std::shared_ptr<Collider>& a, const std::shared_ptr<Collider>& b)
{
	auto it = _colliderLinkTable.find(a);
	if(it != _colliderLinkTable.end())
		return it->second.contains(b);
	return false;
}
std::unordered_set<std::shared_ptr<Collider>> ColliderManager::GetCollisionStayTable(const std::shared_ptr<Collider>& a)
{
	return _colliderLinkTable.find(a)->second;
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

void ColliderManager::RayCastAll(const Vector3& worldPos,const Vector3& dir,const float& dis, std::vector<RayHit>& hits)
{
	
}