#pragma once


enum class GameObjectTag
{
	Defualt = 1 << 0,
	Player = 1 << 0,
	Monster = 1 << 0,
};

enum class GameObjectType
{
	Dynamic = 0, // ����(�������� ����)
	Static, // ����(ù�����ӵ���, ���� ��������)
	Deactivate //��Ȱ��ȭ(����, ������ �Ѵ� X)
};