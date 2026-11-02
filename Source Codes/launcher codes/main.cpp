#include <windows.h>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <ctime>

HWND hwnd;
HWND hBtn2;
HWND hBtn;
int Sx = GetSystemMetrics(SM_CXSCREEN);
int Sy = GetSystemMetrics(SM_CYSCREEN);

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch(Message) {
        
        case WM_COMMAND: {
            int id = LOWORD(wParam);
            
            if(id == 2){
            	ShellExecute(hwnd, "runas", "Backrooms.exe", NULL, NULL, SW_SHOW);
            	ExitProcess(0);
			}
			else if(id == 1){
            	MessageBox(hwnd, "I am 7 th grade student i made a game and launcher i am not a stupid but i am not a clever", "About", MB_OK);
			}
			break;
        }
        case WM_CLOSE:{
        	ExitProcess(0);
			break;
        }
        
        case WM_DESTROY: {
    ExitProcess(0);
            break;
        }
        
        default:
            return DefWindowProc(hwnd, Message, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    WNDCLASSEX wc;  
    MSG msg; 

    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc; 
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDI_EXCLAMATION);
    wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
    wc.lpszClassName = "WindowClass";
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION); 
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION); 

    if(!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION|MB_OK);
        return 0;
    }

    hwnd = CreateWindowEx(
        0,                              // Ekstres stiller
        "WindowClass",                     // Sýnýf adý
        "ENGINE Launcher",      // Pencere Baþlýðý
        WS_POPUP | WS_VISIBLE,            // Pencere stili
        0, 0,   // X, Y koordinatlarý
        Sx, Sy,                       // Geniþlik, Yükseklik
        NULL,                           // Ebeveyn pencere (Parent)
        NULL,                           // Menü
        hInstance,                      // Uygulama örneði (Instance)
        NULL                            // Ek parametreler
    );
        
    hBtn2 = CreateWindowEx(0, "BUTTON", "Play", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 500, 250, 150, 40, hwnd, (HMENU)2, hInstance, NULL);
    hBtn = CreateWindowEx(0, "BUTTON", "About", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 500, 390, 150, 40, hwnd, (HMENU)1, hInstance, NULL);

    if(hwnd == NULL) {
        MessageBox(NULL, "Cant open window", "Error", MB_ICONSTOP | MB_OK);
        return 0;
    }

    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            
        }
    }
    return (int)msg.wParam;
}
