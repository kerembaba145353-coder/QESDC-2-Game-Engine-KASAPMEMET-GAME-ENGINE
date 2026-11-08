#include <windows.h>
#include <fstream>
#include "Software.h"
#include "core.h"
#include <cstring>

bool intro = true;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	std::ifstream check("image\\found.found");
	if (!check){
		MessageBox(NULL, "Didnt find image folder and contents please download game or image folder content again", "FATAL ERROR", MB_OK | MB_ICONSTOP);
		ExitProcess(0);
	}
	if (strstr(lpCmdLine, "-nosound")){
		PlaySound(NULL, NULL, 0);
	}
	else if (strstr(lpCmdLine, "-novid")){
		intro = false;
		system("taskkill /f /im wmplayer.exe");
		system("taskkill /f /im vlc.exe");
	}
	if (intro){
		PlaySound(NULL, NULL, 0);
		ShellExecute(NULL, "open", "image\\media\\intro.mpg", NULL, NULL, SW_SHOW);
		Sleep(25000);
		system("taskkill /f /im wmplayer.exe");
		system("taskkill /f /im vlc.exe");
		intro = false;
	}
	PlaySound("image\\media\\wind.wav", NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
	ShowCursor(FALSE);
    SoftwareEngine engine;

    if (!engine.Initialize(hInstance)) {
        return -1;
    }
    engine.Run();
    engine.Shutdown();

    return 0;
}
