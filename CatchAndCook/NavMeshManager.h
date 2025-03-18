#pragma once

struct NavMeshPathData;

class NavMeshData
{
public:
	NavMeshData();
	virtual ~NavMeshData();

	Vector3 position;
	int index;
	std::vector<int> adjectIndexs;
};



struct NavMeshPathData
{
	NavMeshData* data = nullptr;
	float g = 0;
	float h = 0;
	int id = 0;
	int parentId = 0;
	bool closed = false;
	bool opened = false;

	NavMeshPathData() = default;
	NavMeshPathData(const NavMeshData& data, float g = 0, float h = 0);

	float GetF() const { return g + h; };
	void SetClose() { closed = true; }
	void SetOpen() { opened = true; }
	const bool& IsClose() const { return closed; }
	const bool& IsOpen() const { return opened; }

	bool operator<(const NavMeshPathData& other) const;
	bool operator==(const NavMeshPathData& other) const;
};

struct NavMeshPathDataPtrCompare {
	bool operator()(const NavMeshPathData* lhs, const NavMeshPathData* rhs) const {
		// 내부 operator<를 사용하여 두 객체를 비교합니다.
		return (*lhs) < (*rhs);
	}
};

namespace std {
	template<>
	struct hash<NavMeshPathData> {
		std::size_t operator()(const NavMeshPathData& obj) const {
			return std::hash<int>()(obj.id);
		}
	};
}

class NavMeshManager
{
public:
	static std::unique_ptr<NavMeshManager> main;

	NavMeshManager();
	virtual ~NavMeshManager();

	void Init();
	void Release();

	std::vector<NavMeshData>& GetNavMeshData();
	void SetNavMeshData(const std::vector<NavMeshData>& data);
	static void CalculatePath(const Vector3& startPosition, const Vector3& endPosition, const std::vector<NavMeshData>& datas);

	std::vector<NavMeshData> _datas;
};



