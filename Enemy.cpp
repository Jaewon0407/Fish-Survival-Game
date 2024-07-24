#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>
#include <random>

#include "./include/Game/Enemy.h"

Enemy::Enemy(std::string type, Vector2D position, float sizeScale, int tier, float mass, SDL_Renderer* renderer, SDL_Surface* surface) 
    : GameObject(type, position, sizeScale, tier, mass, renderer, surface), 
    state(EnemyState::IDLE), 
    timer(0),
    velocity({0.0f, 0.0f}),
    delayStarted(false),
    randomTime(0.0f),
    randomVelocityMag(0.0f),
    root(nullptr),
    predatorDetected(false),
    preyDetected(false),
    predator(nullptr),
    prey(nullptr)
{
    initMovement();
    initDecisionTree();
}

Enemy::~Enemy() {

}

void Enemy::applyForce(float thrust_strength, Vector2D& velocity, Vector2D direction) {
    float acc = thrust_strength / mass;

    Vector2D acceleration = {acc, acc};
    acceleration.x = direction.x  * acc;
    acceleration.y = direction.y * acc;

    velocity += acceleration;
}

void Enemy::initMovement() {
    float thrust_strength = 5.0f;
    // applyForce(thrust_strength, velocity.x, velocity.y);
    
    float randomAngle = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * M_PI * 2;
    Vector2D direction;
    direction.x = abs(cos(randomAngle));
    direction.y = abs(sin(randomAngle));
    direction = direction.normalize();

    applyForce(thrust_strength, velocity, direction);

    randomTime = getRandomTime();
    randomVelocityMag = getRandomVelocityMag();
}

void Enemy::initDecisionTree() {
    Action* idleAction = new Action([this](){this->idle();});
    Action* chaseAction = new Action([this](){this->chase();});
    Action* fleeAction = new Action([this](){this->flee();});
    
    Decision* preyNearbyDecision = new Decision([this](){return this->isPreyNearby();}, chaseAction, idleAction);
    Decision* predatorNearbyDecision = new Decision([this](){return this->isPredatorNearby();}, fleeAction, preyNearbyDecision);

    root = predatorNearbyDecision;
}

void Enemy::idle() {
    // Do nothing
}

void Enemy::chase() {
    Vector2D direction = prey->position - position;
    direction = direction.normalize();

    float thrust_strength = 1.0f;
    applyForce(thrust_strength, velocity, direction);
}

void Enemy::flee() {
    // Vector2D direction = position - predator->position;
    // direction = direction.normalize();

    // float thrust_strength = 2.5f;
    // applyForce(thrust_strength, velocity, direction);
}

bool Enemy::isPredatorNearby() {
    return predatorDetected;
}

bool Enemy::isPreyNearby() {
    return preyDetected;
}

void Enemy::traverseDecisionTree() {
    if (root) {
        root->evaluate();
    }
}

void Enemy::detectFishes(GameState& gameState, Enemy*& currentFish, Player*& player, int currentFishIndex, std::vector<Enemy*>& allFish) {
    
    predatorDetected = false;
    preyDetected = false;
    predator = nullptr;
    prey = nullptr;

    for (int i = 0; i < allFish.size(); i++) {

        Enemy* fish = allFish[i];

        // Make sure to not compare the fish itself
        if (&fish != &currentFish) {

            // If the fish is within the bounding range
            if (collisionDetection(currentFish->detectionBounds, player->detectionBounds)) {
                if (collisionDetection(currentFish->regularBounds, player->regularBounds)) {
                    if (currentFish->tier < player->tier) {
                        playerScoreUpdate(currentFish, player);
                        allFish.erase(allFish.begin() + currentFishIndex);
                        currentFish = nullptr;
                        return;
                    } else if (currentFish->tier > player->tier) {
                        delete player;
                        gameState = GameState::LOSE;
                        // terminate game
                        return;
                    }
                }

                if (currentFish->tier < player->tier) {
                    predatorDetected = true;
                    predator = fish;
                } else if (currentFish->tier > player->tier) {
                    preyDetected = true;
                    prey = fish;
                }
            }

            if (collisionDetection(currentFish->detectionBounds, fish->detectionBounds)) {
                if (collisionDetection(currentFish->regularBounds, fish->regularBounds)) {
                    if (currentFish->tier < fish->tier) {
                        allFish.erase(allFish.begin() + currentFishIndex);
                        delete currentFish;
                        currentFish = nullptr;
                        return;
                    } else if (currentFish->tier > fish->tier) {
                        allFish.erase(allFish.begin() + i);
                        delete fish;
                        fish = nullptr;
                        return;
                    }
                }

                if (currentFish->tier < fish->tier) {
                    predatorDetected = true;
                    predator = fish;
                } else if (currentFish->tier > fish->tier) {
                    preyDetected = true;
                    prey = fish;
                }
            }
        }
    }
}

void Enemy::updatePosition(float deltaTime, int windowWidth, int windowHeight) {

    // Traverse Decision Tree
    traverseDecisionTree();

    float currentSpeed = velocity.magnitude();
    float dragCoef = 0.0025f;
    Vector2D dragForce;
    dragForce.x = -dragCoef * velocity.x * currentSpeed;
    dragForce.y = -dragCoef * velocity.y * currentSpeed;

    velocity.x += dragForce.x;
    velocity.y += dragForce.y;

    if (velocity.magnitude() < randomVelocityMag) {
        velocity = {0.0f, 0.0f};
        
        // Check if the delay has started
        if (!delayStarted) {
            startTime = std::chrono::high_resolution_clock::now();
            delayStarted = true;
        }

        // Calculate elapsed time
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = currentTime - startTime;

        if (elapsed.count() >= randomTime) {
            initMovement();
            delayStarted = false; // Reset delay flag
        }
    }

    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;

    wrapAround(position.x, position.y, destRect.w, destRect.h, windowWidth, windowHeight);
    destRect.x = position.x;
    destRect.y = position.y;
    acceleration = {0.0f, 0.0f};
}

void Enemy::render(SDL_Renderer* renderer) {
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    SDL_RenderCopyEx(renderer, texture, nullptr, &destRect, 0, nullptr, flip);
}

bool enemiesCollisionDetection(Circle currentEnemyBounds, std::vector<Circle> allEnemyBounds) {
    for (int i = 0; i < allEnemyBounds.size(); i++) {
        float distance = sqrt(pow(currentEnemyBounds.x - allEnemyBounds[i].x, 2) + pow(currentEnemyBounds.y - allEnemyBounds[i].y, 2));
        if (distance <= ((currentEnemyBounds.r * 2)  + (allEnemyBounds[i].r * 2))) {
            return true;
        }
    }
    return false;
}

bool collisionDetection(Circle currentBounds, Circle otherBounds) {
    float distance = sqrt(pow(currentBounds.x - otherBounds.x, 2) + pow(currentBounds.y - otherBounds.y, 2));
    if (distance <= ((currentBounds.r)  + (otherBounds.r))) {
        return true;
    }
    return false;
}

float getRandomTime() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(1.0f, 2.0f);
    return dis(gen);
}

float getRandomVelocityMag() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.25f, 0.5f);
    return dis(gen);
}

void playerScoreUpdate(Enemy* eatenFish, Player*& player) {
    if (eatenFish->type == "copepod") {
        player->score += 1;
    } else if (eatenFish->type == "shrimp") {
        player->score += 3;
    } else if (eatenFish->type == "hermitCrab") {
        player->score += 8;
    }
}
