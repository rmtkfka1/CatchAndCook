#include "pch.h"
#include "InGameMainField.h"


COMPONENT(InGameMainField)

InGameMainField* InGameMainField::main = nullptr;

InGameMainField::~InGameMainField()
{

}

bool InGameMainField::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void InGameMainField::Init()
{
	Component::Init();
	main = this;
}

void InGameMainField::Start()
{
	Component::Start();


	std::vector<std::shared_ptr<GameObject>> pathPointObjects;
	SceneManager::main->GetCurrentScene()->FindsInclude(L"$PathPoint", pathPointObjects);

	std::vector<std::shared_ptr<GameObject>> pathPointObjectTables;
	std::ranges::copy_if(pathPointObjects, std::back_inserter(pathPointObjectTables), [&](const std::shared_ptr<GameObject>& current) {
		return (current->GetName().find(L"Shop") != std::wstring::npos) &&
			(current->GetName().find(L"Sit") != std::wstring::npos);
		});
	for (auto point : pathPointObjectTables)
	{
		shopTablePointsPool.push_back(point);
	}
}

void InGameMainField::Update()
{
	Component::Update();

	if (Input::main->GetKeyDown(KeyCode::Num6))
	{
		shopOpen = true;
	}
	if (Input::main->GetKeyDown(KeyCode::Num7))
	{
		shopOpen = false;
	}
}

void InGameMainField::Update2()
{
	Component::Update2();
}

void InGameMainField::Enable()
{
	Component::Enable();
}

void InGameMainField::Disable()
{
	Component::Disable();
}

void InGameMainField::RenderBegin()
{
	Component::RenderBegin();
}

void InGameMainField::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void InGameMainField::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void InGameMainField::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void InGameMainField::SetDestroy()
{
	Component::SetDestroy();
}

void InGameMainField::Destroy()
{
	Component::Destroy();
}
