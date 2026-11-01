#ifndef WORLD_H
#define WORLD_H

#include <windows.h>
#include <string>

class World {
public:
    static const int MAP_W = 512;
    static const int MAP_H = 512;
    int map[MAP_H][MAP_W];

    // Parametresiz kurucu (Varsayýlan harita için)
    World();
    
    // Dosya yolunu alan kurucu (Win32 API ile .txt okumak için)
    World(const std::string& filename);
};

#endif
