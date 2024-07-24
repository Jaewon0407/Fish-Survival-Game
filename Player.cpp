#include <iostream>
#include <cmath>

#include "./include/Game/Player.h"

Player::Player(std::string type, Vector2D position, float sizeScale, int tier, float mass, Direction direction, SDL_Renderer* renderer, SDL_Surface* surface) 
    : GameObject(type, position, sizeScale, tier, mass, renderer, surface), _direction(direction), score(50) {} 

Player::~Player() {

}

void Player::handleInput(Direction direction) {
    float thrustStrength = 1.0f;
    Vector2D force;
    switch (direction) {
        case Direction::RIGHT: 
            _direction = Direction::RIGHT;
            force = {thrustStrength, 0.0f};
            break;
        case Direction::LEFT: 
            _direction = Direction::LEFT;
            force = {-thrustStrength, 0.0f};
            break;
        case Direction::UP: 
            _direction = Direction::UP;
            force = {0.0f, -thrustStrength};
            break;
        case Direction::DOWN: 
            _direction = Direction::DOWN;
            force = {0.0f, thrustStrength};
            break;
        case Direction::UP_RIGHT:
            _direction = Direction::UP_RIGHT;
            force = {thrustStrength, -thrustStrength};
            break;
        case Direction::UP_LEFT:
            _direction = Direction::UP_LEFT;
            force = {-thrustStrength, -thrustStrength};
            break;
        case Direction::DOWN_RIGHT:
            _direction = Direction::DOWN_RIGHT;
            force = {thrustStrength, thrustStrength};
            break;
        case Direction::DOWN_LEFT:
            _direction = Direction::DOWN_LEFT;
            force = {-thrustStrength, thrustStrength};
            break;
        case Direction::NONE:
            break;
    }
    applyForce(force);
}

void Player::render(SDL_Renderer* renderer) {

    SDL_RendererFlip flip = SDL_FLIP_NONE;

    if (_direction == Direction::RIGHT || _direction == Direction::DOWN_RIGHT || _direction == Direction::UP_RIGHT) {
        flip = SDL_FLIP_HORIZONTAL;
    } 

    SDL_RenderCopyEx(renderer, texture, nullptr, &destRect, 0, nullptr, flip);
}

void Player::update(float deltaTime, int windowWidth, int windowHeight) {
    Vector2D drag = calculateDrag();
    applyForce(drag);
    velocity += acceleration * deltaTime;

    // Set Velocity to 0 after reaching threshold when velocity decreases from drag
    if (velocity.dotProduct(drag) < 0 && velocity.magnitude() < 0.3) {
        velocity = {0.0f, 0.0f};
    }

    position += velocity * deltaTime;
    wrapAround(position.x, position.y, destRect.w, destRect.h, windowWidth, windowHeight);
    
    destRect.x = position.x;
    destRect.y = position.y;

    acceleration = {0.0f, 0.0f};
}

// void Player::detectPlayer(GameObject* player, std::vector<GameObject*> allFish) {
    
//     for (int i = 0; i < allFish.size(); i++) {

//         GameObject* fish = allFish[i];

//         // If the fish is within the bounding range
//             if (collisionDetection(currentFish->regularBounds, fish->regularBounds)) {
//                 if (currentFish->tier < fish->tier) {
//                     allFish.erase(allFish.begin() + currentFishIndex);
//                     delete currentFish;
//                     return;
//                 } else if (currentFish->tier > fish->tier) {
//                     allFish.erase(allFish.begin() + i);
//                     delete fish;
//                     return;
//                 }
//             }
//         }
//     }
// }
