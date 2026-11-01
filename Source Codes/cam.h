#ifndef CAM_H
#define CAM_H

class Camera {
public:
    double posX, posY;
    double dirX, dirY;
    double planeX, planeY;
    double pitch; // Yukarý / Aþaðý bakma açýsý (Dikey kaydýrma)

    Camera();
    void Update(bool forward, bool backward, bool left, bool right, const int worldMap[512][512], double deltaTime);
    void Rotate(double rotSpeed);
    void AddPitch(double value);
};

#endif
