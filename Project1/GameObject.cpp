#include "GameObject.h"

gameObject::gameObject(SDL_Renderer* _renderer, const char* _file, int _x, int _y, int _w, int _h)
{
	SDL_Surface* bmpFile = SDL_LoadBMP(_file);
	image = SDL_CreateTextureFromSurface(_renderer, bmpFile);
	SDL_FreeSurface(bmpFile);

	rect.x = _x;
	rect.y = _y;
	rect.w = _w;
	rect.h = _h;

	renderer = _renderer;
}

gameObject::~gameObject()
{
	if (image)
	{
		SDL_DestroyTexture(image);
	}
}

void gameObject::Draw()
{
	if (image)
	{
		SDL_RenderCopy(renderer, image, NULL, &rect);
	}
}