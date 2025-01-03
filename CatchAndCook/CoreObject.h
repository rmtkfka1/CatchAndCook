#pragma once
#include "IType.h"


class EObject : public std::enable_shared_from_this<EObject>, public IType
{
public:
    static std::unordered_map<std::wstring, std::weak_ptr<EObject>> _EObjectTable;
	static void StaticInit();
    static void StaticRelease();

protected:
    std::wstring guid;

public:
    EObject();
    ~EObject() override;
    EObject(const std::wstring& guid);
    EObject(const EObject& eObject);
    EObject(EObject&& eObject) noexcept;
    EObject& operator=(const EObject& eObject);
    EObject& operator=(EObject&& eObject) noexcept;
    bool operator==(const EObject& other) const;
    bool operator<(const EObject& other) const;

    void SetGUID(const std::wstring& str);
    std::wstring& GetGUID();

    template <class T, class = std::enable_if_t<std::is_base_of_v<EObject, T>>>
    std::shared_ptr<T> CoreInit()
    {
        AddObject(this->shared_from_this());
        auto& obj = GetCast<T>();
        return obj;
    }

    template <class T, class = std::enable_if_t<std::is_base_of_v<EObject, T>>>
    std::shared_ptr<T> GetCast()
    {
        return std::dynamic_pointer_cast<T>(this->shared_from_this());
    }


public:
    template <class T, class = std::enable_if_t<std::is_convertible_v<T*, EObject*>>>
    static bool AddObject(std::shared_ptr<T> object)
    {
        if (object == nullptr)
            return false;
        if (!_EObjectTable.contains(object->guid))
        {
            _EObjectTable.insert(std::make_pair(object->guid,
                std::weak_ptr<EObject>{
                    std::dynamic_pointer_cast<EObject>(object)
                }));
            return true;
        }
        return false;
    }

    template <class T, class = std::enable_if_t<std::is_convertible_v<T*, EObject*>>>
    static bool RemoveObject(std::shared_ptr<T> object)
    {
        if (object == nullptr)
            return false;
        if (_EObjectTable.contains(object->guid))
        {
            _EObjectTable.erase(object->guid);
            return true;
        }
        return false;
    }

    static bool RemoveGuid(const std::wstring& guid)
    {
        auto iter = _EObjectTable.find(guid);
        if (iter != _EObjectTable.end()) {
            _EObjectTable.erase(iter);
            return true;
        }
        return false;
    }

    static bool ContainsByGuid(const std::wstring& guid)
    {
        if (_EObjectTable.contains(guid))
        {
            if (_EObjectTable[guid].expired())
            {
                _EObjectTable.erase(guid);
                return false;
            }

            return true;
        }

        return false;
    }

    template <class T, typename = typename std::enable_if<std::is_convertible<T*, EObject*>::value>::type>
    static std::shared_ptr<T> FindObjectByType()
    {
        for (auto it = _EObjectTable.begin(); it != _EObjectTable.end();)
        {
            auto& current = *it;
            if (!current.second.expired())
            {
                auto ptr = std::dynamic_pointer_cast<T>(current.second.lock());
                if (ptr != nullptr)
                    return ptr;
                ++it;
            }
            else
                it = _EObjectTable.erase(it);
        }
        return nullptr;
    }

    template <class T, typename = typename std::enable_if<std::is_convertible<T*, EObject*>::value>::type>
    static bool FindObjectsByType(std::vector<std::shared_ptr<T>>& vec)
    {
        bool tf = false;
        for (auto it = _EObjectTable.begin(); it != _EObjectTable.end();)
        {
            auto& current = *it;
            if (!current.second.expired())
            {
                auto ptr = std::dynamic_pointer_cast<T>(current.second.lock());
                if (ptr != nullptr)
                {
                    vec.push_back(ptr);
                    tf = true;
                }
                ++it;
            }
            else
                it = _EObjectTable.erase(it);
        }
        return tf;
    }

    template <class T, typename = typename std::enable_if<std::is_convertible<T*, EObject*>::value>::type>
    static std::shared_ptr<T> FindObjectByGuid(const std::wstring& guid)
    {
        if (_EObjectTable.contains(guid))
        {
            auto& current = _EObjectTable[guid];
            if (!current.expired()) {
                auto ptr = std::dynamic_pointer_cast<T>(current.lock());
                return ptr;
            }
            _EObjectTable.erase(guid);
        }
        return nullptr;
    }

};
