#include "Player.h"
#include "Tower.h"

Player::Player(SDL_Renderer* _renderer, const char* _file, int _w, int _h) : gameObject (_renderer, _file, 960, 500, _w,  _h)
{

}


Player::~Player()
{

}

void Player::Update() 
{
	const Uint8* input = SDL_GetKeyboardState(NULL);

	if (input[SDL_SCANCODE_LEFT] && rect.x > 100) //moves the player if input is left arrow key and the player is not outside the constraints
	{
		rect.x -= 6;
	}

	if (input[SDL_SCANCODE_RIGHT] && rect.x < 1750) //moves the player if input is right arrow key and the player is not outside the constraints
	{
		rect.x += 6;
	}

	if (input[SDL_SCANCODE_UP] && rect.y > 300) //moves the player if input is up arrow key and the player is not outside the constraints
	{
		rect.y -= 6;
	}

	if (input[SDL_SCANCODE_DOWN] && rect.y < 870)//moves the player if input is down arrow key and the player is not outside the constraints
	{
		rect.y += 6;
	}

	if (input[SDL_SCANCODE_SPACE]) //sets spawnTower bool to true so the main update() function can check to see if the tower can be placed
	{
		spawnTower = true;
	}
}

int Player::getWaveNumber()
{
	return waveNumber;
}

void Player::incrementWaveNumber()
{
	waveNumber = getWaveNumber();
	waveNumber++;
}

int Player::getLivesLost()
{
	return livesLost;
}

void Player::incrementLivesLost()
{
	livesLost = getLivesLost();
	livesLost++;
}
