#include <windows.h>
#include <fstream>
#include "Software.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	std::ifstream check("image\\found.found");
	if (!check){
		MessageBox(NULL, "Didnt find image folder and contents please download game or image folder content again", "FATAL ERROR", MB_OK | MB_ICONSTOP);
		ExitProcess(0);
	}
	
	PlaySound("image\\media\\wav.wav", NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
	ShowCursor(FALSE);
    SoftwareEngine engine;

    if (!engine.Initialize(hInstance)) {
        return -1;
    }

    engine.Run();
    engine.Shutdown();

    return 0;
}
