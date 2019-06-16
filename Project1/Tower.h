#pragma once
#include "GameObject.h"

class Tower : public gameObject
{
private: 
	bool cooldown = false;
	int currentTicks;
public:
	Tower(SDL_Renderer* _renderer, const char* _file, int _x, int _y, int _w, int _h);
	~Tower();
	bool getCooldown();
	void setCooldown(bool _cooldown);
	void setCurrentTicks();
	int getCurrentTicks();

};

