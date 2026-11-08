#ifndef CORE_H
#define CORE_H

#include <windows.h>
#include "cam.h"
#include "world.h"
#include "enemy.h"

const int SCREEN_WIDTH = GetSystemMetrics(SM_CXSCREEN);
const int SCREEN_HEIGHT = GetSystemMetrics(SM_CYSCREEN);

class KM_Core {
public:
    bool running;
    static KM_Core* instance;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    
    Camera cam;
    World world;
    Enemy enemy1;

    KM_Core();
    ~KM_Core();

    bool Initialize(HINSTANCE hInstance);
    void Shutdown();
    void Run();
    void Update(double deltaTime);
    void Render();

    // Win32 Pencere Mesaj Yöneticisi
    static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif
