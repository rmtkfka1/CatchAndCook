#include "pch.h"
#include "Model.h"

void ModelGameObject::Init(aiNode* node)
{

}

void ModelGameObject::SetParent(const std::shared_ptr<ModelGameObject>& object)
{
	_parent = object.get();
}

void ModelGameObject::AddChilds(const std::vector<std::shared_ptr<ModelGameObject>>& object)
{

}
