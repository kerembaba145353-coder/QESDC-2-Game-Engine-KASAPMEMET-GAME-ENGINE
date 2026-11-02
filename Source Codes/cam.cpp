#include "cam.h"
#include <cmath>

Camera::Camera() {
    posX = 3.0;
    posY = 2.0;
    dirX = -1.0;
    dirY = 0.0;
    
    // --- FOV (Görüþ Açýsý) Ayarý ---
    // fovMultiplier deðerini büyüttükçe görüþ açýsý GENÝÞLER (örn: 1.2 veya 1.5 yapabilirsin)
    // Küçülttükçe DARALIR / ZOOM yapar (örn: 0.5 yapabilirsin)
    double fovMultiplier = 1.5; 
    
    planeX = 0.0;
    planeY = 0.66 * fovMultiplier; 
    
    pitch = 0.0;
}

void Camera::Rotate(double rotSpeed) {
    double oldDirX = dirX;
    dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
    dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
    double oldPlaneX = planeX;
    planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
    planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
}

void Camera::AddPitch(double value) {
    pitch += value;
    if (pitch > 200.0) pitch = 200.0;
    if (pitch < -200.0) pitch = -200.0;
}

void Camera::Update(bool forward, bool backward, bool left, bool right, const int worldMap[8][8], double deltaTime) {
    double moveSpeed = 4.0 * deltaTime; 
    double rotSpeed = 3.0 * deltaTime;

    if (forward) {
        if (worldMap[(int)posY][(int)(posX + dirX * moveSpeed)] == 0) posX += dirX * moveSpeed;
        if (worldMap[(int)(posY + dirY * moveSpeed)][(int)posX] == 0) posY += dirY * moveSpeed;
    }
    if (backward) {
        if (worldMap[(int)posY][(int)(posX - dirX * moveSpeed)] == 0) posX -= dirX * moveSpeed;
        if (worldMap[(int)(posY - dirY * moveSpeed)][(int)posX] == 0) posY -= dirY * moveSpeed;
    }
    if (right) Rotate(-rotSpeed);
    if (left)  Rotate(rotSpeed);
}
