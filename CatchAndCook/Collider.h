
#include "Component.h"

enum class CollisionType
{
	Box,
	Sphere,
	Capsule,
	Frustum
};

class Collision
{
public:
	Collision();
	Collision(CollisionType type);

	CollisionType GetType(){
		return _type;
	};
	void SetType(CollisionType type){
		_type = type;
	};

	void SetBound(BoundingBox box);
	void SetBound(BoundingSphere sphere);
	void SetBound(BoundingFrustum frustum);
	BoundingOrientedBox GetWorldBound(std::shared_ptr<Transform> transform);
private:
	CollisionType _type;

	BoundingBox _box;
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
	void Collision(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void SetDestroy() override;
	void Destroy() override;
};
