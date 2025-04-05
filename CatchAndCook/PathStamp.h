#pragma once



class PathStamp
{
public:
	static unique_ptr<PathStamp> main;

public:

	void Init();
	void Run();

	vector<vec3> lines;
};

