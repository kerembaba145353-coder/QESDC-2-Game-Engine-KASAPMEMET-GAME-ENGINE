#include "opengl.h"

bool InitOpenGL(HWND hwnd, HDC& hDC, HGLRC& hRC) {
    hDC = GetDC(hwnd);
    
    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory(&pfd, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int format = ChoosePixelFormat(hDC, &pfd);
    if (!format) return false;
    if (!SetPixelFormat(hDC, format, &pfd)) return false;

    hRC = wglCreateContext(hDC);
    if (!hRC) return false;

    wglMakeCurrent(hDC, hRC);
    return true;
}

void ShutdownOpenGL(HWND hwnd, HDC hDC, HGLRC hRC) {
    if (hRC) {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(hRC);
    }
    if (hDC) {
        ReleaseDC(hwnd, hDC);
    }
}

void ResizeOpenGL(int width, int height) {
    if (height == 0) height = 1;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
