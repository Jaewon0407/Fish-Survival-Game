#include <iostream>
#include <random>

#include "./include/Game/GameObject.h"

GameObject::GameObject(std::string type, Vector2D position, float sizeScale, int tier, float mass, SDL_Renderer* renderer, SDL_Surface* surface)
    : 
    type(type),
    position(position), 
    mass(mass), 
    dragCoef(0.05f),
    velocity(0.0f, 0.0f),
    acceleration(0.0f, 0.0f),
    status(Status::ALIVE),
    tier(tier)
{   
    texture = ResourceManager::loadTexture(renderer, surface);

    size = {static_cast<float>(surface->w * sizeScale), static_cast<float>(surface->h * sizeScale)};

    destRect = {static_cast<int>(position.x), static_cast<int>(position.y), static_cast<int>(size.x), static_cast<int>(size.y)};
}

GameObject::~GameObject() {

}

void GameObject::applyForce(Vector2D& force) {
    acceleration += force / mass;
}

Vector2D GameObject::calculateDrag() {
    float speed = velocity.magnitude();
    if (speed == 0.0f) {
        return {0.0f, 0.0f};
    }
    Vector2D drag = velocity.normalize() * -dragCoef * speed * speed;
    return drag;
}

void GameObject::wrapAround(float& entityX, float& entityY, int entityW, int entityH, int windowWidth, int windowHeight) {
    if (entityX < -entityW) {
        entityX = windowWidth;
    } else if (entityX > windowWidth) {
        entityX = -entityW;
    }
    if (entityY < -entityH) {
        entityY = windowHeight;
    } else if (entityY > windowHeight) {
        entityY = -entityH;
    }
}