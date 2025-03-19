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
	static void CalculatePath(const Vector3& startPosition, const Vector3& endPosition, const std::vector<NavMeshData>& datas, const std::vector<std
	                          ::array<Vector3, 2>>& edges);
	static float ccw(const Vector3& A, const Vector3& B, const Vector3& C)
	{
		return (B.x - A.x) * (C.z - A.z) - (B.z - A.z) * (C.x - A.x);
	}
	static bool IsIntersecting(const std::array<Vector3, 2>& edge1, const std::array<Vector3, 2>& edge2)
	{
		float ccw1 = ccw(edge1[0], edge1[1], edge2[0]);
		float ccw2 = ccw(edge1[0], edge1[1], edge2[1]);
		float ccw3 = ccw(edge2[0], edge2[1], edge1[0]);
		float ccw4 = ccw(edge2[0], edge2[1], edge1[1]);

		// 일반적인 내부 교차 체크
		if ((ccw1 * ccw2 < 0) && (ccw3 * ccw4 < 0))
			return true;

		// 필요하다면, ccw가 0인 경우(접촉)도 교차로 판단하도록 수정할 수 있습니다.
		// 예를 들어, 동일한게 2개 이상이면 교차로 보고 싶다면 별도의 조건을 추가하세요.
		return false;
	}
	static bool IsPointOnSegment(const Vector3& A, const Vector3& B, const Vector3& P)
	{
		const float EPSILON = 1e-6f;
		// P가 A, B와 공선인지 확인
		if (std::fabs(ccw(A, B, P)) > EPSILON)
			return false;
		// P가 A와 B 사이에 있는지 확인 (x, z 좌표 각각에 대해)
		if (P.x < std::min(A.x, B.x) - EPSILON || P.x > std::max(A.x, B.x) + EPSILON)
			return false;
		if (P.z < std::min(A.z, B.z) - EPSILON || P.z > std::max(A.z, B.z) + EPSILON)
			return false;
		return true;
	}

	static bool HasLineOfSight(const Vector3& start, const Vector3& end,
		const std::vector<std::array<Vector3, 2>>& edges)
	{
		std::array<Vector3, 2> lineSegment = { start, end };

		for (const auto& edge : edges)
		{
			// (1) 일반적인 선분 교차 검사
			if (IsIntersecting(lineSegment, edge))
				return false;

			// (2) 두 점 모두 해당 외각 엣지 위에 있다면, 교차한 것으로 판단합니다.
			if (IsPointOnSegment(edge[0], edge[1], start) &&
				IsPointOnSegment(edge[0], edge[1], end))
			{
				return false;
			}
		}
		return true;
	}
	std::vector<NavMeshData> _datas;
};



