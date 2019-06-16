#pragma once

#include "GameObject.h"

class Player : public gameObject
{
private:
	int waveNumber = 1;
	int livesLost = 0;
public:
	Player(SDL_Renderer* _renderer, const char* _file, int _w, int _h);
	~Player();
	bool spawnTower = false;
	int getWaveNumber();
	void incrementWaveNumber();
	int getLivesLost();
	void incrementLivesLost();
	void Update();
};

