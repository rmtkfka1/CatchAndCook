#pragma once

class IType
{

public:
    IType();
    virtual ~IType();

    std::string& GetTypeName();
    std::string GetTypeFullName();

	template<class T>
    static std::string GetTypeFullName()
	{
        return std::string{ typeid(T).name() };
	};

private:
    std::string _className;
};