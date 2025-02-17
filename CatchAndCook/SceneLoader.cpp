#include "pch.h"
#include "SceneLoader.h"
#include "SceneLoader.h"

#include <nlohmann/json_fwd.hpp>

#include "BufferPool.h"
#include "Collider.h"
#include "Scene.h"
#include "Component.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "ModelMesh.h"
#include "PhysicsComponent.h"
#include "SkinnedMeshRenderer.h"
#include "Terrain.h"


SceneLoader::SceneLoader()
{

}

SceneLoader::~SceneLoader()
{

}

std::vector<std::shared_ptr<GameObject>> SceneLoader::Load(const std::shared_ptr<Scene>& scene)
{
    _scene = scene;

    for (auto it = originalJson.begin(); it != originalJson.end(); ++it)
    {
        auto& key = it.key();
        auto& value = it.value();

        if (key == "GameObjects")
            for (auto& objectJson : value)
                PrevProcessingGameObject(objectJson);
        if (key == "Components")
            for (auto& objectJson : value)
                PrevProcessingComponent(objectJson);
        if (key == "Materials")
            for (auto& objectJson : value)
                PrevProcessingMaterial(objectJson);
    }

    for (auto& ref : refJson_MaterialTable)
        LinkMaterial(*ref.second);
    for (auto& ref : refJson_ComponentTable)
        LinkComponent(*ref.second);
    for (auto& ref : refJson_GameObjectTable)
        LinkGameObject(*ref.second);
    std::vector<std::shared_ptr<GameObject>> result = gameObjectCache;

    std::cout << std::format("Load - {0}", std::to_string(this->_path)) << "\n";
    std::cout << std::format("  GameObject Count - {0}", refJson_GameObjectTable.size()) << "\n";
    std::cout << std::format("  Component Count - {0}", refJson_ComponentTable.size()) << "\n";
    std::cout << std::format("  Material Count - {0}", refJson_MaterialTable.size()) << "\n";

    componentCache.clear();
    gameObjectCache.clear();
    materialCache.clear();
    _scene = nullptr;
    return result;
}


void SceneLoader::Init(const std::wstring& path)
{
    std::ifstream reader{ path };
    if (!reader)
    {
        std::cout << "Json Load Fail : Wrong Path" << "\n";
        return;
    }
    this->_path = path;
    reader >> originalJson;
    originalJson = originalJson["references"];
}

void SceneLoader::PrevProcessingGameObject(json& data)
{
	std::wstring guid = std::to_wstring(data["guid"].get<std::string>());
	auto gameObject = CreateObject<GameObject>(guid);
    gameObject->SetParent(nullptr);
	refJson_GameObjectTable[guid] = &data;
	gameObjectCache.emplace_back(gameObject);
}

void SceneLoader::PrevProcessingComponent(json& data)
{
    std::wstring guid = std::to_wstring(data["guid"].get<std::string>());
    std::wstring type = std::to_wstring(data["type"].get<std::string>());
    std::shared_ptr<Component> component;
    refJson_ComponentTable[guid] = &data;


    if (type == L"Transform")
    {
        auto transform = CreateObject<Transform>(guid);
        component = transform;
    }
    if (type == L"MeshRenderer")
    {
        auto meshRenderer = CreateObject<MeshRenderer>(guid);
        component = meshRenderer;

        auto mesh = data["mesh"];
        auto modelName = std::to_wstring(mesh["modelName"].get<std::string>());
        auto path = std::to_wstring(mesh["path"].get<std::string>());
        auto pack = ResourceManager::main->Load<Model>(modelName, path, VertexType::Vertex_Static);
    }
    if (type == L"SkinnedMeshRenderer")
    {
        auto meshRenderer = CreateObject<SkinnedMeshRenderer>(guid);
        component = meshRenderer;

        auto mesh = data["mesh"];
        auto pack = ResourceManager::main->Load<Model>(
            std::to_wstring(mesh["modelName"].get<std::string>()),
            std::to_wstring(mesh["path"].get<std::string>()), VertexType::Vertex_Skinned);
    }
    if (type == L"MeshFilter")
    {

    }
    if (type == L"BoxCollider")
    {
        auto collider = CreateObject<Collider>(guid);
        component = collider;
    }
    if(type == L"SphereCollider")
    {
        auto collider = CreateObject<Collider>(guid);
        component = collider;
    }
    if(type == L"Rigidbody")
    {
        auto pc = CreateObject<PhysicsComponent>(guid);
        component = pc;
    }
    if(type == L"Terrain")
    {
        auto terr = CreateObject<Terrain>(guid);
        component = terr;
    }

    componentCache.emplace_back(component);
}

void SceneLoader::PrevProcessingMaterial(json& data)
{
	std::wstring guid = std::to_wstring(data["guid"].get<std::string>());
	auto material = CreateObject<Material>(guid);
	refJson_MaterialTable[guid] = &data;

	for (auto& texture : data["datas"]["textures"]) {
        ResourceManager::main->Load<Texture>(
            std::to_wstring(texture["originalName"].get<std::string>()),
            std::to_wstring(texture["path"].get<std::string>()),
                TextureType::Texture2D, false
		);
	}
	materialCache.emplace_back(material);
}

void SceneLoader::LinkGameObject(json& jsonData)
{

    std::wstring guid = std::to_wstring(jsonData["guid"].get<std::string>());
    std::wstring parentGuid = std::to_wstring(jsonData["parent"].get<std::string>());

    std::shared_ptr<GameObject> gameObject = IGuid::FindObjectByGuid<GameObject>(guid);
    std::shared_ptr<GameObject> parentObject = IGuid::FindObjectByGuid<GameObject>(parentGuid);

    if (gameObject != nullptr)
    {
        auto isActive = jsonData["active"].get<bool>();
        auto isStatic = jsonData["static"].get<bool>();
        auto isDeactivate = jsonData["deactivate"].get<bool>();

        gameObject->SetName(std::to_wstring(jsonData["name"].get<std::string>()));

        for (auto& componentGuid : jsonData["components"])
            gameObject->AddComponent(
	            IGuid::FindObjectByGuid<Component>(std::to_wstring(componentGuid.get<std::string>())));
        if(isDeactivate)
            gameObject->SetType(GameObjectType::Deactivate);
        else
            gameObject->SetType(isStatic ? GameObjectType::Static : GameObjectType::Dynamic); // 수정이 필요함. Unity 쪽의 static 옵션 받아오게 해야함.
        _scene->AddGameObject(gameObject);
        gameObject->SetActiveSelf(isActive);
    	gameObject->Init();
        gameObject->SetParent(parentObject);
    }
/*return gameObject;*/
}

void SceneLoader::LinkComponent(json& jsonData)
{
    std::wstring guid = std::to_wstring(jsonData["guid"].get<std::string>());
    std::wstring type = std::to_wstring(jsonData["type"].get<std::string>());

    if (type == L"Transform")
    {
        auto transform = IGuid::FindObjectByGuid<Transform>(guid);
        auto pos = Vector3(
            jsonData["position"][0].get<float>(),
            jsonData["position"][1].get<float>(),
            jsonData["position"][2].get<float>());
        auto rotation = Quaternion(
            jsonData["rotation"][0].get<float>(),
            jsonData["rotation"][1].get<float>(),
            jsonData["rotation"][2].get<float>(),
            jsonData["rotation"][3].get<float>());
        auto scale = Vector3(
            jsonData["scale"][0].get<float>(),
            jsonData["scale"][1].get<float>(),
            jsonData["scale"][2].get<float>());
        transform->SetLocalPosition(pos);
        transform->SetLocalRotation(rotation);
        transform->SetLocalScale(scale);
    }

    if (type == L"MeshRenderer")
    {
        auto meshRenderer = IGuid::FindObjectByGuid<MeshRenderer>(guid);
        auto meshInfo = jsonData["mesh"];
        auto modelName = std::to_wstring(meshInfo["modelName"].get<std::string>());

        auto model = ResourceManager::main->Get<Model>(modelName);
        if (model != nullptr)
        {
	        std::vector<std::shared_ptr<ModelMesh>> meshes = model->FindMeshsByName(meshInfo["meshName"].get<std::string>());
            std::vector<std::shared_ptr<Material>> materials;
            for(auto& materialGuidJson : jsonData["materials"])
            {
                auto materialGuid = std::to_wstring(materialGuidJson.get<std::string>());
                auto material = IGuid::FindObjectByGuid<Material>(materialGuid);
                auto& materialData = (*refJson_MaterialTable[materialGuid]);
                auto shaderName = std::to_wstring(materialData["shaderName"].get<std::string>());
                auto shader = ResourceManager::main->Get<Shader>(shaderName);
                if (shader == nullptr)
                    shader = ResourceManager::main->Get<Shader>(L"DefaultForward");
                material->SetShader(shader);
                materials.push_back(material);
            }

            meshRenderer->AddMaterials(materials);
            for(auto& mesh : meshes)
				meshRenderer->AddMesh(mesh->GetMesh());
        }
        else
        {
            std::cout << "Json Load - Not Found Model : " << std::to_string(modelName) << "\n";
        }
    }
    if(type == L"SkinnedMeshRenderer")
    {
        auto skinnedmeshRenderer = IGuid::FindObjectByGuid<SkinnedMeshRenderer>(guid);
        auto meshInfo = jsonData["mesh"];
        auto modelName = std::to_wstring(meshInfo["modelName"].get<std::string>());
        auto boneName = std::to_wstring(jsonData["boneRoot"].get<std::string>());

        auto model = ResourceManager::main->Get<Model>(modelName);
        if(model != nullptr)
        {
            std::vector<std::shared_ptr<ModelMesh>> meshes = model->FindMeshsByName(meshInfo["meshName"].get<std::string>());
            std::vector<std::shared_ptr<Material>> materials;
            for(auto& materialGuidJson : jsonData["materials"])
            {
                auto materialGuid = std::to_wstring(materialGuidJson.get<std::string>());
                auto material = IGuid::FindObjectByGuid<Material>(materialGuid);
                auto& materialData = (*refJson_MaterialTable[materialGuid]);
                auto shaderName = std::to_wstring(materialData["shaderName"].get<std::string>());
                auto shader = ResourceManager::main->Get<Shader>(shaderName);
                if(shader == nullptr)
                    shader = ResourceManager::main->Get<Shader>(L"DefaultForward_Skinned");
                material->SetShader(shader);
                materials.push_back(material);
            }

            skinnedmeshRenderer->AddMaterials(materials);
            for(auto& mesh : meshes)
                skinnedmeshRenderer->AddMesh(mesh->GetMesh());
            skinnedmeshRenderer->SetModel(model);
            skinnedmeshRenderer->SetBoneRootName(boneName);
        } else
        {
            std::cout << "Json Load - Not Found Model : " << std::to_string(modelName) << "\n";
        }
    }
    if (type == L"BoxCollider")
    {
        auto collider = IGuid::FindObjectByGuid<Collider>(guid);
        auto center = Vector3(
            jsonData["center"][0].get<float>(),
            jsonData["center"][1].get<float>(),
            jsonData["center"][2].get<float>());
        auto size = Vector3(
            jsonData["size"][0].get<float>(),
            jsonData["size"][1].get<float>(),
            jsonData["size"][2].get<float>());
        collider->SetBoundingBox(center, size/2);
    }
    if (type == L"SphereCollider")
    {
        auto collider = IGuid::FindObjectByGuid<Collider>(guid);
        auto center = Vector3(
            jsonData["center"][0].get<float>(),
            jsonData["center"][1].get<float>(),
            jsonData["center"][2].get<float>());
        auto radius = jsonData["radius"].get<float>();
        collider->SetBoundingSphere(center, radius);
    }
    if(type == L"Rigidbody")
    {
        auto collider = IGuid::FindObjectByGuid<PhysicsComponent>(guid);

    }
    if(type == L"Terrain")
    {
        auto terrain = IGuid::FindObjectByGuid<Terrain>(guid);
        std::wstring rawPath = to_wstring(jsonData["rawPath"].get<string>());
        std::wstring pngPath = to_wstring(jsonData["pngPath"].get<string>());
        float rawSize = jsonData["heightmapResolution"].get<float>();
        auto fieldSize = Vector3(
		    jsonData["size"][0].get<float>(),
		    jsonData["size"][1].get<float>(),
		    jsonData["size"][2].get<float>());


        auto material = make_shared<Material>();
        //material->SetHandle("g_tex_0",ResourceManager::main->Load<Texture>(L"HeightMap",L"Textures/HeightMap/terrainAlbedo.png")->GetSRVCpuHandle());

        int diffuseCount = jsonData["layers"].size();
        for(int i = 0; i < diffuseCount; i++)
        {
            auto& layer = jsonData["layers"][i];
            auto diffusePath = to_wstring(layer["diffuse"]["path"].get<string>());

            auto active = Vector4(0,0,0,0);

            auto diffuse = ResourceManager::main->Load<Texture>(diffusePath, diffusePath, TextureType::Texture2D, false);
            material->SetHandle(std::format("_detailMap{0}",i), diffuse->GetSRVCpuHandle());
            if(layer.contains("normal"))
            {
                auto normalPath = to_wstring(layer["normal"]["path"].get<string>());
                auto normal = ResourceManager::main->Load<Texture>(normalPath, normalPath, TextureType::Texture2D,false);
                material->SetHandle(std::format("_normalMap{0}",i), normal->GetSRVCpuHandle());
                active.x = 1;
            }
            if(layer.contains("mask"))
            {
                auto maskPath = to_wstring(layer["mask"]["path"].get<string>());
                auto mask = ResourceManager::main->Load<Texture>(maskPath,maskPath,TextureType::Texture2D,false);
                material->SetHandle(std::format("_maskMap{0}",i),mask->GetSRVCpuHandle());
                active.y = 1;
            }
            auto tile = Vector4(
                layer["tileSize"][0].get<float>(),
                layer["tileSize"][1].get<float>(),
                layer["tileOffset"][0].get<float>(),
                layer["tileOffset"][1].get<float>());
            material->SetPropertyVector(std::format("_tileST{0}",i), tile);
            material->SetPropertyVector(std::format("_active{0}",i), active);
        }
        int blendCount = jsonData["layerBlendTextures"].size();
        for(int i = 0; i < blendCount; i++)
        {
            auto& layer = jsonData["layerBlendTextures"][i];
            auto blendPath = to_wstring(layer.get<string>());
            auto blend = ResourceManager::main->Load<Texture>(blendPath,blendPath,TextureType::Texture2D,false);
            material->SetHandle(std::format("_blendMap{0}",i),blend->GetSRVCpuHandle());
        }

        vector<shared_ptr<GameObject>> instances;
        vector<vector<Instance_Transform>> instancesDatas;

        int instanceTableCount = jsonData["instanceTable"].size();
        for(int i = 0; i < instanceTableCount; i++)
        {
            std::shared_ptr<GameObject> obj = IGuid::FindObjectByGuid<GameObject>(to_wstring(jsonData["instanceTable"][i].get<string>()));
            if(obj != nullptr) {
                instances.push_back(obj);
            }
        }

        instancesDatas.resize(instanceTableCount);
        int instanceCount = jsonData["instanceCount"].get<float>();
        for(int i = 0; i < instanceCount; i++)
        {
            auto currentInst = jsonData["instanceDatas"][i];
            int index = currentInst["index"].get<int>();
            auto pos = Vector3(
                currentInst["position"][0].get<float>(),
                currentInst["position"][1].get<float>(),
                currentInst["position"][2].get<float>());
            auto rot = Quaternion::CreateFromAxisAngle(Vector3::Up,currentInst["rotation"].get<float>());
            auto scale = Vector3(
                currentInst["scale"][0].get<float>(),
                currentInst["scale"][1].get<float>(),
                currentInst["scale"][2].get<float>());
            Matrix InvertTRS;
        	auto trs = Matrix::CreateScale(scale) *
                Matrix::CreateFromQuaternion(rot) *
                Matrix::CreateTranslation(pos);
			trs.Invert(InvertTRS);

            instancesDatas[index].push_back({trs, InvertTRS, pos});
        }

        //instanceTable
        
        material->SetPropertyInt("detailsCount",diffuseCount);
        material->SetPropertyInt("blendCount", blendCount);

        terrain->SetMaterial(material);
        terrain->SetInstances(instances);
		terrain->SetInstanceDatas(instancesDatas);
		terrain->SetHeightMap(rawPath,pngPath, Vector2(rawSize,rawSize), fieldSize);
    }
}

void SceneLoader::LinkMaterial(json& jsonData)
{
    std::wstring guid = std::to_wstring(jsonData["guid"].get<std::string>());

    auto material = IGuid::FindObjectByGuid<Material>(guid);
    auto datas = jsonData["datas"];
    auto textures = datas["textures"];
    auto floats = datas["floats"];
    auto ints = datas["ints"];
    auto vectors = datas["vectors"];
    
    for(auto& texture : textures)
    {
        auto a = std::to_wstring(texture["originalName"].get<std::string>());
        material->SetHandle(
            texture["name"].get<std::string>(),
            ResourceManager::main->Get<Texture>(std::to_wstring(texture["originalName"].get<std::string>()))->GetSRVCpuHandle());
    }
    for (auto& data : floats)
        material->SetPropertyFloat(data["name"].get<std::string>(), data["data"].get<float>());
    for (auto& data : ints)
        material->SetPropertyInt(data["name"].get<std::string>(), data["data"].get<int>());
    for (auto& data : vectors)
        material->SetPropertyVector(data["name"].get<std::string>(), Vector4(
            data["data"][0].get<float>(),
            data["data"][1].get<float>(),
            data["data"][2].get<float>(),
            data["data"][3].get<float>()
        ));
}