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
	//vec3 axis = { 0, 1, 0 };  // Y축
	//float angle = XM_PI;       // 180도

	//// 회전자체 쿼터니언(q) 구성
	//float x = 0 * sin(angle / 2);    // = 0
	//float y = 1 * sin(angle / 2);    // = 1
	//float z = 0 * sin(angle / 2);    // = 0
	//float w =		cos(angle / 2);    // = cos(π/2) = 0
	//결과 쿼터니언: (0, 1, 0, 0)
	//결과 qp(회전대상)q* 


	{
		Quaternion at = Quaternion::CreateFromAxisAngle(vec3(0, 1, 0), 30.0f * D2R);
		Quaternion bt = Quaternion::CreateFromAxisAngle(vec3(0, 1, 0), 180.0f * D2R);
		auto rad = Quaternion::Angle(at, bt); // 두개의쿼터니언 입력받아 rad 반환
		std::cout << rad * R2D << endl;
		cout << at.x << " " << at.y << " " << at.z << " " << at.w << endl;
	}

	{
		// 두개의쿼터니언 입력받아 두쿼터니언을 연결한 결과 반환
		Quaternion at = Quaternion::CreateFromAxisAngle(vec3(0, 1, 0), 30.0f * D2R);
		Quaternion bt = Quaternion::CreateFromAxisAngle(vec3(0, 1, 0), 180.0f * D2R);
		Quaternion result = Quaternion::Concatenate(at, bt);
		cout << result.x << " " << result.y << " " << result.z << " " << result.w << endl;

	}

	{
		//y축을 중심으로 90도회전.
		Quaternion a = Quaternion::CreateFromYawPitchRoll(vec3(0, 90.0f * D2R, 0));
		cout << a.x << " " << a.y << " " << a.z << " " << a.w << endl;

		vec3 rotateDir = vec3(0, 0, 1);
		rotateDir.Normalize();

		//회전된 방향벡터
		vec3 result = Vector3::Transform(rotateDir, a);
		cout << result.x << " " << result.y << " " << result.z << endl;
	}

	{
		Quaternion a = Quaternion::CreateFromYawPitchRoll(vec3(0, 90.0f * D2R, 0));
		a.Conjugate(); // 켤레쿼터니언
		cout << a.x << " " << a.y << " " << a.z << " " << a.w << endl;
	}

	{

		Quaternion result = Quaternion::FromToRotation(vec3(0, 0, 1), vec3(1, 0, 0));
		//Y축 90도회전
		cout << result.x << " " << result.y << " " << result.z << " " << result.w << endl;

		vec3 dir = vec3(4, 4, 4);
		vec3 resultDir = Vector3::Transform(dir, result);
		cout << resultDir.x << " " << resultDir.y << " " << resultDir.z << endl;

	}

	{

		Quaternion result = Quaternion::LookRotation(vec3(1, 0, 0), vec3(0, 1, 0)); // X+ 바라보게
		vec3 dir = vec3(0, 0, 1.0f); // 원래 Y축
		vec3 resultDir = Vector3::Transform(dir, result);
		cout << resultDir.x << " " << resultDir.y << " " << resultDir.z << endl;

	}

	{
	}


	return 0;
}
