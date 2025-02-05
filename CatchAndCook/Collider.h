
#include "Component.h"

enum class CollisionType
{
	Box,
	Sphere,
	Frustum
};

union BoundingUnion
{
	BoundingOrientedBox _box;
	BoundingSphere _sphere;
	BoundingFrustum _frustum;
};


class Collider : public Component
{
public:
	~Collider() override;
	bool IsExecuteAble() override;
	void Init() override;
	void Start() override;
	void Update() override;
	void Update2() override;
	void Enable() override;
	void Disable() override;
	void RenderBegin() override;
	void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void CollisionEnd(const std::shared_ptr<Collider>& collider,const std::shared_ptr<Collider>& other) override;
	void SetDestroy() override;
	void Destroy() override;

public:
	bool CheckCollision(Collider* other);
	void SetBoundingBox(vec3 center,vec3 extents);
	void SetBoundingSphere(vec3 center,float radius);
	void SetBoundingFrustum(BoundingFrustum& boundingFrustum);

	BoundingUnion& GetBoundUnion() {return _bound;}

private:
	std::unordered_set<Collider*> _collisionList;
	friend class ColliderManager;

private:
	CollisionType _type;
	BoundingUnion _orgin;
	BoundingUnion _bound;
};
