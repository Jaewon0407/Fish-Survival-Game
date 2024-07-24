#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>

#include "./include/Game/Game.h"

Game::Game() : 
windowWidth(0), 
windowHeight(0),
currentLevelIndex(0),
totalFishCount(0),
levels(initLevels()),
enemySpawnRadius(50),
gameState(GameState::MENU)
{}

Game::~Game() {
    // if (renderer) {
    //     SDL_DestroyRenderer(renderer);
    // }
    // if (window) {
    //     SDL_DestroyWindow(window);
    // }
    // IMG_Quit();
    // SDL_Quit();

    // delete player;
    // player = nullptr; // Set to nullptr after deletion

    // for (auto enemy : enemies) {
    //     delete enemy;
    // }
    // enemies.clear(); // Clear the list after deletion
}

void Game::initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Failed to initialize SDL video subsystem: " << SDL_GetError() << std::endl;
        return;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "Failed to initialize SDL_image: " << IMG_GetError() << std::endl;
        return;
    }

    if (TTF_Init() < 0) {
        std::cerr << "Failed to initialize TTF: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0) {
        SDL_Log("Could not get display mode: %s", SDL_GetError());
        SDL_Quit();
        return;
    }  

    window = SDL_CreateWindow("Fish Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, displayMode.w, displayMode.h, SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_Surface* backgroundSurface = ResourceManager::loadSurface("./assets/OceanBackground.png");
    backgroundTexture = ResourceManager::loadTexture(renderer, backgroundSurface);
    
}

void Game::initUI(GameState gameState) {
    const char* button;
    SDL_Color textColor = {255, 255, 255, 255};

    if (gameState == GameState::MENU || gameState == GameState::LOSE) {
        font = TTF_OpenFont("./assets/Arial_Rounded_Bold.ttf", 128);
        if (!font) {
            std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
            return;
        }

        if (gameState == GameState::MENU) {
            button = "Start";
        } else if (gameState == GameState::LOSE) {
            button = "Nom'ed";
        }

        SDL_Surface* textSurface = TTF_RenderText_Solid(font, button, textColor);
        if (!textSurface) {
            std::cerr << "Failed to create texture surface: " << TTF_GetError() << std::endl;
            return;
        }
        textTexture = ResourceManager::loadTexture(renderer, textSurface);

        int windowWidth, windowHeight;
        SDL_GetWindowSize(window, &windowWidth, &windowHeight);

        textRect.x = (windowWidth - textSurface->w) / 2;
        textRect.y = (windowHeight - textSurface->h) / 2;
        textRect.w = textSurface->w;
        textRect.h = textSurface->h;

    } else if (gameState == GameState::RUNNING) {
        font = TTF_OpenFont("./assets/Arial_Rounded_Bold.ttf", 48);
        if (!font) {
            std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
            return;
        }

        std::string scoreText = "Score: " + std::to_string(player->score);
        SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
        if (!scoreSurface) {
            std::cerr << "Failed to create score surface: " << TTF_GetError() << std::endl;
            return;
        }
        scoreTexture = ResourceManager::loadTexture(renderer, scoreSurface);

        scoreRect.x = (windowWidth - scoreSurface->w) / 2;
        scoreRect.y = scoreSurface->h + 5;
        scoreRect.w = scoreSurface->w;
        scoreRect.h = scoreSurface->h;
    }
}

void Game::initBounds(std::string type, GameObject* gameObject, Circle bounds) {
    gameObject->spawnBounds = bounds;
    gameObject->regularBounds = bounds;
    gameObject->detectionBounds = bounds;
    if (type == "player") {
        gameObject->spawnBounds.r *= 1.5;
        gameObject->regularBounds.r *= 0.85;
        gameObject->detectionBounds.r *= 3.0;
    } else {
        gameObject->spawnBounds.r *= 1.25;
        gameObject->regularBounds.r *= 0.75;
        gameObject->detectionBounds.r *= 2.75;
    }
}

void Game::initEntities() {

    initUI(gameState);

    // To randomly generate enemy spawnpoint
    srand(static_cast<unsigned int>(time(nullptr)));

    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    
    float playerX = windowWidth / 2.0f;
    float playerY = windowHeight / 2.0f;
    const char* playerFileName = "./assets/PlayerFish.png";
    std::string playerType = "player";
    SDL_Surface* playerSurface = ResourceManager::loadSurface(playerFileName);

    player = new Player(playerType, {playerX, playerY}, 0.125f, 3, 5.0f, Direction::NONE, renderer, playerSurface);
    Circle playerBounds = setCircularBounds(playerType, player->position.x, player->position.y, player->size.x, player->size.y);
    
    // Initialize the bounds
    initBounds(playerType, player, playerBounds);

    loadLevel(0);
    
    Circle currentEnemyBounds;
    float enemyX = 0.0f;
    float enemyY = 0.0f;
    float prevEnemyX = 0.0f;
    float prevEnemyY = 0.0f;
    float prevEnemyRadius = 0.0f;

    for (int i = 0; i < levels[currentLevelIndex].fishes.size(); i++) {

        // Make sure to reset totalFishCount after level ends
        totalFishCount = 0;
        totalFishCount += levels[currentLevelIndex].fishes[i].num;

        const char* enemyFileName = levels[currentLevelIndex].fishes[i].fileName;
        SDL_Surface* enemySurface = ResourceManager::loadSurface(enemyFileName);

        std::string enemyType = levels[currentLevelIndex].fishes[i].type;
        float enemySizeScale = levels[currentLevelIndex].fishes[i].sizeScale;
        float enemyMass = levels[currentLevelIndex].fishes[i].mass;
        int enemyTier = levels[currentLevelIndex].fishes[i].tier;

        float enemyWidth = enemySurface->w;
        float enemyHeight = enemySurface->h;

        std::cout << "enemySizeScale: " << enemySizeScale << std::endl;

        for (int j = 0; j < totalFishCount; j++) {

            bool getEnemySpawn = true;
            
            // This While loop is here to get the correct coordinates
            while (getEnemySpawn) {

                enemyX = rand() % windowWidth;
                enemyY = rand() % windowHeight;

                // Initialize circular bounds, spawn circular bounds will be slightly bigger than regular circular bounds
                currentEnemyBounds = setCircularBounds(enemyType, enemyX, enemyY, enemyWidth, enemyHeight);
                Circle temp = currentEnemyBounds;
                temp.r *= 1.25;
                // currentEnemyBounds.r *= 1.25;

                if (enemyX > enemyWidth && enemyX < windowWidth - enemyWidth && enemyY > enemyHeight && enemyY < windowHeight - enemyHeight) {
                    if (!collisionDetection(temp, player->spawnBounds)) {
                        if (allEnemyBounds.size() == 0) {
                            getEnemySpawn = false;
                        } else {
                            if (!enemiesCollisionDetection(temp, allEnemyBounds)) {
                                getEnemySpawn = false;
                            }
                        }
                    }
                }
            }

            // player = new Player(playerType, {playerX, playerY}, 0.1f, 1, 5.0f, Direction::NONE, renderer, playerSurface);
            enemy = new Enemy(enemyType, {enemyX, enemyY}, enemySizeScale, enemyTier, enemyMass, renderer, enemySurface);

            if (currentEnemyBounds.r <= 0) {
                std::cerr << "Error: Invalid enemyBounds radius." << std::endl;
                return; 
            }

            if (enemy == nullptr) {
                std::cerr << "Error: enemy is a null pointer." << std::endl;
                return; 
            }   

            // Initialize the bounds for enemies
            initBounds(enemy->type, enemy, enemy->spawnBounds);
            initBounds(enemy->type, enemy, enemy->regularBounds);
            initBounds(enemy->type, enemy, enemy->detectionBounds);

            allEnemyBounds.push_back(currentEnemyBounds);
            enemies.push_back(enemy);
        }
    }
}

void Game::gameLoop() {
    initSDL();
    run();
}

void Game::run() {
    int FPS = 60;
    int frameDelay = 1000 / FPS;

    Uint32 frameStart;
    int frameTime;
    float deltaTime = 1.0f;

    initEntities();
    while (gameState != GameState::END) {
        frameStart = SDL_GetTicks();

        if (gameState == GameState::MENU || gameState == GameState::LOSE) {
            while (SDL_PollEvent(&event) != 0) {
                if (event.type == SDL_QUIT) {
                    gameState = GameState::END;
                } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                    int x, y;
                    SDL_GetMouseState(&x, &y);
                    if (gameState == GameState::MENU) {
                        if (x >= textRect.x && x <= textRect.x + textRect.w && y >= textRect.y && y <= textRect.y + textRect.h) {
                            gameState = GameState::RUNNING;
                        }
                    } else if (gameState == GameState::LOSE) {
                        if (x >= textRect.x && x <= textRect.x + textRect.w && y >= textRect.y && y <= textRect.y + textRect.h) {
                            gameState = GameState::END;
                        }
                    }
                }
            }
        }

        if (gameState == GameState::MENU) {
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
            SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
            SDL_RenderPresent(renderer);
        } else if (gameState == GameState::RUNNING) {
            initUI(gameState);
            InputManager::processInput(event, gameState, player, deltaTime);
            update(deltaTime);
            render();
            frameTime = SDL_GetTicks() - frameStart;
            if (frameDelay > frameTime) {
                SDL_Delay(frameDelay - frameTime);
            }
        } else if (gameState == GameState::LOSE) {
            initUI(gameState);
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
            SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
            SDL_RenderPresent(renderer);
        }
    }
}

void Game::update(float deltaTime) {
    Circle playerBounds = setCircularBounds(player->type, player->position.x, player->position.y, player->size.x, player->size.y);  
    initBounds(player->type, player, playerBounds);
    player->update(deltaTime, windowWidth, windowHeight);

    for (int i = 0; i < enemies.size(); i++) {
        Enemy* enemy = enemies[i];
        enemies[i]->detectFishes(gameState, enemy, player, i, enemies);
        enemies[i]->updatePosition(deltaTime, windowWidth, windowHeight);
        Circle enemyBounds = setCircularBounds(enemies[i]->type, enemies[i]->position.x, enemies[i]->position.y, enemies[i]->size.x, enemies[i]->size.y);
        initBounds(enemies[i]->type, enemies[i], enemyBounds);
    }

    // Update the score texture
    std::string scoreText = "Score: " + std::to_string(player->score);
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText.c_str(), {255, 255, 255, 255});
    if (scoreSurface) {
        if (scoreTexture) {
            SDL_DestroyTexture(scoreTexture);
        }
        scoreTexture = ResourceManager::loadTexture(renderer, scoreSurface);
    }

    checkLevelProgression();
}

void Game::render() {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);

    for (auto& enemy : enemies) {
        enemy->render(renderer);
    }
    player->render(renderer);

    // Render the score
    if (scoreTexture) {
        SDL_RenderCopy(renderer, scoreTexture, nullptr, &scoreRect);
    }

    SDL_RenderPresent(renderer);
}


void Game::terminate() {
    gameState = GameState::END;
}

void Game::loadLevel(int index) {
    if (index < levels.size()) {
        currentLevelIndex = index;
    }
}

void Game::nextLevel() {
    if (currentLevelIndex < levels.size() - 1) {
        loadLevel(currentLevelIndex + 1);
    } else {
        // Game Completed
        gameState = GameState::WIN;  
    }
}

void Game::checkLevelProgression() {
    if (player->score >= levels[currentLevelIndex].score) {
        player->score = 0;
        nextLevel();
    }
}

std::vector<Level> initLevels() {

    std::vector<Level> levels;
    
    // All the fish's num will be initially 0
    Fish fish1 = {"copepod", 0.35f, 0, 0, {64.0f, 64.0f}, 5.0f, "./assets/Copepod.png"};
    Fish fish2 = {"shrimp", 0.75f, 2, 0, {64.0f, 64.0f}, 5.0f, "./assets/Shrimp.png"};
    Fish fish3 = {"hermitCrab", 1.5f, 4, 0, {64.0f, 64.0f}, 5.0f, "./assets/HermitCrab.png"};

    // fish1.num = 30;
    fish2.num = 30;
    fish3.num = 3;
    // fish2.num = 1;
    // fish3.num = 1;
    std::vector<Fish> level1_fishes = {fish2, fish3};

    fish1.num = 5;
    fish2.num = 10;
    fish3.num = 3;
    std::vector<Fish> level2_fishes = {fish1, fish2, fish3};

    levels.push_back(Level(1, 100, level1_fishes));
    levels.push_back(Level(2, 100, level2_fishes));

    return levels;

}

Circle setCircularBounds(std::string entityType, float entityX, float entityY, int entityW, int entityH) {
    Circle circle;
    circle.x = entityX + entityW / 2;
    circle.y = entityY + entityH / 2;
    circle.r = initCircularRadius(entityType, entityW, entityH);
    return circle;
}


int initCircularRadius(std::string entityType, int entityW, int entityH) {
    // All entities set width here but height for copepod
    int radius;
    if (entityType == "copepod") {
        radius = entityW / 3.35;
    } else {
        radius = entityW / 2.5;
    }
    return radius;
}

void drawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius) {
    int x = radius;
    int y = 0;
    int decisionOver2 = 1 - x;  // Decision criterion divided by 2

    while (y <= x) {
        // Draw the points in all eight octants
        SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
        SDL_RenderDrawPoint(renderer, centerX + y, centerY + x);
        SDL_RenderDrawPoint(renderer, centerX - x, centerY + y);
        SDL_RenderDrawPoint(renderer, centerX - y, centerY + x);
        SDL_RenderDrawPoint(renderer, centerX - x, centerY - y);
        SDL_RenderDrawPoint(renderer, centerX - y, centerY - x);
        SDL_RenderDrawPoint(renderer, centerX + x, centerY - y);
        SDL_RenderDrawPoint(renderer, centerX + y, centerY - x);
        y++;
        if (decisionOver2 <= 0) {
            decisionOver2 += 2 * y + 1;  // Change in decision criterion for y -> y+1
        } else {
            x--;
            decisionOver2 += 2 * (y - x) + 1;  // Change for y -> y+1, x -> x-1
        }
    }
}