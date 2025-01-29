#pragma once

class ModelNode : public std::enable_shared_from_this<ModelNode>
{
private:
	std::vector<int> _meshIndexList;
	std::vector<int> _boneIndexList;

	std::weak_ptr<Model> _model;
	std::weak_ptr<ModelNode> _parent;
	std::vector<std::weak_ptr<ModelNode>> _childs;
	std::string _name;

	Matrix _localTransform = Matrix::Identity;
	bool _isDynamic = true;

public:
	void SetName(const std::string& name) { _name = name; };
	std::string& GetName() { return  _name; };

	void SetLocalSRT(const Matrix& matrix) { _localTransform = matrix; };
	Matrix& GetLocalSRT() { return _localTransform; };

	void AddBoneIndex(int index) { _boneIndexList.push_back(index); };
	bool IsBone() const { return _boneIndexList.size() != 0; };

	void SetDynamic(bool isDynamic) { _isDynamic = isDynamic; };
	bool IsDynamic() const { return _isDynamic; };

	void Init(shared_ptr<Model> model, aiNode* node);

	void SetParent(const std::shared_ptr<ModelNode>& object);
	void AddChild(const std::shared_ptr<ModelNode>& object);

	std::shared_ptr<GameObject> CreateGameObject(const std::shared_ptr<Scene>& scene,
		const std::shared_ptr<GameObject>& parent);
};
