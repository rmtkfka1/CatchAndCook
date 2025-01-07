#pragma once

#include "IGuid.h"
#include "BufferPool.h"
#include "RenderTarget.h"

class Shader;
class Texture;


struct MaterialParams
{
	void SetInt(uint8 index, int32 value) { intParams[index] = value; }
	void Setfloat(uint8 index, float value) { floatParams[index] = value; }
	void SetMatrix(int index, Matrix mat) { MatrixParams[index] = mat; }
	void SetVec4(uint8 index, vec4 vector) { vec4Params[index] = vector; }

	array<int32, 4>		intParams{ 0 }; 
	array<vec4, 4>		vec4Params{ }; 
	array<float, 4>		floatParams{ 0 }; 
	array<Matrix, 2>	MatrixParams{};
};


class Material : public IGuid
{
public:
	Material() {};
	Material(bool useMaterialParams) :_useMaterialParams(useMaterialParams) {};
	virtual ~Material();

	void SetShader(std::shared_ptr<Shader> shader) { _shader = shader; }
	RENDER_PASS::PASS pass;

	void SetPass(RENDER_PASS::PASS pass) {this->pass = pass;};

	void SetTexture(std::string name, const std::shared_ptr<Texture>& field);
	shared_ptr<Shader> GetShader() { return _shader; }

public:
	void PushMaterialData();
	void PushGPUData();
	void PushTexture();

	tableContainer _container;
private:
	shared_ptr<Shader> _shader;

	std::unordered_map<std::string, int> _propertyInts; // Bool 값
	std::unordered_map<std::string, float> _propertyFloats; // 0~1 스,무스니스 정보
	std::unordered_map<std::string, vec4> _propertyVectors; // Color 
	std::unordered_map<std::string, Matrix> _propertyMatrixs; // 
	std::unordered_map<std::string, shared_ptr<Texture>> _propertyTextures;

	CBufferContainer* _cbufferContainer;
	MaterialParams _params;
	bool _useMaterialParams=true;

};

