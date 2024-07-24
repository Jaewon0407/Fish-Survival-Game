#include <iostream>
#include "./include/Game/ResourceManager.h"

SDL_Texture* ResourceManager::loadTexture(SDL_Renderer* renderer, SDL_Surface*& surface) {   

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
    }
    // SDL_FreeSurface(surface);
    return texture;
}

SDL_Surface* ResourceManager::loadSurface(const char* fileName) {
    SDL_Surface* surface = IMG_Load(fileName);
    if (!surface) {
        std::cerr << "Failed to create surface: " << IMG_GetError() << std::endl;
    }
    return surface;
}