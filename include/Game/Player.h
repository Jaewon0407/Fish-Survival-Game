#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"

class Player : public GameObject{
public:
    Player(std::string type, Vector2D position, float sizeScale, int tier, float mass, Direction direction, SDL_Renderer* renderer, SDL_Surface* surface);
    ~Player();
    void handleInput(Direction direction);
    void render(SDL_Renderer* renderer);
    void update(float deltaTime, int windowWidth, int windowHeight);

    void detectPlayer(GameObject* player, std::vector<GameObject*> allFish);

    int score;

private:
    Direction _direction;
};

#endif