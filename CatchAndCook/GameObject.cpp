#include "pch.h"
#include "GameObject.h"

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
    rootParent = gameObject;
    gameObject->transform = gameObject->AddComponent<Transform>();
    SetActiveSelf(true);

}

void GameObject::Start()
{

}

void GameObject::Update()
{
}

void GameObject::Update2()
{
}

void GameObject::Enable()
{
}

void GameObject::Disable()
{
}

void GameObject::Destroy()
{
}

void GameObject::RenderBegin()
{
    for (auto& ele : _components)
    {
        ele->RenderBegin();
    }
}

void GameObject::Rendering()
{
    for (auto& ele : _components)
    {
        ele->Rendering();
    }
}

void GameObject::Collision(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void GameObject::DebugRendering()
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

    if (prevParentObj != nullptr) // step 2. 부모연결 끊고 최상단으로 올리기
    {
        prevParentObj->RemoveChild(thisObj);
        this->parent.reset(); // null
        this->rootParent = thisObj;
        this->ActiveUpdateChain(true);
    }

    if (nextParentObj != nullptr) // 
    {
        this->parent = nextParentObj;
        this->rootParent = nextParentObj->rootParent;
        this->ActiveUpdateChain(nextParentObj->_active_total);
        if (nextParentObj->IsDestroy())
            this->Destroy();
        nextParentObj->AddChild(thisObj);
    }
    if (prevRootParent.lock() != this->rootParent.lock())
    {
        SetRootParent(this->rootParent.lock());
    }
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
    return _active_total && _active_self; //이미 total에 반영되있긴 하지만 혹시 모르니 한번더
}

bool GameObject::GetActiveSelf()
{
    return _active_self;
}

bool GameObject::SetActiveSelf(bool _active)
{
    _active_self = _active;
    ActiveUpdateChain((parent.lock() ? parent.lock()->_active_total : true));
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

    for (int i = 0; i < _childs.size(); i++)
    {
        auto current = _childs[i].lock();
        if (current != nullptr && (!current->IsDestroy()))
            current->ActiveUpdateChain(this->_active_total);
    }
}