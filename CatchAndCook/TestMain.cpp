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
	BoundingOrientedBox orgin;
	orgin.Center = vec3(0, 0, 0);
	orgin.Extents = vec3(0.5f, 0.5f, 0.5f);
	orgin.Orientation = Quaternion::Identity;

    BoundingOrientedBox box;
	box.Center = vec3(0, 0, 0);
    box.Extents = vec3(0.5f, 0.5f, 0.5f);
	box.Orientation = Quaternion::Identity;

	Matrix mat = Matrix::CreateRotationY(XM_PIDIV2);

	orgin.Transform(box, mat);
	cout << box.Orientation.x << " " << box.Orientation.y << " " << box.Orientation.z << " " << box.Orientation.w << endl;

    return 0;
}
