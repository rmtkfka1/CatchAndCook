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


struct IndexEdge
{
	int u, v;
	bool operator==(IndexEdge const& o) const noexcept
	{
		return u == o.u && v == o.v;
	}
};
struct IndexEdgeHash {
	size_t operator()(IndexEdge const& e) const noexcept {
		return (size_t(e.u) << 32) ^ size_t(e.v);
	}
};
struct TriangleNode
{
	float g, f;
	int parentIndex;
	bool open, closed;
};

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



	static float Area2_3D(const Vector3& A, const Vector3& B, const Vector3& C) {
		return (B.x - A.x) * (C.z - A.z) - (B.z - A.z) * (C.x - A.x);
	}

	static bool PointInTriangle3D(
		const Vector3& P,
		const Vector3& A, const Vector3& B, const Vector3& C)
	{
		float w1 = Area2_3D(P, A, B), w2 = Area2_3D(P, B, C), w3 = Area2_3D(P, C, A);
		return (w1 >= 0 && w2 >= 0 && w3 >= 0)
			|| (w1 <= 0 && w2 <= 0 && w3 <= 0);
	}

	static float Area2_2D(const Vector2& A, const Vector2& B, const Vector2& C) {
		return (B.x - A.x) * (C.y - A.y) - (B.y - A.y) * (C.x - A.x);
	}

	static Vector2 Normalize2D(const Vector2& v) {
		float l = std::sqrt(v.x * v.x + v.y * v.y);
		return Vector2(v.x / l, v.y / l);
	}

	static bool Vec3Equal(const Vector3& a, const Vector3& b, float eps = 1e-6f) {
		return fabs(a.x - b.x) < eps
			&& fabs(a.y - b.y) < eps
			&& fabs(a.z - b.z) < eps;
	}

	static std::vector<Vector3> StringPull(
		const Vector3& start,
		const Vector3& end,
		const std::vector<std::pair<Vector3, Vector3>>& portals)
	{
		std::vector<Vector3> path;
		path.reserve(portals.size());

		Vector3 apex = start;
		Vector3 leftPt = portals[0].first;
		Vector3 rightPt = portals[0].second;
		int idxApex = 0, idxLeft = 0, idxRight = 0;

		path.push_back(apex);
		for (int i = 1; i < (int)portals.size(); ++i) {
			auto nl = portals[i].first;
			auto nr = portals[i].second;

			if (Area2_3D(apex, rightPt, nr) >= 0) {
				if (Vec3Equal(apex, rightPt)
					|| Area2_3D(apex, leftPt, nr) < 0)
				{
					rightPt = nr; idxRight = i;
				}
				else {
					apex = leftPt;
					path.push_back(apex);
					idxApex = idxLeft;
					leftPt = apex; rightPt = apex;
					idxLeft = idxApex; idxRight = idxApex;
					i = idxApex;
					continue;
				}
			}

			if (Area2_3D(apex, leftPt, nl) <= 0) {
				if (Vec3Equal(apex, leftPt)
					|| Area2_3D(apex, rightPt, nl) > 0)
				{
					leftPt = nl; idxLeft = i;
				}
				else {
					apex = rightPt;
					path.push_back(apex);
					idxApex = idxRight;
					leftPt = apex; rightPt = apex;
					idxLeft = idxApex; idxRight = idxApex;
					i = idxApex;
					continue;
				}
			}
		}
		path.push_back(end);
		return path;
	}


	std::vector<Vector3> NavMeshManager::CalculatePath_Funnel(
		const Vector3& startPos,
		const Vector3& endPos,
		const std::vector<NavMeshData>& datas,
		const std::vector<int>& _tris);


	std::vector<NavMeshData> _datas;


	bool _gizmoDebug = true;
};



