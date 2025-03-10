#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <DirectXMath.h>
using namespace DirectX;

int main()
{
    // 뷰포트 설정 (화면 좌표)
    float viewportX = 0.0f;
    float viewportY = 0.0f;
    float viewportWidth = 1800.0f;
    float viewportHeight = 600.0f;
    float minDepth = 0.0f;
    float maxDepth = 1.0f;

    // 1. 로컬 공간 좌표 (예: z=1000)
    XMFLOAT3 localSpace = XMFLOAT3(600, 0, 1000);

    // 2. 월드 행렬 (이 경우 이동 없음)
    XMMATRIX worldMat = XMMatrixTranslation(0, 0, 0);

    // 로컬 -> 월드 변환
    XMVECTOR localSpaceVec = XMLoadFloat3(&localSpace);
    XMVECTOR worldSpaceVec = XMVector3Transform(localSpaceVec, worldMat);
    XMFLOAT3 worldSpace;
    XMStoreFloat3(&worldSpace, worldSpaceVec);
    std::cout << "World Space: " << worldSpace.x << " " << worldSpace.y << " " << worldSpace.z << std::endl;

    // 3. 뷰 행렬 (카메라: eye=(0,0,0), 타겟=(0,0,1), 업=(0,1,0))
    XMVECTOR eyePos = XMVectorSet(0, 0, 0, 1);
    XMVECTOR targetPos = XMVectorSet(0, 0, 1, 1);
    XMVECTOR up = XMVectorSet(0, 1, 0, 0);
    XMMATRIX viewMat = XMMatrixLookToLH(eyePos, targetPos, up);

    // 월드 -> 뷰 변환
    XMVECTOR viewSpaceVec = XMVector3Transform(worldSpaceVec, viewMat);
    XMFLOAT3 viewSpace;
    XMStoreFloat3(&viewSpace, viewSpaceVec);
    std::cout << "View Space: " << viewSpace.x << " " << viewSpace.y << " " << viewSpace.z << std::endl;

    // 4. 투영 행렬 설정 (FOV 90°, near=0.1, far=10000)
    float fov = XMConvertToRadians(90.0f);
    float aspect = viewportWidth / viewportHeight;
    float nearPlane = 0.1f;
    float farPlane = 10000.0f;
    XMMATRIX projMat = XMMatrixPerspectiveFovLH(fov, aspect, nearPlane, farPlane);

    // 뷰 -> 클립 공간 변환
    XMVECTOR clipSpaceVec = XMVector4Transform(viewSpaceVec, projMat);
    XMFLOAT4 clipSpace;
    XMStoreFloat4(&clipSpace, clipSpaceVec);
    std::cout << "Clip Space: "
        << clipSpace.x << " " << clipSpace.y << " "
        << clipSpace.z << " " << clipSpace.w << std::endl;

    // 5. NDC 변환 (클립 스페이스를 w로 나누기)
    XMFLOAT3 ndc;
    if (clipSpace.w != 0.0f)
    {
        ndc.x = clipSpace.x / clipSpace.w;
        ndc.y = clipSpace.y / clipSpace.w;
        ndc.z = clipSpace.z / clipSpace.w;
    }
    std::cout << "NDC: "
        << ndc.x << " " << ndc.y << " " << ndc.z << std::endl;

    // 6. NDC를 스크린 공간 좌표로 변환
    float screenX = (ndc.x + 1.0f) * 0.5f * viewportWidth + viewportX;
    float screenY = (1.0f - ndc.y) * 0.5f * viewportHeight + viewportY;  // y축 반전
    float screenZ = ndc.z;  // 깊이 값은 그대로 사용

    XMVECTOR screenPos = XMVectorSet(screenX, screenY, screenZ, 1.0f);

    XMFLOAT4 screenPosF;
    XMStoreFloat4(&screenPosF, screenPos);
    std::cout << "screenPos : "
        << screenPosF.x << " " << screenPosF.y << " "
        << screenPosF.z << " " << screenPosF.w << std::endl;
    
    return 0;
}
