#pragma once

class Mesh;

class GeoMetryHelper
{

public:
	static shared_ptr<Mesh> LoadRectangleBox(const float scale = 1.0f);
	static shared_ptr<Mesh> LoadRectangleMesh(const float scale = 1.0f);
	static shared_ptr<Mesh> LoadGripMesh(float width, float height, int division_x, int division_z);
	static shared_ptr<Mesh> LoadSprtieMesh();
};

