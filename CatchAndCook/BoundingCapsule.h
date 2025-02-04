#pragma once


struct BoundingCapsule {
	XMFLOAT3 p0;   // 선분 시작점
	XMFLOAT3 p1;   // 선분 끝점
	float radius = 0.5f;  // 캡슐 반지름
	BoundingCapsule();
	BoundingCapsule(XMFLOAT3 p0,XMFLOAT3 p1,float radius);

	static float ClosestDistanceBetweenSegmentsSquared(FXMVECTOR A0,FXMVECTOR A1,FXMVECTOR B0,FXMVECTOR B1);
	static float DistancePointSegmentSquared(FXMVECTOR segStart,FXMVECTOR segEnd,FXMVECTOR point);
	static float DistancePointAABBSquared(const XMFLOAT3& point,const XMFLOAT3& boxMin,const XMFLOAT3& boxMax);
	static float DistanceSegmentAABBSquared(const XMFLOAT3& p0,const XMFLOAT3& p1,const XMFLOAT3& boxMin, const XMFLOAT3& boxMax);
	static bool RaySphereIntersection(FXMVECTOR rayOrigin,FXMVECTOR rayDir, FXMVECTOR sphereCenter, float sphereRadius, float maxDistance, float& tHit);

	void Transform(const Matrix& matrix,BoundingCapsule& capB) const;

	bool Intersects(const BoundingCapsule& capB) const;
	bool Intersects(const BoundingBox& capB) const;
	bool Intersects(const BoundingSphere& capB) const;
	bool Intersects(const BoundingOrientedBox& capB) const;
	bool Intersects(const BoundingFrustum& capB) const;
	bool Intersects(const FXMVECTOR& rayOrigin, const FXMVECTOR& rayDir, float maxDistance, float* outDistance) const;
};