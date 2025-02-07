#pragma once

#include <any>

#include "BufferManager.h"
#include "Core.h"
#include "Shader.h"

class Material;

//이름을 가지고 매칭하는 Dynamic CBuffer들에 대해서 CBuffer를 주입하는 기능.
class ICBufferInjector
{
private:
    int _staticIndex = -1;
public:

    virtual ~ICBufferInjector() = default;
    virtual void Inject(const std::any& source) = 0;
    virtual void SetData(const std::shared_ptr<Shader>& shader) = 0;
    //Shader 없이 상수 위치에 넣어주기 위한 설정

    void SetStaticRegisterIndex(int index) {_staticIndex = index; }
    int GetStaticRegisterIndex() { return _staticIndex; }
};

//인젝터를 생성하는 헬퍼 디파인
#define CBUFFER_INJECTOR(CBUFFER_NAME, STRUCT_TYPE, STRUCT_COUNT, CBUFFER_TYPE, SOURCE_TYPE, CODE) \
class STRUCT_TYPE##Injector : public ICBufferInjector { \
public: \
	~STRUCT_TYPE##Injector() override = default;\
	const char* cbufferName = CBUFFER_NAME;\
    STRUCT_TYPE data;\
    CBufferContainer* _cbufferContainer; \
    void Inject(const std::any& originalSource) override { \
		STRUCT_TYPE& param = data; \
        const SOURCE_TYPE& source = std::any_cast<const SOURCE_TYPE&>(originalSource); \
        CODE \
        _cbufferContainer = Core::main->GetBufferManager()->CreateAndGetBufferPool(CBUFFER_TYPE, sizeof(STRUCT_TYPE), STRUCT_COUNT)->Alloc(1);\
        memcpy(_cbufferContainer->ptr, &param, sizeof(STRUCT_TYPE));\
    } \
	void SetData(const std::shared_ptr<Shader>& shader) override { \
		int index = GetStaticRegisterIndex();\
        if(shader)\
			index = shader->GetRegisterIndex(cbufferName); \
        if (index != -1) \
			Core::main->GetCmdList()->SetGraphicsRootConstantBufferView(index, _cbufferContainer->GPUAdress);\
	}\
};\

//사용 예제
/*
	CBUFFER_INJECTOR("UserName", TestSubMaterialParam, BufferType::TransformParam, std::shared_ptr<Material> source,
	    source->GetShader();
	)


        const SOURCE_TYPE& source;\
        if(originalSource.a.has_value()) \
            source = const_cast<SOURCE_TYPE&>(std::any_cast<const SOURCE_TYPE&>(originalSource)); \
*/

// 인젝터들을 모아두는 함수
class InjectorManager
{
public:
    static std::unique_ptr<InjectorManager> main;

    std::unordered_map<BufferType, std::shared_ptr<ICBufferInjector>> _injectTable;

    void Init() { _injectTable.reserve(256); }

    template <class T, class = std::enable_if_t<std::is_base_of_v<ICBufferInjector, T>>>
    std::shared_ptr<ICBufferInjector>& Get(BufferType type)
    {
        if(_injectTable.contains(type))
            return _injectTable[type];
        std::shared_ptr<ICBufferInjector> inject = static_pointer_cast<ICBufferInjector>(std::make_shared<T>());
        _injectTable[type] = inject;
        return _injectTable[type];
    };
};