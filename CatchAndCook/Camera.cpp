#include "pch.h"
#include "Camera.h"

void Camera::Update()
{
    CalculateVPMatrix();
}

void Camera::CalculateVPMatrix()
{
    _params.cameraScreenData = vec4(
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0, 0);

     _params.cameraPos = vec4(_cameraPos.x, _cameraPos.y, _cameraPos.z, 1);
     _params.cameraLook = vec4(_cameraLook.x, _cameraLook.y, _cameraLook.z, 0);
     _params.cameraUp = vec4(_cameraUp.x, _cameraUp.y, _cameraUp.z, 0);

    if (_projmode == CameraProjectionMode::Perspective)
    {
        _params.cameraFrustumData = Vector4(_fov , _params.cameraScreenData.x / _params.cameraScreenData.y, _near, _far);
        _params.ProjectionMatrix = Matrix::CreatePerspectiveFieldOfView(
            _params.cameraFrustumData.x,
            _params.cameraFrustumData.y,
            _params.cameraFrustumData.z,
            _params.cameraFrustumData.w);
    }

    if (_projmode == CameraProjectionMode::Orthographic)
    {
        _params.cameraFrustumData = vec4(_orthoSize.x, _orthoSize.y, _near, _far);
        _params.ProjectionMatrix = Matrix::CreateOrthographic(
            _params.cameraFrustumData.x,
            _params.cameraFrustumData.y,
            _params.cameraFrustumData.z,
            _params.cameraFrustumData.w);
    }

    _params.ViewMatrix = XMMatrixLookToLH(
        _params.cameraPos,
        _params.cameraLook,
        _params.cameraUp);

    _params.VPMatrix = _params.ViewMatrix * _params.ProjectionMatrix;
    _params.InvertViewMatrix = _params.ViewMatrix.Invert();
    _params.InvertProjectionMatrix = _params.ProjectionMatrix.Invert();
    _params.InvertVPMatrix = _params.VPMatrix.Invert();


}

void Camera::PushData()
{
	_data = Core::main->GetBufferManager()->GetBufferPool(BufferType::CameraParam)->Alloc(1);
	memcpy(_data->ptr, (void*)&_params, sizeof(CameraParams));
}


void Camera::SetData()
{
	auto& cmdList =Core::main->GetCmdList();
	cmdList->SetGraphicsRootConstantBufferView(2, _data->GPUAdress);
}

/*************************
*                        *
*     ThirdPersonCamera  *
*                        *
**************************/

ThirdPersonCamera::ThirdPersonCamera()
{
    _type = CameraType::ThirdPersonCamera;
}

ThirdPersonCamera::~ThirdPersonCamera()
{
}

void ThirdPersonCamera::Update()
{
	Camera::Update();
}
