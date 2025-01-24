#include "pch.h"
#include "GameObject.h"
#include "Component.h"
#include "Transform.h"
#include "Game.h"

GameObject::GameObject()
{
	_components.reserve(4);
}

GameObject::GameObject(std::wstring name)
{
	this->SetName(name);
}

GameObject::~GameObject()
{
	_components.clear();
}

void GameObject::Init()
{
    std::shared_ptr<GameObject> gameObject = GetCast<GameObject>();
    parent.reset();
    rootParent = gameObject;
    gameObject->_transform = gameObject->AddComponent<Transform>();
    SetActiveSelf(true);
}

void GameObject::Start()
{
    if ((!IsDestroy()) && GetActive() && IsFirst()) 
    {
        for (auto& component : _components) 
        {
            if (component->IsFirst()) {
                component->Start();
                component->FirstOff();
            }
        }
        FirstOff();
    }
}

void GameObject::Update()
{
    if ((!IsDestroy()) && GetActive() && (!IsFirst())) {
        for (auto& component : _components) {
            if (((!component->IsDestroy()) && (!component->IsFirst())))
                component->Update();
        }
    }
}

void GameObject::Update2()
{
 
    if ((!IsDestroy()) && GetActive() && (!IsFirst())) {
        for (auto& component : _components) {
            if (((!component->IsDestroy()) && (!component->IsFirst())))
                component->Update2();
        }
    }
}


void GameObject::RenderBegin()
{
    if ((!IsDestroy()) && GetActive() && (!IsFirst())) {
        for (auto& component : _components) {
            if (((!component->IsDestroy()) && (!component->IsFirst())))
                component->RenderBegin();
        }
    }
}

//void GameObject::Rendering()
//{
//  
//}


void GameObject::Destroy()
{
	if (IsDestroy()) {
        for (int i = 0; i < _components.size(); ++i) {
            auto& component = _components[i];
            component->Destroy();
            DisconnectComponent(component);
            --i;
        }
        if (!parent.expired()) parent.lock()->RemoveChild(GetCast<GameObject>());
    }
    else
    {
        for (int i = 0; i < _components.size(); ++i) {
            auto& component = _components[i];
			if (component->IsDestroy()) {
	            component->DestroyComponentOnly();
	            DisconnectComponent(component);
	            --i;
            }
        }
    }
}

void GameObject::SetDestroy()
{
    if (!IsDestroy())
    {
        SetActiveSelf(false);
        for (auto& child : _childs)
            if (!child.expired())
                child.lock()->SetDestroy();
    }
    IDelayDestroy::SetDestroy();
}

void GameObject::Debug()
{
    std::cout << "GameObject\n";
}

void GameObject::Enable()
{
    for (auto& component : _components) {
        component->Enable();
    }

}

void GameObject::Disable()
{
    for (auto& component : _components) {
        component->Disable();
    }
}


void GameObject::Collision(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}







std::shared_ptr<GameObject> GameObject::GetChild(int index)
{
    if (index < 0 || index >= _childs.size())
        return nullptr;
    return _childs[index].lock();
}

std::shared_ptr<GameObject> GameObject::GetChildByName(const std::wstring& name)
{
    auto iter = std::find_if(_childs.begin(), _childs.end(), [&](const std::weak_ptr<GameObject> obj)
        {
            if (obj.expired())
                return false;
            return obj.lock()->GetName() == name;
        });
    if (iter == _childs.end())
        return nullptr;
    return iter->lock();
}

int GameObject::GetChildAll(OUT std::vector<std::shared_ptr<GameObject>>& vec)
{
    int count = 0;
    for (auto& child : _childs)
        if (!child.expired())
        {
            vec.push_back(child.lock());
            ++count;
        }
    return count;
}

int GameObject::GetChildsByName(const std::wstring& name, OUT std::vector<std::shared_ptr<GameObject>>& vec)
{
    int count = 0;
    for (auto& c : _childs)
    {
        auto currentObj = c.lock();
        if (currentObj != nullptr && currentObj->GetName() == name)
        {
            vec.push_back(currentObj);
            ++count;
        }
    }
    return count;
}

int GameObject::GetChildsAll(std::vector<std::shared_ptr<GameObject>>& vec)
{
    int count = 1;
    vec.push_back(GetCast<GameObject>());

    for (auto& c : _childs)
    {
        auto currentObj = c.lock();
        if (currentObj != nullptr)
            count += currentObj->GetChildsAll(vec);
        
    }
    return count;
}

int GameObject::GetChildsAllByName(const std::wstring& name, std::vector<std::shared_ptr<GameObject>>& vec)
{
    int count = 1;
    if (GetName() == name)
        vec.push_back(GetCast<GameObject>());

    for (auto& c : _childs)
    {
        auto currentObj = c.lock();
        if (currentObj != nullptr)
            count += currentObj->GetChildsAllByName(name, vec);
    }
    return count;
}





bool GameObject::AddChild(const std::shared_ptr<GameObject>& obj)
{
    if (obj == nullptr)
        return false;
    auto iter = std::find_if(_childs.begin(), _childs.end(), [&](const std::weak_ptr<GameObject>& element) {
            return element.lock() == obj;
        });
    if (iter == _childs.end())
    {
        _childs.push_back(obj);
        return true;
    }
    return false;
}

bool GameObject::RemoveChild(const std::shared_ptr<GameObject>& obj)
{
    if (obj == nullptr)
        return false;

    auto iter = std::find_if(_childs.begin(), _childs.end(), [&](const std::weak_ptr<GameObject>& element) {
            return element.lock() == obj;
        });
    if (iter != _childs.end()) {
        _childs.erase(iter);
        return true;
    }
    return false;
}

bool GameObject::RemoveAtChild(int index)
{
    if (index < 0 || index >= _childs.size())
        return false;
    _childs.erase(_childs.begin() + index);
    return true;
}

bool GameObject::IsInParent(const std::shared_ptr<GameObject>& obj)
{
    if (obj == nullptr || obj == GetCast<GameObject>())
        return false;
    auto parent = this->parent.lock();
    while (parent != nullptr)
    {
        if (parent == obj)
            return true;
        parent = parent->parent.lock();
    }
    return false;
}



bool GameObject::SetParent(const std::shared_ptr<GameObject>& nextParentObj)
{
    if (nextParentObj != nullptr && nextParentObj.get() == this)
        return false;
    if (this->IsDestroy())
        return false;

    auto thisObj = GetCast<GameObject>();
    auto prevParentObj = this->parent.lock();
    auto prevRootParent = this->rootParent;

	if (nextParentObj != nullptr && nextParentObj->IsInParent(thisObj)) // step 1.
        return false;

    if (prevParentObj != nullptr) // step 2. �θ𿬰� ���� �ֻ������ �ø���
    {
        prevParentObj->RemoveChild(thisObj);
        this->parent.reset(); // null
        this->rootParent = thisObj;
        this->ActiveUpdateChain(true);
    }

    if (nextParentObj != nullptr && !nextParentObj->IsDestroy()) // �ı��� �༮�� �θ�� ���� �Ұ���
    {
        this->parent = nextParentObj;
        this->rootParent = nextParentObj->rootParent;
        this->ActiveUpdateChain(nextParentObj->GetActive());
        nextParentObj->AddChild(thisObj);
    }
    if (prevRootParent.lock() != this->rootParent.lock()) //root ����
        SetRootParent(this->rootParent.lock());
}

void GameObject::SetRootParent(const std::shared_ptr<GameObject>& rootParent)
{
    this->rootParent = rootParent;
    for (auto& child : _childs)
        if (!child.expired())
            child.lock()->SetRootParent(rootParent);
}




bool GameObject::GetActive()
{
    return _active_total = _active_total && _active_self; //�̹� total�� �ݿ����ֱ� ������ Ȥ�� �𸣴� �ѹ���
}

bool GameObject::GetActiveSelf()
{
    return _active_self;
}

bool GameObject::SetActiveSelf(bool _active)
{
    _active_self = _active;
    ActiveUpdateChain(parent.lock() ? parent.lock()->GetActive() : true);
    return _active;
}


void GameObject::SetActivePrev(bool activeTotalPrev)
{
    _active_total_prev = activeTotalPrev;
}

bool GameObject::CheckActiveUpdated()
{
    return _active_total_prev != _active_total;
}

void GameObject::SyncActivePrev()
{
	_active_total_prev = _active_total;
}

void GameObject::ActiveUpdateChain(bool active_total)
{
    this->_active_total = active_total && _active_self;
    if (CheckActiveUpdated()) {
        for (auto& element : _components) {
            if (GetActive()) element->Enable();
            else element->Disable();
        }
        SyncActivePrev();
    }
    for (int i = 0; i < _childs.size(); i++)
    {
        auto current = _childs[i].lock();
        if (current != nullptr)
            current->ActiveUpdateChain(this->_active_total);
    }
}