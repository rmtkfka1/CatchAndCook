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
	std::string _originalName;

	Matrix _localTransform = Matrix::Identity;
	bool _isDynamic = true;

public:
	void SetName(const std::string& name)
	{
		_name = name;
		_originalName = _name;
		if (name.find("$AssimpFbx$") != std::string::npos)
		{
			vector<string> originalList = str::split(name, "_$AssimpFbx$");
			_originalName = originalList[0];
		}
	};
	std::string& GetName() { return  _name; };
	std::string& GetOriginalName() { return  _originalName; };

	void SetLocalSRT(const Matrix& matrix) { _localTransform = matrix; };
	Matrix& GetLocalSRT() { return _localTransform; };

	void AddBoneIndex(int index) { _boneIndexList.push_back(index); };
	bool IsBone() const { return _boneIndexList.size() != 0; };

	bool IsMesh() const { return _meshIndexList.size() != 0; };

	void SetDynamic(bool isDynamic) { _isDynamic = isDynamic; };
	bool IsDynamic() const { return _isDynamic; };

	void Init(shared_ptr<Model> model, aiNode* node);

	void SetParent(const std::shared_ptr<ModelNode>& object);
	std::shared_ptr<ModelNode> GetParent();
	void AddChild(const std::shared_ptr<ModelNode>& object);

	std::shared_ptr<GameObject> CreateGameObject(const std::shared_ptr<Scene>& scene,
		const std::shared_ptr<GameObject>& parent);
};
