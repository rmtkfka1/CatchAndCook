
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


void Print(const Matrix& mat)
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			cout << mat.m[i][j] << " ";
		}
		cout << endl;
	}

	cout << endl;
}

int main()
{

	Matrix transMat = Matrix::CreateTranslation(1, 2, 3);

	Print(transMat);


	Matrix viewMat = Matrix::CreateLookAt(vec3(0, 0, 0), vec3(1, 0, 0), vec3(0, 1, 0));

	Print(viewMat);

}
