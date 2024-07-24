#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <unordered_map>

#include "Vector2D.h"
#include "ResourceManager.h"

enum class Direction {UP, RIGHT, DOWN, LEFT, UP_RIGHT, UP_LEFT, DOWN_RIGHT, DOWN_LEFT, NONE};

enum class Status {
    ALIVE,
    DEAD,
};

struct Circle {
    float x, y;
    int r;
};

class GameObject {
public:
    GameObject(std::string type, Vector2D position, float sizeScale, int tier, float mass, SDL_Renderer* renderer, SDL_Surface* surface);
    ~GameObject();

    void applyForce(Vector2D& force);
    Vector2D calculateDrag();
    
    void wrapAround(float& entityX, float& entityY, int entityW, int entityH, int windowWidth, int windowHeight);

    Circle spawnBounds;
    Circle regularBounds;
    Circle detectionBounds;

    Vector2D size;
    Vector2D position;
    Vector2D velocity;
    Vector2D acceleration;
    std::string type;
    int tier;

protected:
    SDL_Rect srcRect;
    SDL_Rect destRect;
    SDL_Texture* texture;
    SDL_Surface* surface;

    float dragCoef;
    float maxSpeed;
    float mass;
    Status status;
};

#endif