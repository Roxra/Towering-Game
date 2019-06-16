#pragma once

#include "GameObject.h"

class Enemy : public gameObject
{
private:
	bool dirtyFlag;
public:
	Enemy(SDL_Renderer* _renderer, const char* _file, int _y, int _w, int _h);
	~Enemy();
	int tickWhenCreated;
	int currentTick = SDL_GetTicks() - tickWhenCreated;
	int lastTick;
	int dir = 2;
	bool getDF();
	bool checkEnd();
	void setDF(bool _state);
	void Move();
};

