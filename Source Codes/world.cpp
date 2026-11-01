#include "world.h"
#include <windows.h>

// 1. Parametresiz çaðrýldýðýnda varsayýlan haritayý oluþturur
World::World() {
    for(int y = 0; y < MAP_H; ++y) {
        for(int x = 0; x < MAP_W; ++x) {
            if (x == 0 || x == MAP_W - 1 || y == 0 || y == MAP_H - 1) {
                map[y][x] = 1;
            } else {
                map[y][x] = 0;
            }
        }
    }
}

// 2. Win32 API kullanarak .txt dosyasýný okur
World::World(const std::string& filename) {
    // Önce güvenlik amaçlý haritayý varsayýlan duvarlarla dolduralým
    for(int y = 0; y < MAP_H; ++y) {
        for(int x = 0; x < MAP_W; ++x) {
            if (x == 0 || x == MAP_W - 1 || y == 0 || y == MAP_H - 1) {
                map[y][x] = 1;
            } else {
                map[y][x] = 0;
            }
        }
    }

    // Win32 API ile dosyayý açýyoruz
    HANDLE hFile = CreateFileA(
        filename.c_str(), 
        GENERIC_READ, 
        FILE_SHARE_READ, 
        NULL, 
        OPEN_EXISTING, 
        FILE_ATTRIBUTE_NORMAL, 
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        return; // Dosya bulunamazsa varsayýlan harita ile devam eder
    }

    DWORD fileSize = GetFileSize(hFile, NULL);
    if (fileSize == INVALID_FILE_SIZE || fileSize == 0) {
        CloseHandle(hFile);
        return;
    }

    // Dosya içeriðini belleðe okumak için bellek ayýrýyoruz
    char* buffer = new char[fileSize + 1];
    DWORD bytesRead = 0;

    if (ReadFile(hFile, buffer, fileSize, &bytesRead, NULL)) {
        buffer[bytesRead] = '\0';

        // Metin içindeki karakterleri tarayýp matrise aktarýyoruz
        int x = 0;
        int y = 0;
        char* ptr = buffer;

        while (*ptr != '\0' && y < MAP_H) {
            if (*ptr >= '0' && *ptr <= '9') {
                int val = *ptr - '0';
                if (x < MAP_W) {
                    map[y][x] = val;
                    x++;
                }
            } else if (*ptr == '\n') {
                y++;
                x = 0;
            }
            ptr++;
        }
    }

    delete[] buffer;
    CloseHandle(hFile);
}
