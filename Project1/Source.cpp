#include <iostream>
#include "SDL.h"
#include <SDL_ttf.h>
#include <ctime>
#include "GameObject.h"
#include "Player.h"
#include "Enemy.h"
#include "Tower.h"
#include <vector>
#include <Windows.h>
#include <string>

void readySDL(SDL_Window** window, SDL_Renderer** renderer);
void readyEnvironment(SDL_Window** window, SDL_Renderer** renderer);
void mainMenu(SDL_Window** window, SDL_Renderer** renderer);
void gameLoop(SDL_Window** window, SDL_Renderer** renderer);
void processInputs(SDL_Window** window, SDL_Renderer** renderer, bool &loop);
void update(SDL_Window** window, SDL_Renderer** renderer, Player* player, std::vector<Enemy*>& enemyList, std::vector<Tower*>& towerList, bool &loop);
void render(SDL_Window** window, SDL_Renderer** renderer, Player* player, std::vector<Enemy*> enemyList, std::vector<Tower*> towerList);
void addEnemy(SDL_Renderer** renderer, std::vector<Enemy*>&);
void addTower(SDL_Renderer** renderer, std::vector<Tower*>&, Player* player);
void nextWave(SDL_Renderer** renderer, std::vector<Enemy*>& enemyList, Player* player);
void closeSDL(SDL_Window** window, SDL_Renderer** renderer);

int main(int argc, char* argv[])
{
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	srand(time(NULL)); //seed for random actions needed by enemy movement
	readySDL(&window,&renderer);

	SDL_AudioSpec wavSpec;
	Uint32 wavLength;
	Uint8 *wavBuffer;
	SDL_LoadWAV("Fix The Gear.wav", &wavSpec, &wavBuffer, &wavLength); // plays music
	SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
	int success = SDL_QueueAudio(deviceId, wavBuffer, wavLength);
	SDL_PauseAudioDevice(deviceId, 0);

	mainMenu(&window, &renderer);
	closeSDL(&window,&renderer);
	return 0;
}

void gameLoop(SDL_Window** window, SDL_Renderer** renderer)
{
	bool loop = true; //loop required for the game to run
	Player* player = new Player(*renderer, "Cursor.bmp", 50, 50);
	Enemy* enemy = new Enemy(*renderer, "Enemy.bmp", 0, 30, 30);
	std::vector<Enemy*> enemyList{enemy};
	Tower* tower = new Tower(*renderer, "Tower.bmp", -110, -110, 60, 60);
	std::vector<Tower*> towerList{ tower };

	while (loop)
	{
		SDL_RenderClear(*renderer); //clears the screen
		processInputs(window, renderer, loop); //handles the quit button input (player inputs are on player class)
		update(window, renderer, player, enemyList, towerList, loop); //updates, processes, and checks various things about the game
		render(window, renderer, player, enemyList, towerList); //displays the game to the screen
	}

	mainMenu(window,renderer); //revert back to the main menu if loop is set to false
}

void processInputs(SDL_Window** window, SDL_Renderer** renderer, bool &loop)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			loop = false;
			break;
		}
		else
		{

		}
	}
}

void update(SDL_Window** window, SDL_Renderer** renderer, Player* player, std::vector<Enemy*> &enemyList, std::vector<Tower*>& towerList, bool &loop)
{
	for (std::vector<Tower*>::iterator it = towerList.begin(); it != towerList.end(); ++it)
	{
		if ((*it)->getCooldown() == true)
		{
			if ((*it)->getCurrentTicks() <= SDL_GetTicks()) //checks if cooldown can be set back to false after 1000 ticks have passed
			{
				(*it)->setCooldown(false);
			}
		}
	}

	for(std::vector<Enemy*>::iterator it = enemyList.begin(); it != enemyList.end();++it)
	{
		(*it)->Move(); //random movement for the enemy
		if ((*it)->checkEnd()) //checks if enemy has reached the bottom of the screen
		{
			player->incrementLivesLost();
			(*it)->setDF(true); //sets the enemy to be killed
			if (player->getLivesLost() >= 3) //causes the player to lose and the game loop to end if 3 enemies reach the bottom
			{
				loop = false;
			}
		}
		for (Tower* tower : towerList)
		{
			int difX = std::abs(tower->GetRectX() + 30 - (*it)->GetRectX() + 15); //difference between selected tower and selected enemy on the x axis
			int difY = std::abs(tower->GetRectY() + 30 - (*it)->GetRectY() + 15); //difference between selected tower and selected enemy on the y axis
			if (difX <= 160 && difY <= 160 && tower->getCooldown() == false) //if enemy is close to tower and tower can shoot
			{
				(*it)->setDF(true); //sets the enemy to be killed
				tower->setCooldown(true); //sets a cooldown for towers stopping them from attacking every enemy at once
				tower->setCurrentTicks();
			}
		}
	}
	for (std::vector<Enemy*>::iterator it = enemyList.begin(); it != enemyList.end();)
	{
		if (!enemyList.empty())
		{
			if ((*it)->getDF() == true) //if enemy is set to be killed in previous iterator, delete gameobject and delete it off the enemyList vector
			{
				delete *it;
				it = enemyList.erase(it);
			}
			else //if not set to be killed, check the next enemy
			{
				++it;
			}
		}
	}

	if (enemyList.empty())
	{
		nextWave(renderer, enemyList, player); //starts next wave with more enemies if all enemies are defeated
	}

	player->Update(); //player inputs
	if (player->spawnTower == true) //if player presses space bar
	{
		if(towerList.size() <= player->getWaveNumber()) //if there are less or equal number of towers as the current wave, create a tower
		{
			addTower(renderer, towerList, player);
		}
		player->spawnTower = false; //reverts back even if the player could not place the tower
	}
}

void render(SDL_Window** window, SDL_Renderer** renderer, Player* player, std::vector<Enemy*> enemyList, std::vector<Tower*> towerList)
{
	readyEnvironment(window, renderer); //creates background
	for (Enemy* enemy : enemyList)
	{
		if (enemy != NULL)
		{
			enemy->Draw(); //draw each enemy in enemy list
			for (Tower* tower : towerList)
			{
				int difX = std::abs(tower->GetRectX() + 30 - (enemy)->GetRectX() + 15); //difference between selected tower and selected enemy on the x axis
				int difY = std::abs(tower->GetRectY() + 30 - (enemy)->GetRectY() + 15); //difference between selected tower and selected enemy on the x axis
				if (difX <= 180 && difY <= 180) //higher range between tower and enemy than kill function as to make it look like the laser is preparing to kill the enemy
				{
					SDL_SetRenderDrawColor(*renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
					SDL_RenderDrawLine(*renderer, tower->GetRectX() + 30, tower->GetRectY() + 30, enemy->GetRectX() + 15, enemy->GetRectY() + 15); //draws a line from tower to enemy using an offset to make it aim from the centre
				}
			}
		}
	}
	for (Tower* tower : towerList)
	{
		tower->Draw(); //draw each enemy in tower list
	}
	player->Draw(); //draws the player

	SDL_SetRenderDrawColor(*renderer, 30, 30, 30, 255);
	SDL_Rect scoreRect;
	scoreRect.x = 0;
	scoreRect.y = 0;
	scoreRect.w = 200;
	scoreRect.h = 200;
	SDL_RenderFillRect(*renderer, &scoreRect); //create an object at the top left of the screen for the score to be placed in

	TTF_Font * font = TTF_OpenFont("arial.ttf", 25);
	SDL_Color color = { 255, 255, 255 };
	std::string waveNumber = std::to_string(player->getWaveNumber()); //converts wave number int to a string
	const char* score = waveNumber.c_str(); //converts string to a const char*
	SDL_Surface * surface = TTF_RenderText_Solid(font, score, color);
	SDL_Texture * texture = SDL_CreateTextureFromSurface(*renderer, surface);
	SDL_RenderCopy(*renderer, texture, NULL, &scoreRect); //puts score in object at top left of teh screen

	SDL_RenderPresent(*renderer); //renders the screen
	SDL_Delay(16); //delay to simulate 60fps
}

void addEnemy(SDL_Renderer** renderer, std::vector<Enemy*>& enemyList)
{
	Enemy* enemy = new Enemy(*renderer, "Enemy.bmp", 0, 30, 30);
	enemyList.push_back(enemy); //adds enemy to enemyList vector
}

void addTower(SDL_Renderer** renderer, std::vector<Tower*>& towerList, Player* player)
{
	Tower* tower = new Tower(*renderer, "Tower.bmp", player->GetRectX(), player->GetRectY(), 60, 60);
	towerList.push_back(tower);  //adds tower to towerList vector

	SDL_AudioSpec wavSpec;
	Uint32 wavLength;
	Uint8 *wavBuffer;
	SDL_LoadWAV("Place.wav", &wavSpec, &wavBuffer, &wavLength); //plays a tower placing sound
	SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
	int success = SDL_QueueAudio(deviceId, wavBuffer, wavLength);
	SDL_PauseAudioDevice(deviceId, 0);
}

void nextWave(SDL_Renderer** renderer, std::vector<Enemy*>& enemyList, Player* player)
{
	player->incrementWaveNumber();
	std::cout << player->getWaveNumber() << std::endl;

	for (int i = 1; i < player->getWaveNumber(); i++) //loops adding two enemies as many times as waveNumber which is increased each wave to create more enemies as the game progresses
	{
		addEnemy(renderer, enemyList);
		addEnemy(renderer, enemyList);
	}
}

void mainMenu(SDL_Window** window, SDL_Renderer** renderer)
{
	SDL_SetRenderDrawColor(*renderer, 30, 30, 30, 255);
	SDL_Rect backgroundRect;
	backgroundRect.x = 0;
	backgroundRect.y = 0;
	backgroundRect.w = 1920;
	backgroundRect.h = 1080;
	SDL_RenderFillRect(*renderer, &backgroundRect);

	SDL_SetRenderDrawColor(*renderer, 0, 0, 255, 255);
	SDL_Rect startRect;
	startRect.x = 800;
	startRect.y = 200;
	startRect.w = 300;
	startRect.h = 100;
	SDL_RenderFillRect(*renderer, &startRect);

	SDL_SetRenderDrawColor(*renderer, 0, 255, 0, 255);
	SDL_Rect guideRect;
	guideRect.x = 800;
	guideRect.y = 400;
	guideRect.w = 300;
	guideRect.h = 100;
	SDL_RenderFillRect(*renderer, &guideRect);

	SDL_SetRenderDrawColor(*renderer, 255, 0, 0, 255);
	SDL_Rect quitRect;
	quitRect.x = 800;
	quitRect.y = 600;
	quitRect.w = 300;
	quitRect.h = 100;
	SDL_RenderFillRect(*renderer, &quitRect);

	TTF_Font * font = TTF_OpenFont("arial.ttf", 25);
	SDL_Color color = { 255, 255, 255 };
	SDL_Surface * surface = TTF_RenderText_Solid(font, "Play Game", color);
	SDL_Texture * texture = SDL_CreateTextureFromSurface(*renderer, surface);
	SDL_RenderCopy(*renderer, texture, NULL, &startRect);

	SDL_Surface * surface1 = TTF_RenderText_Solid(font, "How To Play", color);
	SDL_Texture * texture1 = SDL_CreateTextureFromSurface(*renderer, surface1);
	SDL_RenderCopy(*renderer, texture1, NULL, &guideRect);

	SDL_Surface * surface2 = TTF_RenderText_Solid(font, "Quit Game", color);
	SDL_Texture * texture2 = SDL_CreateTextureFromSurface(*renderer, surface2);
	SDL_RenderCopy(*renderer, texture2, NULL, &quitRect);

	SDL_RenderPresent(*renderer); //renders the screen

	int mX = 0; //mouse position on x axis
	int mY = 0; //mouse position on y axis
	bool loop = true;

	while (loop)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			SDL_GetMouseState(&mX, &mY); //updates mouse position
			if (mX >= quitRect.x && mX <= quitRect.x + quitRect.w && mY >= quitRect.y && mY <= quitRect.y + quitRect.h) //if mouse in this area of the quit button
			{
				if (event.type == SDL_MOUSEBUTTONDOWN)
				{
					loop = false;
				}
			}
			if (event.type == SDL_QUIT) //if player presses quit button of window
			{
				loop = false;
			}
			if (mX >= startRect.x && mX <= startRect.x + startRect.w && mY >= startRect.y && mY <= startRect.y + startRect.h) //if mouse in this area of the play game button
			{
				if (event.type == SDL_MOUSEBUTTONDOWN)
				{
					std::cout << "Play Game" << std::endl;
					gameLoop(&*window, &*renderer);
				}
			}
			if (mX >= guideRect.x && mX <= guideRect.x + guideRect.w && mY >= guideRect.y && mY <= guideRect.y + guideRect.h) //if mouse in this area of the how to play button
			{
				if (event.type == SDL_MOUSEBUTTONDOWN) //creates 2 objects with text in explaining how to play the game at the bottom of the screen
				{
					std::cout << "How To Play" << std::endl;
					SDL_Surface * surface2 = TTF_RenderText_Solid(font, "Place Towers to defeat the Enemies. You can only place a select amount of towers with the number increasing with each wave.", color);
					SDL_Texture * texture2 = SDL_CreateTextureFromSurface(*renderer, surface2);
					int texW = 0;
					int texH = 0;
					SDL_QueryTexture(texture2, NULL, NULL, &texW, &texH);
					SDL_Rect dstrect = { 300, 850, texW, texH };
					SDL_RenderCopy(*renderer, texture2, NULL, &dstrect);
					SDL_Surface * surface3 = TTF_RenderText_Solid(font, "You cannot delete turrets and enemies will appear from the top of the screen and try to reach the bottom of the screen.", color);
					SDL_Texture * texture3 = SDL_CreateTextureFromSurface(*renderer, surface3);
					int texW1 = 0;
					int texH1 = 0;
					SDL_QueryTexture(texture3, NULL, NULL, &texW1, &texH1);
					SDL_Rect dstrect2 = { 300, 900, texW, texH };
					SDL_RenderCopy(*renderer, texture3, NULL, &dstrect2); 

					SDL_RenderPresent(*renderer);
				}
			}
		}
	}
}

void readySDL(SDL_Window** window, SDL_Renderer** renderer)
{
	*window = SDL_CreateWindow("Tower Defence SDL Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1080, 720, SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);
	TTF_Init(); //initilises text library of SDL
	SDL_Init(SDL_INIT_AUDIO);

	if (*window == nullptr)
	{
		std::cout << "Failed to create window!!! :( " << SDL_GetError();
	}

	*renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);

	if (*renderer == nullptr)
	{
		std::cout << "Could not create renderer!!! :( " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(*window);
	}
}

void closeSDL(SDL_Window** window, SDL_Renderer** renderer)
{
	SDL_DestroyWindow(*window);
	SDL_DestroyRenderer(*renderer);
	TTF_Quit();
	SDL_Quit();
}

void readyEnvironment(SDL_Window** window, SDL_Renderer** renderer)
{
	SDL_SetRenderDrawColor(*renderer, 116, 164, 242, 255);
	SDL_Rect background;
	background.x = 0;
	background.y = 0;
	background.w = 5000;
	background.h = 5000;
	SDL_RenderFillRect(*renderer, &background);

	SDL_SetRenderDrawColor(*renderer, 192, 205, 226, 255);
	background.x = 100;
	background.y = 300;
	background.w = 1700;
	background.h = 630;
	SDL_RenderFillRect(*renderer, &background);

	SDL_SetRenderDrawColor(*renderer, 192, 205, 226, 255);
	background.x = 930;
	background.y = 930;
	background.w = 100;
	background.h = 100;
	SDL_RenderFillRect(*renderer, &background);
}