#include "Software.h"
#include "opengl.h"
#include "render.h"
#include "core.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CLOSE:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

bool SoftwareEngine::Initialize(HINSTANCE hInstance) {
    WNDCLASSA wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "KasapEngineClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClassA(&wc)) return false;

    hwnd = CreateWindowExA(
        0, "KasapEngineClass", "KasapEngineMemet engine 79.32",
        WS_POPUP | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, SCREEN_WIDTH, SCREEN_HEIGHT,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd) return false;
    if (!InitOpenGL(hwnd, hDC, hRC)) return false;
    ResizeOpenGL(SCREEN_WIDTH, SCREEN_HEIGHT);

    running = true;
    return true;
}

void SoftwareEngine::Run() {
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

        bool fwd = GetAsyncKeyState('W') & 0x8000;
        bool bwd = GetAsyncKeyState('S') & 0x8000;
        bool left = GetAsyncKeyState('A') & 0x8000;
        bool right = GetAsyncKeyState('D') & 0x8000;

        cam.Update(fwd, bwd, left, right, world.map, deltaTime);

        RenderFrame(cam, world);
        SwapBuffers(hDC);
    }
}

void SoftwareEngine::Shutdown() {
    ShutdownOpenGL(hwnd, hDC, hRC);
}
