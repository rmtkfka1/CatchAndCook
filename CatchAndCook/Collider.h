
#include "Component.h"

enum class CollisionType
{
	Box,
	Sphere,
	Frustum
};

struct BoundingCapsule {
	XMFLOAT3 p0;   // 선분 시작점
	XMFLOAT3 p1;   // 선분 끝점
	float radius = 0.5f;  // 캡슐 반지름
	BoundingCapsule();
	BoundingCapsule(XMFLOAT3 p0,XMFLOAT3 p1, float radius);

	static float ClosestDistanceBetweenSegmentsSquared(FXMVECTOR A0,FXMVECTOR A1,FXMVECTOR B0,FXMVECTOR B1);
	static float DistancePointSegmentSquared(FXMVECTOR segStart,FXMVECTOR segEnd,FXMVECTOR point);
	static float DistancePointAABBSquared(const XMFLOAT3& point,const XMFLOAT3& boxMin,const XMFLOAT3& boxMax);
	static float DistanceSegmentAABBSquared(const XMFLOAT3& p0,const XMFLOAT3& p1,const XMFLOAT3& boxMin,
	                                 const XMFLOAT3& boxMax);
	static bool RaySphereIntersection(
	FXMVECTOR rayOrigin,
	FXMVECTOR rayDir,
	FXMVECTOR sphereCenter,
	float sphereRadius,
	float maxDistance,
	float& tHit);

	static bool Intersects(const BoundingCapsule& capA, const BoundingCapsule& capB);
	static bool Intersects(const BoundingCapsule& capA,const BoundingBox& capB);
	static bool Intersects(const BoundingCapsule& capA,const BoundingSphere& capB);
	static bool Intersects(const BoundingCapsule& capA,const BoundingOrientedBox& capB);
	static bool Intersects(const BoundingCapsule& capA,const BoundingFrustum& capB);
	static bool Intersects(const BoundingCapsule& capA, FXMVECTOR rayOrigin,FXMVECTOR rayDir,float maxDistance,float* outDistance);
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
