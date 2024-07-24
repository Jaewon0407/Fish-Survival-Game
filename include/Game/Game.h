#ifndef GAME_H
#define GAME_H

#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "GameObject.h"
#include "Player.h"
#include "Enemy.h"
#include "Level.h"
#include "ResourceManager.h"
#include "InputManager.h"

class Game {
public:
    Game();
    ~Game();

    void initSDL();
    void initBounds(std::string type, GameObject* gameObject, Circle bounds);
    void initEntities();
    void initUI(GameState gameState);
    void idle();

    void gameLoop();
    void run();
    void terminate();
    void render();
    void update(float deltaTime);

    void loadLevel(int index);
    void nextLevel();
    void checkLevelProgression();

private:
    SDL_Window* window;
    int windowWidth, windowHeight;
    SDL_Renderer* renderer;
    SDL_Event event;

    SDL_Texture* backgroundTexture;
    SDL_Texture* textTexture;
    SDL_Texture* scoreTexture;
    SDL_Rect textRect;
    SDL_Rect scoreRect;

    TTF_Font* font;

    Player* player;
    Enemy* enemy;

    std::vector<Enemy*> enemies;
    std::vector<GameObject*> entities;
    std::vector<Level> levels;

    int currentLevelIndex;
    int totalFishCount;
    std::vector<Circle> allEnemyBounds;

    int enemySpawnRadius;

    Circle circle;
    GameState gameState;
};

std::vector<Level> initLevels();

void drawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius);
Circle setCircularBounds(std::string entityType, float entityX, float entityY, int entityW, int entityH);
int initCircularRadius(std::string entityType, int entityW, int entityH);

#endif // GAME_H