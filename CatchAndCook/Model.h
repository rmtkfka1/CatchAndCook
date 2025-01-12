#pragma once

class ModelGameObject;

class Model
{
public:
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<std::shared_ptr<ModelGameObject>> models;
};

class ModelGameObject
{
private:
	std::vector<int> meshList;

	ModelGameObject* _parent;
	std::vector<ModelGameObject*> _childs;
	std::string _name;
public:
	Matrix localTransform = Matrix::Identity;
	void SetName(const std::string& name) { _name = name; };
	std::string& GetName() { return  _name; };

	void Init(aiNode* node);

	void SetParent(const std::shared_ptr<ModelGameObject>& object);
	void AddChilds(const std::vector<std::shared_ptr<ModelGameObject>>& object);
};