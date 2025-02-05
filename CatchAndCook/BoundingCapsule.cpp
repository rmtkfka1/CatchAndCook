#include "pch.h"
#include "BoundingCapsule.h"


BoundingCapsule::BoundingCapsule()
{

}

BoundingCapsule::BoundingCapsule(XMFLOAT3 p0,XMFLOAT3 p1,float radius)
{
    this->p0 = p0;
    this->p1 = p1;
    this->radius = radius;
}

// 두 선분(A: A0-A1, B: B0-B1) 사이의 최단 거리 제곱을 계산하는 함수
float BoundingCapsule::ClosestDistanceBetweenSegmentsSquared(FXMVECTOR A0,FXMVECTOR A1,FXMVECTOR B0,FXMVECTOR B1)
{
    // 선분 방향 벡터 계산
    XMVECTOR u = XMVectorSubtract(A1,A0); // A의 방향
    XMVECTOR v = XMVectorSubtract(B1,B0); // B의 방향
    XMVECTOR w = XMVectorSubtract(A0,B0);   // 두 선분 시작점 사이 벡터

    // 내적 계산
    float a = XMVectorGetX(XMVector3Dot(u,u)); // u·u, 즉 |u|^2
    float b = XMVectorGetX(XMVector3Dot(u,v)); // u·v
    float c = XMVectorGetX(XMVector3Dot(v,v)); // v·v, 즉 |v|^2
    float d = XMVectorGetX(XMVector3Dot(u,w)); // u·w
    float e = XMVectorGetX(XMVector3Dot(v,w)); // v·w

    float D = a * c - b * b;  // 분모
    float s,t;             // A, B 선분 상의 파라미터 (각각 0~1 범위)
    const float SMALL_NUM = 1e-6f; // 부동소수점 비교용 작은 값

    // 두 선분이 거의 평행한 경우
    if(D < SMALL_NUM)
    {
        s = 0.0f;         // A쪽은 고정
        t = (c > SMALL_NUM) ? (e / c) : 0.0f; // B쪽 파라미터 (c가 0에 가까우면 0으로 처리)
        t = XMMin(XMMax(t,0.0f),1.0f); // [0,1]로 클램프
    } else
    {
        // 일반적인 경우
        s = (b * e - c * d) / D;
        t = (a * e - b * d) / D;
        // s가 [0,1] 범위를 벗어나면 클램프하고 t 재계산
        if(s < 0.0f)
        {
            s = 0.0f;
            t = (c > SMALL_NUM) ? (e / c) : 0.0f;
        } else if(s > 1.0f)
        {
            s = 1.0f;
            t = (c > SMALL_NUM) ? ((e + b) / c) : 0.0f;
        }
    }

    // t가 [0,1] 범위를 벗어나면 클램프하고 s 재계산
    if(t < 0.0f)
    {
        t = 0.0f;
        if(-d < 0.0f)
            s = 0.0f;
        else if(-d > a)
            s = 1.0f;
        else
            s = -d / a;
    } else if(t > 1.0f)
    {
        t = 1.0f;
        if((-d + b) < 0.0f)
            s = 0.0f;
        else if((-d + b) > a)
            s = 1.0f;
        else
            s = (-d + b) / a;
    }

    // 두 선분 상의 최근접 점 계산
    XMVECTOR cpA = XMVectorAdd(A0,XMVectorScale(u,s));
    XMVECTOR cpB = XMVectorAdd(B0,XMVectorScale(v,t));

    // 최근접 두 점 사이의 벡터와 제곱 거리 계산
    XMVECTOR diff = XMVectorSubtract(cpA,cpB);
    float sqDistance = XMVectorGetX(XMVector3Dot(diff,diff));
    return sqDistance;
}

float BoundingCapsule::DistancePointSegmentSquared(FXMVECTOR segStart,FXMVECTOR segEnd,FXMVECTOR point)
{
    // 선분의 방향 벡터: segVec = segEnd - segStart
    XMVECTOR segVec = XMVectorSubtract(segEnd,segStart);
    // 점에서 선분 시작점까지의 벡터: ptVec = point - segStart
    XMVECTOR ptVec  = XMVectorSubtract(point,segStart);

    // 선분 길이의 제곱: |segVec|^2
    float segLenSq = XMVectorGetX(XMVector3Dot(segVec,segVec));

    // 선분이 거의 한 점과 같은 경우 (길이가 0에 가까우면)
    if(segLenSq < 1e-6f)
    {
        // 선분이 사실상 점이므로, 시작점과의 거리 제곱을 반환
        return XMVectorGetX(XMVector3Dot(ptVec,ptVec));
    }

    // 파라미터 t: segStart에서 segEnd 방향으로 얼마나 떨어진 지점이 point와 수직 투영되는지 계산
    float t = XMVectorGetX(XMVector3Dot(ptVec,segVec)) / segLenSq;

    if(t <= 0.0f)
    {
        // t가 0 이하이면, 가장 가까운 점은 segStart
        return XMVectorGetX(XMVector3Dot(ptVec,ptVec));
    } else if(t >= 1.0f)
    {
        // t가 1 이상이면, 가장 가까운 점은 segEnd
        XMVECTOR diff = XMVectorSubtract(point,segEnd);
        return XMVectorGetX(XMVector3Dot(diff,diff));
    } else
    {
        // 0 < t < 1인 경우, 선분 위의 내부 점이 가장 가까움
        // 점의 투영 위치 계산: projection = segStart + t * segVec
        XMVECTOR projection = XMVectorAdd(segStart,XMVectorScale(segVec,t));
        // 점과 투영된 점 사이의 차이 벡터
        XMVECTOR diff = XMVectorSubtract(point,projection);
        // 차이 벡터의 제곱 거리 반환
        return XMVectorGetX(XMVector3Dot(diff,diff));
    }
}

// (3) 점과 AABB 사이의 제곱 거리 계산
// 주어진 점과, AABB의 최소, 최대 좌표(boxMin, boxMax)를 사용
float BoundingCapsule::DistancePointAABBSquared(const XMFLOAT3& point,const XMFLOAT3& boxMin,const XMFLOAT3& boxMax)
{
    float dx = 0.0f,dy = 0.0f,dz = 0.0f;
    if(point.x < boxMin.x)       dx = boxMin.x - point.x;
    else if(point.x > boxMax.x)  dx = point.x - boxMax.x;
    if(point.y < boxMin.y)       dy = boxMin.y - point.y;
    else if(point.y > boxMax.y)  dy = point.y - boxMax.y;
    if(point.z < boxMin.z)       dz = boxMin.z - point.z;
    else if(point.z > boxMax.z)  dz = point.z - boxMax.z;
    return dx * dx + dy * dy + dz * dz;
}

// (4) 선분과 AABB 사이의 최단 거리 제곱 계산
// 선분은 p0, p1로 주어지고, AABB는 boxMin, boxMax로 주어짐.
// 선분 내의 후보 파라미터 t들을 구해 해당 점에서 AABB까지의 거리 제곱의 최소값을 구합니다.
float BoundingCapsule::DistanceSegmentAABBSquared(const XMFLOAT3& p0,const XMFLOAT3& p1,const XMFLOAT3& boxMin,const XMFLOAT3& boxMax)
{
    XMVECTOR P0 = XMLoadFloat3(&p0);
    XMVECTOR P1 = XMLoadFloat3(&p1);
    std::vector<float> candidates;
    candidates.push_back(0.0f);
    candidates.push_back(1.0f);

    // 각 좌표축별로, 선분과 AABB 경계의 교차 시점을 후보로 추가
    float p0Arr[3] = {p0.x,p0.y,p0.z};
    float p1Arr[3] = {p1.x,p1.y,p1.z};
    float dArr[3]  = {p1.x - p0.x,p1.y - p0.y,p1.z - p0.z};
    float bmin[3]  = {boxMin.x,boxMin.y,boxMin.z};
    float bmax[3]  = {boxMax.x,boxMax.y,boxMax.z};

    for(int i = 0; i < 3; i++)
    {
        if(fabs(dArr[i]) > 1e-6f)
        {
            float t1 = (bmin[i] - p0Arr[i]) / dArr[i];
            float t2 = (bmax[i] - p0Arr[i]) / dArr[i];
            if(t1 >= 0.0f && t1 <= 1.0f)
                candidates.push_back(t1);
            if(t2 >= 0.0f && t2 <= 1.0f)
                candidates.push_back(t2);
        }
    }

    float minDistSq = FLT_MAX;
    for(float t : candidates)
    {
        XMVECTOR P = XMVectorLerp(P0,P1,t);
        // AABB의 범위(clamp)를 사용하여 점 P와 AABB 사이의 거리를 구함
        XMVECTOR boxMinVec = XMLoadFloat3(&boxMin);
        XMVECTOR boxMaxVec = XMLoadFloat3(&boxMax);
        XMVECTOR clamped = XMVectorClamp(P,boxMinVec,boxMaxVec);
        XMVECTOR diff = XMVectorSubtract(P,clamped);
        float distSq = XMVectorGetX(XMVector3Dot(diff,diff));
        if(distSq < minDistSq)
            minDistSq = distSq;
    }
    return minDistSq;
}

bool BoundingCapsule::RaySphereIntersection(FXMVECTOR rayOrigin,FXMVECTOR rayDir,FXMVECTOR sphereCenter,
    float sphereRadius,float maxDistance,float& tHit)
{

    // m = rayOrigin - sphereCenter
    XMVECTOR m = XMVectorSubtract(rayOrigin,sphereCenter);
    float b = XMVectorGetX(XMVector3Dot(m,rayDir));
    float c = XMVectorGetX(XMVector3Dot(m,m)) - sphereRadius * sphereRadius;
    // 만약 레이 시작점이 구 바깥에 있고, rayDir이 구에서 멀어지는 경우 충돌 없음
    if(c > 0.0f && b > 0.0f)
        return false;
    float discr = b * b - c;
    if(discr < 0.0f)
        return false;
    float t = -b - sqrtf(discr);
    if(t < 0.0f)
        t = 0.0f;
    if(t <= maxDistance)
    {
        tHit = t;
        return true;
    }
    return false;
}

void BoundingCapsule::Transform(const Matrix& matrix, BoundingCapsule& capB) const
{
    // 1. p0와 p1를 SRT 행렬로 변환 (위치 값은 XMVector3TransformCoord 사용)
    XMVECTOR p0Vec = XMLoadFloat3(&p0);
    XMVECTOR p1Vec = XMLoadFloat3(&p1);

    XMVECTOR p0Transformed = XMVector3TransformCoord(p0Vec, matrix);
    XMVECTOR p1Transformed = XMVector3TransformCoord(p1Vec, matrix);

    XMStoreFloat3(&capB.p0, p0Transformed);
    XMStoreFloat3(&capB.p1, p1Transformed);

    // 2. SRT 행렬에서 스케일 성분을 추출
    // DirectXMath에서는 행렬의 r[0], r[1], r[2] 벡터에 각각 X, Y, Z 축에 해당하는 방향이 포함됩니다.
    // 각 벡터의 길이를 구하면 해당 축의 스케일 값을 알 수 있습니다.

	XMVECTOR p0Transformed3 = XMVector3TransformNormal(Vector3(1,0, 1),matrix);

    XMMATRIX matrix2 = XMLoadFloat4x4(&matrix);

    float scaleX = XMVectorGetX(XMVector3Length(matrix2.r[0]));
    float scaleY = XMVectorGetX(XMVector3Length(matrix2.r[1]));
    float scaleZ = XMVectorGetX(XMVector3Length(matrix2.r[2]));

    // 균일 스케일의 경우 세 값이 모두 동일합니다.
    // 비균일 스케일인 경우, 보통 반지름에 가장 큰 스케일 값을 곱해줍니다.
    
    // 3. 반지름에 스케일 적용
    capB.radius = radius * std::max(scaleX, scaleZ);
}

// 캡슐 간 충돌 검사 함수
bool BoundingCapsule::Intersects(const BoundingCapsule& capB) const
{
    // 캡슐의 중심 선분을 DirectXMath 벡터로 로드
    XMVECTOR A0 = XMLoadFloat3(&p0);
    XMVECTOR A1 = XMLoadFloat3(&p1);
    XMVECTOR B0 = XMLoadFloat3(&capB.p0);
    XMVECTOR B1 = XMLoadFloat3(&capB.p1);
    BoundingBox b;

    // 두 선분 사이의 최단 거리 제곱 계산
    float sqDist = ClosestDistanceBetweenSegmentsSquared(A0,A1,B0,B1);
    float radiusSum = radius + capB.radius;

    // 충돌 판정: 두 선분 사이의 최단 거리가 반지름 합 이하이면 충돌
    return (sqDist <= radiusSum * radiusSum);
}


bool BoundingCapsule::Intersects(const BoundingBox& box) const
{
    // AABB는 Center와 Extents로 정의되므로, 먼저 최소/최대 좌표를 구합니다.
    XMFLOAT3 boxMin,boxMax;
    boxMin.x = box.Center.x - box.Extents.x;
    boxMin.y = box.Center.y - box.Extents.y;
    boxMin.z = box.Center.z - box.Extents.z;
    boxMax.x = box.Center.x + box.Extents.x;
    boxMax.y = box.Center.y + box.Extents.y;
    boxMax.z = box.Center.z + box.Extents.z;

    float sqDist = DistanceSegmentAABBSquared(p0,p1,boxMin,boxMax);
    return sqDist <= radius * radius;
}

// (D) 캡슐–OBB 충돌 판정
bool BoundingCapsule::Intersects(const BoundingOrientedBox& obb) const
{
    // OBB의 로컬 좌표로 변환: 캡슐 선분의 각 점에 대해
    XMVECTOR obbCenter = XMLoadFloat3(&obb.Center);
    XMVECTOR obbOrientation = XMLoadFloat4(&obb.Orientation);
    XMVECTOR invOrientation = XMQuaternionConjugate(obbOrientation);

    XMVECTOR p0World = XMLoadFloat3(&p0);
    XMVECTOR p1World = XMLoadFloat3(&p1);
    // OBB 중심을 기준으로 하고, 회전의 역(=로컬 변환) 적용
    XMVECTOR p0Local = XMVector3Rotate(XMVectorSubtract(p0World,obbCenter),invOrientation);
    XMVECTOR p1Local = XMVector3Rotate(XMVectorSubtract(p1World,obbCenter),invOrientation);

    // OBB는 로컬 공간에서 AABB로 표현됩니다.
    XMFLOAT3 p0LocalF,p1LocalF;
    XMStoreFloat3(&p0LocalF,p0Local);
    XMStoreFloat3(&p1LocalF,p1Local);
    XMFLOAT3 ext = obb.Extents;
    XMFLOAT3 aabbMin = {-ext.x,-ext.y,-ext.z};
    XMFLOAT3 aabbMax = {ext.x,ext.y,ext.z};

    float sqDist = DistanceSegmentAABBSquared(p0LocalF,p1LocalF,aabbMin,aabbMax);
    return sqDist <= radius * radius;
}

// (E) 캡슐–프러스텀 충돌 판정
bool BoundingCapsule::Intersects(const BoundingFrustum& frustum) const
{
    XMVECTOR p0Vec = XMLoadFloat3(&p0);
    XMVECTOR p1Vec = XMLoadFloat3(&p1);
    // 프러스텀의 6개 평면에 대해 검사합니다.
    XMVECTOR p[6];
    frustum.GetPlanes(&p[0],&p[1],&p[2],&p[3],&p[4],&p[5]);
    for(int i = 0; i < 6; i++)
    {
        float d0 = XMVectorGetX(XMPlaneDotCoord(p[i],p0Vec));
        float d1 = XMVectorGetX(XMPlaneDotCoord(p[i],p1Vec));
        // 선형 함수이므로, 두 점의 부호가 다르면 선분은 평면을 통과하여 최소 거리는 0입니다.
        float dmin = (d0 < d1) ? d0 : d1;
        if(d0 * d1 < 0.0f)
            dmin = 0.0f;
        // 해당 평면의 음의 영역(밖)으로 캡슐 전체가 밀려나 있다면 충돌 없음.
        if(dmin < -radius)
            return false;
    }
    return true;
}

bool BoundingCapsule::Intersects(const FXMVECTOR& rayOrigin, const FXMVECTOR& rayDir,float maxDistance,float* outDistance) const
{
    XMVECTOR A = XMLoadFloat3(&p0);
    XMVECTOR B = XMLoadFloat3(&p1);
    float r = radius;

    // 캡슐 중심선 벡터와 길이 제곱
    XMVECTOR d = XMVectorSubtract(B,A);
    float dd = XMVectorGetX(XMVector3Dot(d,d));
    const float epsilon = 1e-6f;

    float tCandidate = FLT_MAX;  // 후보 충돌 거리
    bool  hitFound   = false;

    // [Case 1] 캡슐이 degenerate하여 사실상 구(sphere)인 경우
    if(dd < epsilon)
    {
        // 캡슐이 한 점 A와 같으므로, ray-sphere 테스트
        if(RaySphereIntersection(rayOrigin,rayDir,A,r,maxDistance,tCandidate))
        {
            if(outDistance) *outDistance = tCandidate;
            return true;
        }
        return false;
    }

    // [Case 2] 캡슐이 일반적인 경우 (중심선 구간 길이 > 0)
    // m = rayOrigin - A
    XMVECTOR m = XMVectorSubtract(rayOrigin,A);
    float md = XMVectorGetX(XMVector3Dot(m,d));
    float nd = XMVectorGetX(XMVector3Dot(rayDir,d));

    // 무한 원통(infinite cylinder)과의 충돌을 위한 계수 계산  
    // (원통의 축은 캡슐의 중심선 d, 반지름 r)
    // a = 1 - (nd^2)/dd, b = (m·rayDir) - (md*nd)/dd, c = (m·m) - (md^2)/dd - r^2
    float mDotDir = XMVectorGetX(XMVector3Dot(m,rayDir));
    float a = 1.0f - (nd * nd) / dd;
    float b = mDotDir - (md * nd) / dd;
    float c = XMVectorGetX(XMVector3Dot(m,m)) - (md * md) / dd - r * r;

    bool cylinderHit = false;
    float tCylinder = FLT_MAX;
    float sCylinder = 0.0f;  // 캡슐 중심선 상의 파라미터 (0: A, 1: B)

    // [2-A] 일반적인 경우: a가 충분히 크면 quadratic 식을 풀어 무한 원통과의 교차 t 값을 구함
    if(fabs(a) > epsilon)
    {
        float disc = b * b - a * c;
        if(disc >= 0.0f)
        {
            float sqrt_disc = sqrtf(disc);
            float t0 = (-b - sqrt_disc) / a;
            float t1 = (-b + sqrt_disc) / a;
            // 가능한 t 중 가장 작은 양의 해 선택
            float tTemp = (t0 >= 0.0f) ? t0 : t1;
            if(tTemp >= 0.0f && tTemp <= maxDistance)
            {
                // s = (md + t * nd) / dd  (캡슐 중심선 파라미터)
                float s = (md + tTemp * nd) / dd;
                // 만약 s가 [0,1] 내에 있다면 원통 부분에서 유효한 해
                if(s >= 0.0f && s <= 1.0f)
                {
                    cylinderHit = true;
                    tCylinder = tTemp;
                    sCylinder = s;
                }
            }
        }
    } else
    {
        // [2-B] a가 0에 가까우면, 레이 방향이 캡슐 축과 거의 평행함.
        // 이 경우, ray와 캡슐 축 사이의 수선거리가 상수값.
        float m2 = XMVectorGetX(XMVector3Dot(m,m));
        float proj = (dd > epsilon) ? (md / dd) : 0.0f;
        float dist2 = m2 - (md * md) / dd;
        if(dist2 <= r * r)
        {
            // 레이 원점이 무한 원통 내부임
            // 캡슐 중심선의 양쪽 평면(s=0, s=1)과의 교차 t 값을 계산
            if(fabs(nd) > epsilon)
            {
                float tA = -md / nd;           // s = 0 (캡슐의 시작점 A)
                float tB = (dd - md) / nd;       // s = 1 (캡슐의 끝점 B)
                float tEnter = FLT_MAX;
                if(tA >= 0.0f)
                    tEnter = tA;
                if(tB >= 0.0f && tB < tEnter)
                    tEnter = tB;
                if(tEnter <= maxDistance)
                {
                    cylinderHit = true;
                    tCylinder = tEnter;
                    // sCylinder은 tA에 가까우면 0, tB에 가까우면 1로 결정
                    sCylinder = (fabs(tEnter - tA) < fabs(tEnter - tB)) ? 0.0f : 1.0f;
                }
            }
        }
    }

    // [Case 3] 구면 캡: 원통 부분이 유효하지 않거나 s가 [0,1] 범위를 벗어난 경우,
    // 양쪽 끝점에서의 레이-구 충돌을 검사합니다.
    float tCapA = FLT_MAX,tCapB = FLT_MAX;
    bool capAHit = false,capBHit = false;
    // 캡슐의 시작점 A에 대한 ray-sphere 검사
    if(RaySphereIntersection(rayOrigin,rayDir,A,r,maxDistance,tCapA))
        capAHit = true;
    // 캡슐의 끝점 B에 대한 ray-sphere 검사
    if(RaySphereIntersection(rayOrigin,rayDir,B,r,maxDistance,tCapB))
        capBHit = true;

    // [최종] 여러 후보(tCylinder, tCapA, tCapB) 중에서 최소의 양의 t를 선택합니다.
    float tMin = FLT_MAX;
    if(cylinderHit && tCylinder < tMin)
        tMin = tCylinder;
    if(capAHit && tCapA < tMin)
        tMin = tCapA;
    if(capBHit && tCapB < tMin)
        tMin = tCapB;

    if(tMin < FLT_MAX && tMin <= maxDistance)
    {
        if(outDistance)
            *outDistance = tMin;
        return true;
    }
    return false;
}