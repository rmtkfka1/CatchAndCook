#include "pch.h"
#include "ColliderManager.h"

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
}

void ColliderManager::RemoveCollider(const std::shared_ptr<Collider>& collider)
{
	if(collider->GetOwner()->GetType() == GameObjectType::Static)
		if(auto it = std::ranges::find(_staticColliders,collider); it != _staticColliders.end())
			_staticColliders.erase(it);
	if(collider->GetOwner()->GetType() == GameObjectType::Dynamic)
		if(auto it = std::ranges::find(_dynamicColliders,collider); it != _dynamicColliders.end())
			_dynamicColliders.erase(it);
}

void ColliderManager::Update()
{


	for(int i = 0;i<_dynamicColliders.size();++i)
	{
		auto& currentCollider = _dynamicColliders[i];

		for(int j = 0;j<_staticColliders.size();++j)
		{
			auto& otherCollider = _staticColliders[j];

			if(currentCollider->CheckCollision(otherCollider.get()))
			{
				if(!otherCollider->_collisionList.contains(currentCollider.get()))
				{
					CallBackBegin(currentCollider,otherCollider);
					CallBackBegin(otherCollider,currentCollider);
					otherCollider->_collisionList.insert(currentCollider.get());
					currentCollider->_collisionList.insert(otherCollider.get());
				}
			}

			else
			{
				if(otherCollider->_collisionList.contains(currentCollider.get()))
				{
					CallBackEnd(currentCollider,otherCollider);
					CallBackEnd(otherCollider,currentCollider);
					otherCollider->_collisionList.erase(currentCollider.get());
					currentCollider->_collisionList.erase(otherCollider.get());
				}
			}
		}

		for(int j = i+1; j< _dynamicColliders.size(); ++j)
		{
			auto& otherCollider = _dynamicColliders[j];

			// 그룹 개념 적용해야함.
			// 그리고 if 충돌했을시
			if(currentCollider->CheckCollision(otherCollider.get()))
			{
				if(!otherCollider->_collisionList.contains(currentCollider.get()))
				{
					CallBackBegin(currentCollider,otherCollider);
					CallBackBegin(otherCollider,currentCollider);
					otherCollider->_collisionList.insert(currentCollider.get());
					currentCollider->_collisionList.insert(otherCollider.get());
				}
			} else
			{
				if(otherCollider->_collisionList.contains(currentCollider.get())) 
				{
					CallBackEnd(currentCollider,otherCollider);
					CallBackEnd(otherCollider, currentCollider);
					otherCollider->_collisionList.erase(currentCollider.get());
					currentCollider->_collisionList.erase(otherCollider.get());
				}
			}
		}
	}
}

void ColliderManager::CallBackBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	auto& components = collider->GetOwner()->GetComponentAll();
	for(auto& component : components)
		if(component != collider)
			component->CollisionBegin(collider,other);
}

void ColliderManager::CallBackEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	auto& components = collider->GetOwner()->GetComponentAll();
	for(auto& component : components)
		if(component != collider)
			component->CollisionEnd(collider,other);
}
