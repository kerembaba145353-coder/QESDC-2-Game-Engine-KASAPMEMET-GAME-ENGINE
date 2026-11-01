#ifndef SOFTWARE_H
#define SOFTWARE_H

#include <windows.h>
#include "cam.h"
#include "world.h"

class SoftwareEngine {
public:
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    Camera cam;
    World world;
    bool running;

    bool Initialize(HINSTANCE hInstance);
    void Run();
    void Shutdown();
};

#endif
