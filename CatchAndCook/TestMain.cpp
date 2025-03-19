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

    BoundingBox box;
	box.Center = vec3(0, 0, 0);
    box.Extents = vec3(0.5f, 0.5f, 0.5f);
	//box.Orientation = Quaternion::CreateFromYawPitchRoll(vec3(30.0f,0,0)*D2R);

	cout << box.Center.x << " " << box.Center.y << " " << box.Center.z << endl;
	cout << box.Extents.x << " " << box.Extents.y << " " << box.Extents.z << endl;

	BoundingBox box2;
	box2.Center = vec3(0, 1.01f, 0);
	box2.Extents = vec3(0.5f, 0.5f, 0.5f);

	bool result =  box.Intersects(box2);

	cout << result << endl;






    return 0;
}
