#pragma once
//[wall] <-> [player][molgogi]
enum class GameObjectTag : uint64
{
	Defualt = 1ull << 0,
	Player = 1ull << 1,
	Monster = 1ull << 2,
	Wall = 1ull << 3,

	NonCulling = 1ull << 25,
	NonInstancing = 1ull << 26,
};

static unordered_map<string, GameObjectTag> GameObjectTagMap =
{
	{ "Defualt", GameObjectTag::Defualt },
	{ "Player", GameObjectTag::Player },
	{ "Monster", GameObjectTag::Monster },
	{ "Wall", GameObjectTag::Wall },
	{ "NonCulling", GameObjectTag::NonCulling },
	{ "NonInstancing", GameObjectTag::NonInstancing },
};


inline GameObjectTag& operator|=(GameObjectTag& a, GameObjectTag b) {
	a = static_cast<GameObjectTag>(
		static_cast<std::uint64_t>(a) | static_cast<std::uint64_t>(b));
	return a;
}

inline GameObjectTag operator&(GameObjectTag& a, GameObjectTag b) {
	return static_cast<GameObjectTag>(
		static_cast<uint64>(a) & static_cast<uint64>(b));
}

inline bool HasTag(GameObjectTag tag, GameObjectTag flag) {
	return (static_cast<uint64>(tag) & static_cast<uint64>(flag)) != 0;
}


enum class GameObjectType
{
	Dynamic = 0, // ����(�������� ����)
	Static, // ����(ù�����ӵ���, ���� ��������)
	Deactivate //��Ȱ��ȭ(����, ������ �Ѵ� X)
};