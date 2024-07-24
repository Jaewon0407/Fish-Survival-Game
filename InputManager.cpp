#include <iostream>

#include "./include/Game/InputManager.h"

bool InputManager::UP = false;
bool InputManager::RIGHT = false;
bool InputManager::LEFT = false;
bool InputManager::DOWN = false;

void InputManager::processInput(SDL_Event& event, GameState& gameState, Player* player, float deltaTime) {
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                gameState = GameState::END;
                break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym) {
                    case SDLK_UP:
                        UP = true;
                        break;
                    case SDLK_DOWN:
                        DOWN = true;
                        break;
                    case SDLK_RIGHT:
                        RIGHT = true;
                        break;
                    case SDLK_LEFT:
                        LEFT = true;
                        break;
                }
                break;
            case SDL_KEYUP:
                switch(event.key.keysym.sym) {
                    case SDLK_UP:
                        UP = false;
                        break;
                    case SDLK_DOWN:
                        DOWN = false;
                        break;
                    case SDLK_RIGHT:
                        RIGHT = false;
                        break;
                    case SDLK_LEFT:
                        LEFT = false;
                        break;
                }
                break;
        }
    }

    Direction direction;
    if (UP) {
        if (RIGHT) {
            direction = Direction::UP_RIGHT;
        } else if (LEFT) {
            direction = Direction::UP_LEFT;
        } else if (DOWN) {
            return;
        } else {
            direction = Direction::UP;
        }
    } else if (DOWN) {
        if (RIGHT) {
            direction = Direction::DOWN_RIGHT;
        } else if (LEFT) {
            direction = Direction::DOWN_LEFT;
        } else if (UP) {
            return;
        } else {
            direction = Direction::DOWN;
        }
    } else if (RIGHT) {
        if (LEFT) {
            return;
        } else {
            direction = Direction::RIGHT;
        }
    } else if (LEFT) {
        if (RIGHT) {
            return;
        } else {
            direction = Direction::LEFT;
        }
    } else {
        direction = Direction::NONE;
    }
    player->handleInput(direction);
}