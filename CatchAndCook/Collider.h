
#include "ColliderManager.h"
#include "Component.h"
#include "Game.h"

enum class CollisionType
{
	Box,
	Sphere,
	Frustum
};

union BoundingUnion
{
	BoundingOrientedBox box;
	BoundingSphere sphere;
	BoundingFrustum frustum;
};



class Collider : public Component
{
public:
	Collider();
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
	CollisionType GetType() { return _type; }
	vec3 GetCenter();
	bool CheckCollision(const std::shared_ptr<Collider>& other);
	bool RayCast(const Ray& ray, const float& dis, RayHit& hit);
	void SetBoundingBox(vec3 center,vec3 extents);
	void SetBoundingSphere(vec3 center,float radius);
	void SetBoundingFrustum(BoundingFrustum& boundingFrustum);
	void CalculateBounding();
	pair<vec3, vec3> GetMinMax();

	BoundingUnion& GetBoundUnion() {return _bound;}

private:
	friend class ColliderManager;

private:
	CollisionType _type;
	BoundingUnion _orgin;
	BoundingUnion _bound;

	int groupId = 0;
	std::weak_ptr<GameObject> groupRootObject;
};
