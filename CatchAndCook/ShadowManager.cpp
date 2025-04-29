#include "pch.h"
#include "ShadowManager.h"

#include "Camera.h"
#include "LightManager.h"

unique_ptr<ShadowManager> ShadowManager::main = nullptr;

ShadowManager::ShadowManager()
{
}

ShadowManager::~ShadowManager()
{
}

void ShadowManager::Init()
{

}

void ShadowManager::SetShadowCasterParams()
{
	
}


std::vector<BoundingFrustum> ShadowManager::GetFrustums(Camera* camera, Light* light, const std::vector<float>& distances)
{
    camera->Calculate();
    auto params = camera->GetCameraParams();

    float n = params.cameraFrustumData.z;
    float f = params.cameraFrustumData.w;

    std::vector<float> dis;
    dis.reserve(distances.size() + 1);
    dis.push_back(n);
    dis.insert(dis.end(), distances.begin(), distances.end());


    std::vector<BoundingFrustum> frustums;
    std::vector<BoundingOrientedBox> bounds;
    frustums.reserve(dis.size());
    bounds.reserve(dis.size());

    for (int i = 1; i < dis.size(); i++)
    {
        //Vector4(_fov, _params.cameraScreenData.x / _params.cameraScreenData.y, _near, _far);
        auto mat = Matrix::CreatePerspectiveFieldOfView(
            params.cameraFrustumData.x,
            params.cameraFrustumData.y,
            dis[i - 1],
            dis[i]);
        BoundingFrustum frustum;
        BoundingFrustum::CreateFromMatrix(frustum, mat);
        frustum.Transform(frustum, params.InvertViewMatrix);
        frustums.push_back(frustum);
    }
    return frustums;
}


std::vector<BoundingOrientedBox> ShadowManager::GetBounds(Camera* camera, Light* light, const std::vector<float>& distances)
{
    camera->Calculate();
    auto params = camera->GetCameraParams();

    float n = params.cameraFrustumData.z;
    float f = params.cameraFrustumData.w;

    std::vector<float> dis;
    dis.reserve(distances.size() + 1);
	dis.push_back(n);
    dis.insert(dis.end(), distances.begin(), distances.end());

    this->_shadowCasterParams.cascadeCount = distances.size();


	std::vector<BoundingFrustum> frustums;
    std::vector<BoundingOrientedBox> bounds;
    frustums.reserve(dis.size());
    bounds.reserve(dis.size());

	for (int i = 0;i < dis.size() - 1; i++)
	{
        //Vector4(_fov, _params.cameraScreenData.x / _params.cameraScreenData.y, _near, _far);
        auto mat = Matrix::CreatePerspectiveFieldOfView(
            params.cameraFrustumData.x,
            params.cameraFrustumData.y,
            dis[i],
            dis[i + 1]);

        BoundingFrustum frustum;
        BoundingFrustum::CreateFromMatrix(frustum, mat);
        frustum.Transform(frustum, params.InvertViewMatrix);
        array<Vector3, 8> corners;
        frustum.GetCorners(corners.data());
        Vector3 lightDir = light->direction;
        lightDir.Normalize();
        Vector3 lightUp = Vector3::UnitY; // 필요하면 카메라 상향으로 조정
        Vector3 lightPos = -lightDir * 500.0f;
        Matrix lightView = Matrix::CreateLookAt(lightPos, Vector3::Zero, lightUp);
        Matrix invLightView = lightView.Invert();


        Vector3 minLS(FLT_MAX), maxLS(-FLT_MAX);
        for (auto& p : corners)
        {
            Vector3 lp = Vector3::Transform(p, lightView);
            minLS = Vector3::Min(minLS, lp);
            maxLS = Vector3::Max(maxLS, lp);
        }

        Vector3 centerLS = (minLS + maxLS) * 0.5f;
        Vector3 extentsLS = (maxLS - minLS) * 0.5f;

        //정사각형으로 맞춰주기.
		extentsLS.x = max(extentsLS.x, extentsLS.y);
        extentsLS.y = extentsLS.x;

        BoundingOrientedBox obb(centerLS, extentsLS, Quaternion::Identity);
        obb.Transform(obb, invLightView);


        Vector3 centerWS = Vector3::Transform(centerLS, invLightView);
        Vector3 lightPosWS = centerWS - lightDir * 1000;
        lightView = Matrix::CreateLookAt(lightPosWS, centerWS, lightUp);
        invLightView = lightView.Invert();

        
        auto projMat = Matrix::CreateOrthographic(extentsLS.x * 2, extentsLS.y * 2, 0.5, 2000);
        Matrix invProjMat = projMat.Invert();
        
        this->_shadowCasterParams.lightViewMatrix[i] = lightView;
		this->_shadowCasterParams.lightProjectionMatrix[i] = projMat;
        this->_shadowCasterParams.lightInvertViewMatrix[i] = invLightView;
        this->_shadowCasterParams.lightInvertProjectionMatrix[i] = invProjMat;

        this->_shadowCasterParams.lightVPMatrix[i] = lightView * projMat;
        this->_shadowCasterParams.lightInvertVPMatrix[i] = this->_shadowCasterParams.lightVPMatrix[i].Invert();


        frustums.push_back(frustum);
        bounds.push_back(obb);
	}
	return bounds;
}

void ShadowManager::SetData(Material* material)
{
    auto* cbuffer = Core::main->GetBufferManager()->GetBufferPool(BufferType::ShadowCasterParams)->Alloc(1);
    memcpy(cbuffer->ptr, &this->_shadowCasterParams, sizeof(ShadowCasterParams));
    Core::main->GetCmdList()->SetGraphicsRootConstantBufferView(8, cbuffer->GPUAdress);
    //GLOBAL_SRV_SHADOW_INDEX
}
