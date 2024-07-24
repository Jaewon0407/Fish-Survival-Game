#ifndef LEVEL_H
#define LEVEL_H

#include <string>
#include "Vector2D.h"

struct Fish {
    std::string type;
    float sizeScale;
    int tier;
    int num;
    Vector2D size;
    float mass;
    const char* fileName;
};

class Level {
public:
    Level(int level, int score, std::vector<Fish> fishes);
    ~Level();
    int level;
    int score;
    std::vector<Fish> fishes;
};

#endif // LEVEL_H