#pragma once
//[wall] <-> [player][molgogi]
enum class GameObjectTag
{
	Defualt = 1 << 0,
	Player = 1 << 1,
	Monster = 1 << 2,
};

enum class GameObjectType
{
	Dynamic = 0, // ����(�������� ����)
	Static, // ����(ù�����ӵ���, ���� ��������)
	Deactivate //��Ȱ��ȭ(����, ������ �Ѵ� X)
};