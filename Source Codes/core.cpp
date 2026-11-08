#include "core.h"
#include "opengl.h"
#include "render.h"
#include <cmath>

KM_Core* KM_Core::instance = NULL;

KM_Core::KM_Core() {
    running = false;
    instance = this;
    hwnd = NULL;
    hDC = NULL;
    hRC = NULL;
    enemy1.x = 5.5;
enemy1.y = 5.5;
enemy1.health = 100;
enemy1.alive = 1;
enemy1.texture = 0;
enemy1.moveTimer = 0.0;
    
}

KM_Core::~KM_Core() {
    Shutdown();
}

bool KM_Core::Initialize(HINSTANCE hInstance) {
    WNDCLASSA wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.lpfnWndProc = KM_Core::StaticWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "KasapEngineClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClassA(&wc)) return false;

    bool fullscreen = true; 
    DWORD dwStyle = WS_POPUP | WS_VISIBLE;

    if (fullscreen) {
        DEVMODE dmScreenSettings;
        memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
        dmScreenSettings.dmSize = sizeof(dmScreenSettings);
        
        // 640x480 çözünürlüðünü ve 32-bit renk derinliðini doðrudan talep et
        dmScreenSettings.dmPelsWidth  = GetSystemMetrics(SM_CXSCREEN);
        dmScreenSettings.dmPelsHeight = GetSystemMetrics(SM_CYSCREEN);
        dmScreenSettings.dmBitsPerPel = 32;
        dmScreenSettings.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

        // Windows'un bu modu zorla kabul etmesini saðla
        if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
            // Eðer ekran kartý sürücüsü reddederse pencereli moda düþmesin, 
            // ama en azýndan hata vermemesi için uyarý býrakabiliriz.
        }
        ShowCursor(FALSE); // Ýmleci gizle
    } else {
        dwStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    }

    hwnd = CreateWindowExA(
        0, "KasapEngineClass", "KasapEngine - 2.5D Raycasting",
        dwStyle,
        0, 0, 640, 480,
        NULL, NULL, hInstance, this
    );

    if (!hwnd) return false;
    if (!InitOpenGL(hwnd, hDC, hRC)) return false;
    ResizeOpenGL(640, 480);

    running = true;
    return true;
}

void KM_Core::Shutdown() {
    ChangeDisplaySettings(NULL, 0);
    ShowCursor(TRUE);
    ShutdownOpenGL(hwnd, hDC, hRC);
    running = false;
}

void KM_Core::Run() {
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    DWORD lastTime = GetTickCount();

    while (running) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                running = false;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        DWORD currentTime = GetTickCount();
        double deltaTime = (currentTime - lastTime) / 1000.0;
        lastTime = currentTime;
        if (deltaTime > 0.1) deltaTime = 0.1;

        Update(deltaTime);
        Render();
        SwapBuffers(hDC);
    }
}

void KM_Core::Update(double deltaTime) {
    bool fwd = GetAsyncKeyState('W') & 0x8000;
    bool bwd = GetAsyncKeyState('S') & 0x8000;
    bool left = GetAsyncKeyState('A') & 0x8000;
    bool right = GetAsyncKeyState('D') & 0x8000;

    cam.Update(fwd, bwd, left, right, world.map, deltaTime);
    
    if (enemy1.alive) {
    double dx = cam.posX - enemy1.x;
    double dy = cam.posY - enemy1.y;

    double distance = std::sqrt(dx * dx + dy * dy);

    if (distance > 1.0) {
        double speed = 1.0 * deltaTime;

        double nx = enemy1.x + (dx / distance) * speed;
        double ny = enemy1.y + (dy / distance) * speed;

        int mapX = (int)nx;
        int mapY = (int)ny;

        if (mapX >= 0 && mapX < World::MAP_W &&
            mapY >= 0 && mapY < World::MAP_H &&
            world.map[mapY][mapX] == 0) {

            enemy1.x = nx;
            enemy1.y = ny;
        }
    }
}
}

void KM_Core::Render() {
    RenderFrame(cam, world);
}

LRESULT CALLBACK KM_Core::StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    KM_Core* pThis = NULL;
    if (msg == WM_NCCREATE) {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pThis = (KM_Core*)pCreate->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
    } else {
        pThis = (KM_Core*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    if (pThis) {
        return pThis->WndProc(hwnd, msg, wParam, lParam);
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK KM_Core::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    KM_Core* pThis = (KM_Core*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    if (msg == WM_KEYDOWN && wParam == VK_ESCAPE) {
        ChangeDisplaySettings(NULL, 0);
        ShowCursor(TRUE);
        PostQuitMessage(0);
        return 0;
    }

    if (pThis && msg == WM_MOUSEMOVE) {
        int mouseX = LOWORD(lParam);
        int mouseY = HIWORD(lParam);

        static int lastX = mouseX;
        static int lastY = mouseY;

        int deltaX = mouseX - lastX;
        int deltaY = mouseY - lastY;

        lastX = mouseX;
        lastY = mouseY;

        if (deltaX != 0) {
            double sens = 0.005;
            pThis->cam.Rotate(-deltaX * sens);
        }
        if (deltaY != 0) {
            double sens = 0.8;
            pThis->cam.AddPitch(deltaY * sens);
        }
    }

    switch (msg) {
        case WM_CLOSE:
            ChangeDisplaySettings(NULL, 0);
            ShowCursor(TRUE);
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
