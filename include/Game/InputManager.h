#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <SDL2/SDL.h>

#include "Game.h"
#include "Player.h"

class InputManager {
public:
    InputManager();
    ~InputManager();
    static void processInput(SDL_Event& event, GameState& gameState, Player* player, float deltaTime);
private:
    static bool UP, RIGHT, LEFT, DOWN;
};

#endif