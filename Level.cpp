#include <vector>

#include "./include/Game/Level.h"

Level::Level(int level, int score, std::vector<Fish> fishes) :
level(level), score(score), fishes(fishes)
{

}

Level::~Level() 
{

}