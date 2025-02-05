#pragma once

class ColliderManager
{
public:
	static unique_ptr<ColliderManager> main;

	void AddCollider(const std::shared_ptr<Collider>& collider);
	void RemoveCollider(const std::shared_ptr<Collider>& collider);

	void Update();

private:
	std::vector<std::shared_ptr<Collider>> _staticColliders;
	std::vector<std::shared_ptr<Collider>> _dynamicColliders;
};

