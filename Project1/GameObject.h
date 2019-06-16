#pragma once

#include <SDL.h>

class gameObject
{
protected:
	SDL_Renderer* renderer;
	SDL_Texture* image;
	SDL_Rect rect;
public:
	gameObject(SDL_Renderer* _renderer, const char* _file, int _x, int _y, int _w, int _h);
	~gameObject();
	virtual void Draw();

	int GetRectX()
	{
		return rect.x;
	}

	int GetRectY()
	{
		return rect.y;
	}

	void SetRectX(int x)
	{
		rect.x = x;
	}

	void SetRectY(int y)
	{
		rect.y = y;
	}
};