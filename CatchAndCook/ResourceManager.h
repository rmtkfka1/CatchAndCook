#pragma once

class Shader;
class Mesh;
class Texture;
class Scene;

class ResourceManager
{
public:
    static unique_ptr<ResourceManager> main;

    void Init();

    void CreateDefaultMesh();
    void CreateDefaultShader();
    void CreateDefaultMaterial();

    template<typename T, typename... Args>
    shared_ptr<T> Load(const wstring& key ,const wstring& path, Args&&... args);

    template<typename T>
    shared_ptr<T> Get(const wstring& key);

    template<typename T>
    void Add(const wstring& key, shared_ptr<T> object);

private:
    template<typename T>
    unordered_map<wstring, shared_ptr<T>>& GetResourceMap();
private:

    unordered_map<wstring, shared_ptr<Scene>> _sceneMap;
    unordered_map<wstring, shared_ptr<Shader>> _shaderMap;
    unordered_map<wstring, shared_ptr<Mesh>> _meshMap;
    unordered_map<wstring, shared_ptr<Texture>> _textureMap;

};

template<typename T, typename ...Args>
inline shared_ptr<T> ResourceManager::Load(const wstring& key, const wstring& path, Args && ...args)
{
    auto& Map = GetResourceMap<T>();

    auto it = Map.find(key);
    if (it != Map.end())
        return it->second;

    shared_ptr<T> object = make_shared<T>();
    object->Init(path, forward<Args>(args)...);
    Map[key] = object;
    return object;
}

template<typename T>
inline shared_ptr<T> ResourceManager::Get(const wstring& key)
{
    auto it = GetResourceMap<T>().find(key);
    if (it != GetResourceMap<T>().end())
        return it->second;

    return nullptr;
}

template<typename T>
inline void ResourceManager::Add(const wstring& key, shared_ptr<T> object)
{
    GetResourceMap<T>()[key] = object;
}

template<typename T>
inline unordered_map<wstring, shared_ptr<T>>& ResourceManager::GetResourceMap()
{

    if constexpr (is_same_v<T, Scene>)
    {
        return _sceneMap;
    }
    else if constexpr (is_same_v<T, Shader>)
    {
        return _shaderMap;
    }

    else if constexpr (is_same_v<T, Mesh>)
    {
        return _meshMap;
    }
    else if constexpr (is_same_v<T, Texture>)
    {
        return _textureMap;
    }

    else
    {
        throw std::invalid_argument("Unsupported resource type");
    }
}
