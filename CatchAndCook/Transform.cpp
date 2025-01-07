#include "pch.h"
#include "Transform.h"
#include "Core.h"
#include "GameObject.h"
#include "simple_mesh_ext.h"
#include "BufferPool.h"
#include "BufferManager.h"
// SCENE -> 트리구조 / MATERIAL CAMERA MESHRENDER SCRIPTS LIGHT  / RENDERPASS / MODEL / RESOURCEMANAGER / INSTANCING
// 1.12 전까지 엔진구조완성.


void Transform::SetDestroy()
{
	Component::SetDestroy();
}

Transform::Transform()
{
}

Transform::~Transform()
{
}

void Transform::Init()
{
	Component::Init();
    std::cout << "Init\n";
}

void Transform::Start()
{
	Component::Start();
    std::cout << "Start\n";
}

void Transform::Update()
{
	Component::Update();
}

void Transform::Update2()
{
	Component::Update2();
}

void Transform::Enable()
{
	Component::Enable();
    std::cout << "Enable\n";
}

void Transform::Disable()
{
	Component::Disable();
    std::cout << "Disable\n";
}

void Transform::Destroy()
{
	Component::Destroy();
    std::cout << "Destroy\n";
}

void Transform::RenderBegin()
{
	Component::RenderBegin();
    PushData();
}

void Transform::Rendering()
{
	Component::Rendering();
}

void Transform::Collision(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::Collision(collider, other);
}

void Transform::DebugRendering()
{
	Component::DebugRendering();
}

void Transform::PushData()
{
    Matrix matrix;
    GetLocalToWorldMatrix(matrix);
    _container = Core::main->GetBufferManager()->GetBufferPool(BufferType::TransformParam)->Alloc(1);
    memcpy(_container->ptr, (void*)&matrix, sizeof(Matrix));
}

void Transform::SetData()
{
    Core::main->GetCmdList()->SetGraphicsRootConstantBufferView(0, _container->GPUAdress);
}

vec3 Transform::SetForward(const vec3& dir)
{

    _forward = dir;
    _forward.Normalize(_forward);
    _right = _up.Cross(_forward);
    _up = _forward.Cross(_right);

    SetWorldRotation(Quaternion::CreateFromRotationMatrix(Matrix(_right, _up, _forward)));
    return _forward;

};

vec3 Transform::SetUp(const vec3& dir)
{

    _up = dir;
    _up.Normalize();
    _right = _up.Cross(_forward);
    _forward = _right.Cross(_up);
    SetWorldRotation(Quaternion::CreateFromRotationMatrix(Matrix(_right, _up, _forward)));

    return _up;

};

vec3 Transform::SetRight(const vec3& dir)
{
    _right = dir;
    _right.Normalize(_right);
    _forward = _right.Cross(_up);
    _up = _forward.Cross(_right);
    SetWorldRotation(Quaternion::CreateFromRotationMatrix(Matrix(_right, _up, _forward)));
    return _right;
};

vec3 Transform::GetForward()
{
    Quaternion quat = GetWorldRotation();
    _forward = vec3::Transform(vec3(0, 0, 1), quat);
    return _forward;
}

vec3 Transform::GetUp()
{
    Quaternion quat = GetWorldRotation();
    _up = vec3::Transform(vec3(0, 1, 0), quat);
    return _up;
}

vec3 Transform::GetRight()
{
    Quaternion quat = GetWorldRotation();
    _right = vec3::Transform(vec3(1, 0, 0), quat);
    return _right;
}

vec3 Transform::GetLocalEuler()
{
    return _localRotation.ToEuler();
}

const vec3& Transform::SetLocalRotation(const vec3& euler)
{
    _localRotation = Quaternion::CreateFromYawPitchRoll(euler);
    _needLocalUpdated = true;
    return euler;
}

vec3 Transform::GetLocalPosition()
{
    return _localPosition;
}

const vec3& Transform::SetLocalPosition(const vec3& worldPos)
{
    _needLocalUpdated = true;
    return _localPosition = worldPos;
}

vec3 Transform::GetLocalScale()
{
    return _localScale;
}

const vec3& Transform::SetLocalScale(const vec3& worldScale)
{
    _needLocalUpdated = true;
    return _localScale = worldScale;
}

Quaternion Transform::GetLocalRotation()
{
    return _localRotation;
}

const Quaternion& Transform::SetLocalRotation(const Quaternion& quaternion)
{
    _needLocalUpdated = true;
    return _localRotation = quaternion;
}

vec3 Transform::GetWorldPosition()
{
    auto parent = GetOwner()->parent.lock();

    if (parent)
    {
        Matrix mat;
        parent->transform->GetLocalToWorldMatrix_BottomUp(mat);
        return vec3::Transform(_localPosition, mat);
    }

    return _localPosition;
}

const vec3& Transform::SetWorldPosition(const vec3& worldPos)
{
    auto parent = GetOwner()->parent.lock();

    if (parent)
    {
        Matrix mat;
        parent->transform->GetLocalToWorldMatrix_BottomUp(mat);
        _localPosition = vec3::Transform(worldPos, mat.Invert());
    }

    else
        _localPosition = worldPos;

    _needLocalUpdated = true;
    return worldPos;
}

vec3 Transform::GetWorldScale()
{
    vec3 totalScale = _localScale;
    auto currentObj = GetOwner()->parent.lock();
    while (currentObj != nullptr)
    {
        totalScale = totalScale * currentObj->transform->_localScale;
        currentObj = currentObj->parent.lock();
    }
    return totalScale;
}

const vec3& Transform::SetWorldScale(const vec3& scale)
{
    auto parent = GetOwner()->parent.lock();
    if (parent)
    {
        vec3 ws = parent->transform->GetWorldScale();
        _localScale = (ws / (ws * ws)) * scale;
    }
    else
        _localScale = scale;
    _needLocalUpdated = true;
    return scale;
}

Quaternion Transform::GetWorldRotation()
{
    Quaternion totalQuat = Quaternion::Identity;
    auto currentObj = GetOwner();

    while (currentObj != nullptr)
    {
        totalQuat = totalQuat * currentObj->transform->_localRotation;
        currentObj = currentObj->parent.lock();
    }

    totalQuat.Normalize();
    return totalQuat;
}

const Quaternion& Transform::SetWorldRotation(const Quaternion& quaternion)
{
    auto parent = GetOwner()->parent.lock();

    if (parent)
    {
        Quaternion result;
        parent->transform->GetWorldRotation().Inverse(result);
        _localRotation = quaternion * result;
        _localRotation.Normalize();
    }

    else
        _localRotation = quaternion;

    _needLocalUpdated = true;

    return quaternion;
}

bool Transform::GetLocalToWorldMatrix(OUT Matrix& localToWorldMatrix)
{
    if (CheckLocalToWorldMatrixUpdate())
    {
        auto root = GetOwner()->rootParent.lock();
        root->transform->TopDownLocalToWorldUpdate(Matrix::Identity);
        std::memcpy(&localToWorldMatrix, &this->_localToWorldMatrix, sizeof(Matrix));
        return _isLocalToWorldChanged;
    }

    std::memcpy(&localToWorldMatrix, &this->_localToWorldMatrix, sizeof(Matrix));
    return _isLocalToWorldChanged;
}

bool Transform::GetLocalToWorldMatrix_BottomUp(Matrix& localToWorld)
{
    if (CheckLocalToWorldMatrixUpdate())
    {
        BottomUpLocalToWorldUpdate();
        std::memcpy(&localToWorld, &_localToWorldMatrix, sizeof(Matrix));
        return _isLocalToWorldChanged;
    }

    std::memcpy(&localToWorld, &_localToWorldMatrix, sizeof(Matrix));
    return _isLocalToWorldChanged;
}

bool Transform::GetLocalSRTMatrix(Matrix& localSRT)
{
    isLocalSRTChanged = CheckLocalSRTUpdate();

    if (isLocalSRTChanged)
    {
        _needLocalUpdated = false;
        _localSRTMatrix = Matrix::CreateScale(_localScale) * Matrix::CreateFromQuaternion(_localRotation) *
            Matrix::CreateTranslation(_localPosition);
        isLocalSRTChanged = true;
    }

    isLocalSRTChanged |= _prevLocalSRTMatrix != _localSRTMatrix;
    std::memcpy(&localSRT, &_localSRTMatrix, sizeof(Matrix));
    return isLocalSRTChanged;
}

bool Transform::SetLocalSRTMatrix(Matrix& localSRT)
{
    vec3 position;
    Quaternion rotation;
    vec3 scale;
    // 행렬을 위치, 회전, 스케일로 분해
    std::memcpy(&_localSRTMatrix, &localSRT, sizeof(Matrix));
    if (localSRT.Decompose(scale, rotation, position))
    {
        _localScale = scale;
        _localRotation = rotation;
        _localPosition = position;
        _needLocalUpdated = false;
        return true;
    }
    std::cout << "분해 실패\n";
    return false;
}

bool Transform::CheckLocalSRTUpdate()
{
    return _needLocalUpdated;
}

bool Transform::CheckLocalMatrixUpdate()
{
    return CheckLocalSRTUpdate() || (_prevLocalSRTMatrix != _localSRTMatrix);
}

bool Transform::CheckLocalToWorldMatrixUpdate()
{
    //bottom-up check 방식
    bool needUpdate = false;
    auto currentObj = GetOwner();
    while (currentObj != nullptr && (!needUpdate))
    {
        needUpdate |= currentObj->transform->CheckLocalMatrixUpdate() || _needLocalToWorldUpdated;
        currentObj = currentObj->parent.lock();
    }
    return needUpdate;
}

void Transform::TopDownLocalToWorldUpdate(const Matrix& parentLocalToWorld, bool isParentUpdate)
{
    bool isLocalUpdate = GetLocalSRTMatrix(_prevLocalSRTMatrix);
    bool isFinalUpdate = isLocalUpdate || isParentUpdate || _needLocalToWorldUpdated;

    //?
    if (_isLocalToWorldChanged = isFinalUpdate)
    {
        _localToWorldMatrix = _localSRTMatrix * parentLocalToWorld;
        _needLocalToWorldUpdated = false;
    }

    auto& childs = GetOwner()->_childs;
    for (int i = 0; i < childs.size(); i++)
    {
        auto ptr = childs[i].lock();
        if (ptr != nullptr)
            ptr->transform->TopDownLocalToWorldUpdate(_localToWorldMatrix, isFinalUpdate);
    }
}

bool Transform::BottomUpLocalToWorldUpdate()
{
    if (auto parent = GetOwner()->parent.lock(); parent != nullptr)
    {
        bool parentUpdate = parent->transform->BottomUpLocalToWorldUpdate();
        bool localUpdate = GetLocalSRTMatrix(_prevLocalSRTMatrix);
        if (parentUpdate || localUpdate || _needLocalToWorldUpdated)
        {
            _localToWorldMatrix = _localSRTMatrix * parent->transform->_localToWorldMatrix;
            _needLocalToWorldUpdated = false;

            for (auto& child : GetOwner()->_childs)
            {
                auto ptr = child.lock();

                if (ptr)
                    ptr->transform->_needLocalToWorldUpdated = true;
            }

            return _isLocalToWorldChanged = true;
        }
    }

    else if (GetLocalSRTMatrix(_prevLocalSRTMatrix) || _needLocalToWorldUpdated)
    {
        _localToWorldMatrix = _localSRTMatrix;
        _needLocalToWorldUpdated = false;
        if (GetOwner())
            for (auto& child : GetOwner()->_childs)
                if (child.lock())
                    child.lock()->transform->_needLocalToWorldUpdated = true;

        return _isLocalToWorldChanged = true;
    }
    return _isLocalToWorldChanged = false;
}

void Transform::LookUp(const vec3& dir, const vec3& up)
{
    SetWorldRotation(LookToQuaternion(GetWorldPosition(), dir, up));
}

vec3 Transform::LocalToWorld_Position(const vec3& value)
{
    Matrix mat;
    GetLocalToWorldMatrix_BottomUp(mat);
    return vec3::Transform(value, mat);
}

vec3 Transform::LocalToWorld_Direction(const vec3& value)
{
    Matrix mat;
    GetLocalToWorldMatrix_BottomUp(mat);
    return vec3::TransformNormal(value, mat);
}

Quaternion Transform::LocalToWorld_Quaternion(const Quaternion& value)
{
    Quaternion result = value * GetWorldRotation();
    result.Normalize();
    return result;
}

vec3 Transform::WorldToLocal_Position(const vec3& value)
{
    Matrix mat;
    GetLocalToWorldMatrix_BottomUp(mat);
    mat.Invert(mat);
    return vec3::Transform(value, mat);
}

vec3 Transform::WorldToLocal_Direction(const vec3& value)
{
    Matrix mat;
    GetLocalToWorldMatrix_BottomUp(mat);
    mat.Invert(mat);
    return vec3::TransformNormal(value, mat);
}

Quaternion Transform::WorldToLocal_Quaternion(const Quaternion& value)
{
    Quaternion result;
    GetWorldRotation().Inverse(result);
    result = value * result;
    result.Normalize();
    return result;
}