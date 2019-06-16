#include "Tower.h"

Tower::Tower(SDL_Renderer* _renderer, const char* _file, int _x, int _y, int _w, int _h) : gameObject(_renderer, _file, _x, _y, _w, _h)
{
}

Tower::~Tower()
{
}

int Tower::getCurrentTicks()
{
	return currentTicks;
}

void Tower::setCurrentTicks()
{
	currentTicks = SDL_GetTicks() + 1000; //sets cooldown timer
}

bool Tower::getCooldown()
{
	return cooldown;
}

void Tower::setCooldown(bool _cooldown)
{
	cooldown = _cooldown;
}
