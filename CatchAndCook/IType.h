#pragma once
class IType
{
private:
    std::string _className;

public:
    IType();
    virtual ~IType();

    static void StaticInit();
    static void StaticRelease();

    std::string& GetTypeName();
    std::string GetTypeFullName();

	template<class T>
    static std::string GetTypeFullName()
	{
        return std::string{ typeid(T).name() };
	};

private:
    static std::unordered_map<std::string, std::function<void* ()>> _typeTable;

protected:
    template <class T, class = std::enable_if_t<std::is_base_of_v<IType, T*>>>
    static T* TypeRegister(T* obj)
    {
        auto& key = obj->GetTypeName();
        if (!_typeTable.contains(key))
            _typeTable.emplace(std::make_pair(key, []() { return dynamic_cast<IType*>(new T()); }));
        return obj;
    }

public:
    template <class T, class = std::enable_if_t<std::is_base_of_v<IType, T*>>>
    static T* CreateObject(const std::string& typeName)
    {
        auto it = _typeTable.find(typeName);
        if (it != _typeTable.end())
            return dynamic_cast<T*>(static_cast<IType*>(it->second())); // 등록된 생성자 호출 후 객체 반환
        return nullptr; // 해당 key가 없을 때 nullptr 반환
    }
};