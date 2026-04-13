#pragma once

class Engine
{
private:	
	Engine();
	
	void Initialize();
public:
	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;

	static Engine& GetInstance();
	void Start();
};
