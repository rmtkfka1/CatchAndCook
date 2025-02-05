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
		auto& src = _dynamicColliders[i];

		for(int j = 0;j<_staticColliders.size();++j)
		{
			auto& dest = _staticColliders[j];

			if(src->CheckCollision(dest.get()))
			{
				if(src->_collisionList.contains(dest.get())==false)
				{
					CallBackBegin(src,dest);
					CallBackBegin(dest,src);
					src->_collisionList.insert(dest.get());
					dest->_collisionList.insert(src.get());
				}
			}

			else
			{
				if(src->_collisionList.contains(dest.get()))
				{
					CallBackEnd(src,dest);
					CallBackEnd(dest,src);
					src->_collisionList.erase(dest.get());
					dest->_collisionList.erase(src.get());
				
				}
			}
		}

		for(int j = i+1; j< _dynamicColliders.size(); ++j)
		{
			auto& dest = _dynamicColliders[j];

			if(src->CheckCollision(dest.get()))
			{
				if(src->_collisionList.contains(dest.get())==false)
				{
					CallBackBegin(src,dest);
					CallBackBegin(dest,src);
					src->_collisionList.insert(dest.get());
					dest->_collisionList.insert(src.get());
				}
			}

			else
			{
				if(src->_collisionList.contains(dest.get())) 
				{
					CallBackEnd(src,dest);
					CallBackEnd(dest, src);
					src->_collisionList.erase(dest.get());
					dest->_collisionList.erase(src.get());
				}
			}
		}
	}
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
