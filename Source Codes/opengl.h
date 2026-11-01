#ifndef OPENGL_H
#define OPENGL_H

#include <windows.h>
#include <GL/gl.h>

bool InitOpenGL(HWND hwnd, HDC& hDC, HGLRC& hRC);
void ShutdownOpenGL(HWND hwnd, HDC hDC, HGLRC hRC);
void ResizeOpenGL(int width, int height);

#endif
