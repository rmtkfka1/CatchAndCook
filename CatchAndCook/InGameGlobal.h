#pragma once

class InGameGlobal
{
public:
	static unique_ptr<InGameGlobal> main;
	void Init();
	void InitMainField();

public: // Main Field
	float skyTime = 0;


};

