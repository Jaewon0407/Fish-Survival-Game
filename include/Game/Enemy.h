#ifndef ENEMY_H
#define ENEMY_H

#include <chrono>
#include <string>
#include <functional>

#include "Player.h"
#include "GameObject.h"
#include "Decision.h"
#include "Vector2D.h"

enum class GameState {
    RUNNING,
    MENU,
    PAUSE,
    LOSE,
    WIN,
    END
};

enum class EnemyState {
    IDLE,
    CHASE,
    RUN,
};

class Enemy : public GameObject {
public:
    Enemy(std::string type, Vector2D position, float sizeScale, int tier, float mass, SDL_Renderer* renderer, SDL_Surface* surface);
    ~Enemy();
    void render(SDL_Renderer* renderer);
    void traverseDecisionTree(); 
    void initDecisionTree();

    void idle(); 
    void chase(); // Need collision detection
    void flee();
    bool isPredatorNearby();
    bool isPreyNearby();

    void initMovement();
    void applyForce(float thrust_strength, Vector2D& velocity, Vector2D direction);
    void updatePosition(float deltaTime, int windowWidth, int windowHeight);

    void detectFishes(GameState& gameState, Enemy*& currentFish, Player*& player, int currentFishIndex, std::vector<Enemy*>& allFish);

private:
    EnemyState state;
    Vector2D force;
    Vector2D velocity;
    
    float timer;

    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    bool delayStarted;

    float randomTime;
    float randomVelocityMag;

    DecisionNode* root;

    bool predatorDetected;
    bool preyDetected;

    GameObject* predator;
    GameObject* prey;
};

float getRandomTime();
float getRandomVelocityMag();

bool enemiesCollisionDetection(Circle currentEnemyBounds, std::vector<Circle> allEnemyBounds);
bool collisionDetection(Circle currentBounds, Circle otherBounds);

void playerScoreUpdate(Enemy* eatenFish, Player*& player);

#endif