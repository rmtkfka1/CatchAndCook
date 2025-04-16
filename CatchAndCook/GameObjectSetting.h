#pragma once
//[wall] <-> [player][molgogi]
enum class GameObjectTag : uint32
{
	Defualt = 1 << 0,
	Player = 1 << 1,
	Monster = 1 << 2,
	Wall = 1 << 3,
};

static unordered_map<string, GameObjectTag> GameObjectTagMap =
{
	{ "Defualt", GameObjectTag::Defualt },
	{ "Player", GameObjectTag::Player },
	{ "Monster", GameObjectTag::Monster },
	{ "Wall", GameObjectTag::Wall },
};


inline GameObjectTag& operator|=(GameObjectTag& a, GameObjectTag b) {
	a = static_cast<GameObjectTag>(
		static_cast<std::uint32_t>(a) | static_cast<std::uint32_t>(b));
	return a;
}

inline GameObjectTag operator&(GameObjectTag& a, GameObjectTag b) {
	return static_cast<GameObjectTag>(
		static_cast<uint32>(a) & static_cast<uint32>(b));
}

inline bool HasTag(GameObjectTag tag, GameObjectTag flag) {
	return (static_cast<uint32>(tag) & static_cast<uint32>(flag)) != 0;
}


enum class GameObjectType
{
	Dynamic = 0, // ����(�������� ����)
	Static, // ����(ù�����ӵ���, ���� ��������)
	Deactivate //��Ȱ��ȭ(����, ������ �Ѵ� X)
};