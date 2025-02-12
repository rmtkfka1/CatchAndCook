#include "pch.h"
#include "ColliderManager.h"

#include <algorithm>

#include "Collider.h"


unique_ptr<ColliderManager> ColliderManager::main = nullptr;


void ColliderManager::AddCollider(const std::shared_ptr<Collider>& collider)
{
	if(collider->GetOwner()->GetType() == GameObjectType::Static)
			_staticColliders.push_back(collider);
	if(collider->GetOwner()->GetType() == GameObjectType::Dynamic)
			_dynamicColliders.push_back(collider);
}

void ColliderManager::RemoveCollider(const std::shared_ptr<Collider>& collider)
{
	if(collider->GetOwner()->GetType() == GameObjectType::Static)
	{
		if(auto it = std::ranges::find(_staticColliders,collider); it != _staticColliders.end())
		{
			for(auto& ele : collider->_collisionList)
			{
				if(auto sharedEle = ele.lock())
				{
					CallBackEnd(collider,sharedEle);
					CallBackEnd(sharedEle,collider);
				}
			}

			collider->_collisionList.clear();

			_staticColliders.erase(it);
		}
	}

	if(collider->GetOwner()->GetType() == GameObjectType::Dynamic)
	{
		if(auto it = std::ranges::find(_dynamicColliders,collider); it != _dynamicColliders.end())
		{
	
			for(auto& ele : collider->_collisionList)
			{
				if(auto sharedEle = ele.lock())
				{
					CallBackEnd(collider,sharedEle);
					CallBackEnd(sharedEle,collider);
				}
			}

			collider->_collisionList.clear();
			_dynamicColliders.erase(it);
		}
	}



	
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
				if(src->_collisionList.contains(dest)==false)
				{
					src->_collisionList.insert(dest);
					dest->_collisionList.insert(src);
					CallBackBegin(src,dest);
					CallBackBegin(dest,src);
				}
			}

			else
			{
				if(src->_collisionList.contains(dest))
				{
					src->_collisionList.erase(dest);
					dest->_collisionList.erase(src);
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
				if(src->_collisionList.contains(dest)==false)
				{
					src->_collisionList.insert(dest);
					dest->_collisionList.insert(src);
					CallBackBegin(src,dest);
					CallBackBegin(dest,src);
				}
			}

			else
			{
				if(src->_collisionList.contains(dest))
				{
					src->_collisionList.erase(dest);
					dest->_collisionList.erase(src);
					CallBackEnd(src,dest);
					CallBackEnd(dest,src);
				}
			}
		}
	}
} 

//무언가와 충돌하고 있는지 체크
bool ColliderManager::IsCollision(const std::shared_ptr<Collider>& a) 
{
	return a->IsCollision();
}

//특정누군가와 충돌하고 있는지 체크
bool ColliderManager::IsCollision(const std::shared_ptr<Collider>& src,const std::shared_ptr<Collider>& dest)
{
	return src->IsCollision(dest);
}

bool ColliderManager::TotalCheckCollision(const std::shared_ptr<Collider>& a, const std::shared_ptr<Collider>& b)
{
	return (a->groupId != b->groupId) && a->CheckCollision(b);
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

