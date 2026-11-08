#ifndef ENEMY_H
#define ENEMY_H

struct Enemy {
    double x;
    double y;

    int health;
    int alive;

    unsigned int texture;

    double moveTimer;
};

#endif
