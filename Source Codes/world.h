#ifndef WORLD_H
#define WORLD_H

class World {
public:
    static const int MAP_W = 8;
    static const int MAP_H = 8;
    int map[MAP_H][MAP_W];

    World();
};

#endif
