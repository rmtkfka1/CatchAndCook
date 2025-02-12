#pragma once

#include "BufferManager.h"
#include "IGuid.h"
#include "BufferPool.h"
#include "ICBufferInjector.h"
#include "RenderTarget.h"

class Shader;
class Texture;


struct MaterialParams
{
	void SetInt(uint8 index, int32 value) { intParams[index] = value; }
	void Setfloat(uint8 index, float value) { floatParams[index] = value; }
	void SetMatrix(int index, Matrix mat) { MatrixParams[index] = mat; }
	void SetVec4(uint8 index, vec4 vector) { vec4Params[index] = vector; }

	array<int32, 4>		intParams { 0 }; 
	array<vec4, 4>		vec4Params { vec4::One, vec4::One, vec4::One, vec4::One };
	array<float, 4>		floatParams { 0 }; 
	array<Matrix, 2>	MatrixParams { Matrix::Identity, Matrix::Identity };
};

class Material : public IGuid
{
public:
	Material() {};
	Material(bool useMaterialParams) : _useMaterialParams(useMaterialParams) {};
	virtual ~Material();

	void SetShader(std::shared_ptr<Shader> shader)
	{
		_shader = shader;
		_shaderInjectors.clear();
		for(auto& type : shader->_cbufferInjectorTypes)
			_shaderInjectors.push_back(InjectorManager::main->Get(type));
	}

	void SetPass(RENDER_PASS::PASS pass) {_pass = pass;};
	RENDER_PASS::PASS& GetPass() { return _pass; };

	void SetStencilIndex(int index) { _stencilIndex = index; };
	int GetStencilIndex() const { return _stencilIndex; };

	void SetHandle(std::string name, D3D12_CPU_DESCRIPTOR_HANDLE& handle);
	shared_ptr<Shader> GetShader() { return _shader; }

public:
	void PushData();
	void SetData();

	void SetInjector(const std::vector<std::shared_ptr<ICBufferInjector>>& injectors) { _customInjectors = injectors; }

	int GetPropertyInt(const std::string& name) { return _propertyInts[name]; };
	void SetPropertyInt(const std::string& name, int data) { _propertyInts[name] = data; };
	float GetPropertyFloat(const std::string& name) { return _propertyFloats[name]; };
	void SetPropertyFloat(const std::string& name, float data) { _propertyFloats[name] = data; };
	vec4 GetPropertyVector(const std::string& name) { return _propertyVectors[name]; };
	void SetPropertyVector(const std::string& name, const vec4& data) { _propertyVectors[name] = data; };
	Matrix GetPropertyMatrix(const std::string& name) { return _propertyMatrixs[name]; };
	void SetPropertyMatrix(const std::string& name, const Matrix& data) { _propertyMatrixs[name] = data; };

	tableContainer _tableContainer;
	tableContainer _tableLongContainer;

private:
	void PushHandle();
private:
	shared_ptr<Shader> _shader;

	std::unordered_map<std::string, int> _propertyInts; // Bool 값
	std::unordered_map<std::string, float> _propertyFloats; // 0~1 스,무스니스 정보
	std::unordered_map<std::string, vec4> _propertyVectors; // Color 
	std::unordered_map<std::string, Matrix> _propertyMatrixs; // 
	std::unordered_map<std::string, D3D12_CPU_DESCRIPTOR_HANDLE> _propertyHandle;

	CBufferContainer* _cbufferContainer;
	MaterialParams _params; // 추가 정보함수 넘겨서 데이터 넣는 셋 작업
	bool _useMaterialParams = false;
	RENDER_PASS::PASS _pass = RENDER_PASS::Forward;
	int _stencilIndex = 0;

	std::vector<std::shared_ptr<ICBufferInjector>> _customInjectors;
	std::vector<std::shared_ptr<ICBufferInjector>> _shaderInjectors;
};



//
//class TestSubMaterialParamInjector : public ICBufferInjector
//{
//public:
//	~TestSubMaterialParamInjector() override = default;
//	const char* cbufferName = "TestSubMaterialParam";
//	TestSubMaterialParam data;
//	CBufferContainer* _cbufferContainer;
//
//	void Inject(const std::any& originalSource) override
//	{
//		TestSubMaterialParam& param = data;
//		const std::shared_ptr<Material>& source = std::any_cast<const std::shared_ptr<Material>&>(originalSource);
//		param.uv = Vector2(source->GetPropertyVector("uv"));
//		_cbufferContainer = Core::main->GetBufferManager()->GetBufferPool(BufferType::MateriaSubParam)->Alloc(1);
//		memcpy(_cbufferContainer->ptr, &param, sizeof(TestSubMaterialParam));
//	}
//
//	void SetData(const std::shared_ptr<Shader>& shader) override
//	{
//		int index = GetStaticRegisterIndex();
//		if (shader) index = shader->GetRegisterIndex(cbufferName);
//		if (index != -1) Core::main->GetCmdList()->SetGraphicsRootConstantBufferView(
//			index, _cbufferContainer->GPUAdress);
//	}
//};