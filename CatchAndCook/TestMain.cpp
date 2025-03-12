#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <DirectXMath.h>
using namespace DirectX;


using vec2 = DirectX::SimpleMath::Vector2;
using vec3 = DirectX::SimpleMath::Vector3;
using vec4 = DirectX::SimpleMath::Vector4;
using Matrix = DirectX::SimpleMath::Matrix;
using Ray = DirectX::SimpleMath::Ray;
using Quaternion = DirectX::SimpleMath::Quaternion;

int main()
{
    vec4 localPos = vec4(0, 0, 0, 1);

    Matrix mat = Matrix::CreateTranslation(3.0f, 0, 0.001f);
    vec4 worldPos = vec4::Transform(localPos, mat);
    std::cout << "World Position: "
        << worldPos.x << ", " << worldPos.y << ", "
        << worldPos.z << ", " << worldPos.w << std::endl;

   
    vec3 cameraPos = vec3(0, 0, -5);
    vec3 cameraLook = vec3(0, 0, 1.0f);
    vec3 cameraUp = vec3(0, 1.0f, 0);

    Matrix ViewMatrix = XMMatrixLookToLH(cameraPos, cameraLook, cameraUp);
    vec4 viewPos = vec4::Transform(worldPos, ViewMatrix);
    std::cout << "View Position: "
        << viewPos.x << ", " << viewPos.y << ", "
        << viewPos.z << ", " << viewPos.w << std::endl;


    Matrix ProjectionMatrix = Matrix::CreatePerspectiveFieldOfView(
        XMConvertToRadians(60.0f),
        800.0f / 600.0f,
        0.1f,
        300.0f);

    vec4 clipPos = vec4::Transform(viewPos, ProjectionMatrix);
    std::cout << "Clip Position: "
        << clipPos.x << ", " << clipPos.y << ", "
        << clipPos.z << ", " << clipPos.w << std::endl;


    vec3 ndcPos = vec3(clipPos.x / clipPos.w, clipPos.y / clipPos.w, clipPos.z / clipPos.w);
    std::cout << "NDC Position: " << ndcPos.x << ", " << ndcPos.y << ", " << ndcPos.z << std::endl;

    
    float depth = clipPos.z / clipPos.w;
    std::cout << "Depth: " << depth << std::endl;


	Matrix inverseProjection = ProjectionMatrix.Invert();

	vec4 clipPos2 = vec4(ndcPos.x , ndcPos.y , ndcPos.z , 1);

	std::cout << "Clip Position2: " << clipPos2.x << ", " << clipPos2.y << ", " << clipPos2.z << ", " << clipPos2.w << std::endl;

	vec4 viewPos2 = vec4::Transform(clipPos2, inverseProjection);

    std::cout << "View Position2: " << viewPos2.x << ", " << viewPos2.y << ", " << viewPos2.z << ", " << viewPos2.w  << std::endl;

	viewPos2 /= viewPos2.w;

	std::cout << "View Position2: " << viewPos2.x << ", " << viewPos2.y << ", " << viewPos2.z << ", " << viewPos2.w << std::endl;



    return 0;
}
