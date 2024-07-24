#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class ResourceManager {
public:
    static SDL_Texture* loadTexture(SDL_Renderer* renderer, SDL_Surface*& surface);
    static SDL_Surface* loadSurface(const char* fileName);
}; 

#endif