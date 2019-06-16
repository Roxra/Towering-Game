#include "Enemy.h"
#include <ctime>
#include <iostream>

Enemy::Enemy(SDL_Renderer* _renderer, const char* _file, int _y, int _w, int _h) : gameObject(_renderer, _file, 960, _y, _w, _h)
{
	rect.x = rand() % (1600 - 400 + 1) + 300; //random spawn location inbetween these constraints;
	dirtyFlag = false;
	tickWhenCreated = SDL_GetTicks();
}

Enemy::~Enemy()
{
}

bool Enemy::getDF()
{
	return dirtyFlag; 
}

void Enemy::setDF(bool _state) 
{
	dirtyFlag = _state; 
}

void Enemy::Move()
{
	currentTick = SDL_GetTicks() - tickWhenCreated;

	if (currentTick > lastTick + 3000) //random direction every 3000 ticks
	{
		dir = rand() % 4;
		lastTick = currentTick;
	}

	if (rect.x > 1700)
	{
		dir = 0; //stops the enemies from being too far on the right
	}

	if (rect.x < 300)
	{
		dir = 1; //stops the enemies from being too far on the left
	}

	switch (dir){
		case 0:
			rect.x--; //moves the enemy to the left
			break;
		case 1:
			rect.x++; //moves the enemy to the right
			break;
		case 2:
			rect.y++; //moves the enemy downwards
			break;
		default:
			rect.y++;
			break;
	}
}

bool Enemy::checkEnd()
{
	if (rect.y >= 1080) //checks if the enemy has reached the bottom of the screen
	{
		return true;
	}
	else
	{
		return false;
	}
}
