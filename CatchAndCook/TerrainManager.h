#pragma once

class Terrain;

class TerrainManager
{
public:
	static unique_ptr<TerrainManager> main;

public:
	void PushTerrain(shared_ptr<Terrain> terrain)
	{
		_terrains.push_back(terrain);
	}

	vector<shared_ptr<Terrain>> & GetTerrains()
	{
		return _terrains;
	}
	
private:
	vector<shared_ptr<Terrain>> _terrains;

};

