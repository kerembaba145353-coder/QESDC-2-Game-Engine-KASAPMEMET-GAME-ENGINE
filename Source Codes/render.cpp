#include "render.h"
#include "core.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <GL/gl.h>

// BMP Yükleme Fonksiyonu
static GLuint LoadBMP(const char* filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) return 0;

    unsigned char header[54];
    if (fread(header, 1, 54, file) != 54) {
        fclose(file);
        return 0;
    }

    if (header[0] != 'B' || header[1] != 'M') {
        fclose(file);
        return 0;
    }

    unsigned int dataPos   = *(int*)&(header[0x0A]);
    unsigned int imageSize = *(int*)&(header[0x22]);
    unsigned int width     = *(int*)&(header[0x12]);
    unsigned int height    = *(int*)&(header[0x16]);

    if (imageSize == 0) imageSize = width * height * 3;
    if (dataPos == 0) dataPos = 54;

    unsigned char *data = new unsigned char[imageSize];
    fread(data, 1, imageSize, file);
    fclose(file);

    for (unsigned int i = 0; i < imageSize; i += 3) {
        unsigned char temp = data[i];
        data[i] = data[i + 2];
        data[i + 2] = temp;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // GL_CLAMP_TO_EDGE yerine OpenGL 1.1 uyumlu GL_CLAMP kullanýyoruz:
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    delete[] data;
    return textureID;
}

static GLuint wallTexture = 0;
static GLuint gunTexture = 0;
static bool texturesLoaded = false;

void RenderFrame(Camera& cam, World& world) {
    if (!texturesLoaded) {
        wallTexture = LoadBMP("image\\Textures\\wall.bmp");
        gunTexture = LoadBMP("image\\Textures\\gun.bmp");
        texturesLoaded = true;
    }

    glClear(GL_COLOR_BUFFER_BIT);

    // 1. GÖKYÜZÜ (Üst Kýsým - Mavi)
    int horizon = SCREEN_HEIGHT / 2 + (int)cam.pitch;
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.3f, 0.6f, 0.9f);
    glBegin(GL_QUADS);
        glVertex2i(0, 0);
        glVertex2i(SCREEN_WIDTH, 0);
        glVertex2i(SCREEN_WIDTH, horizon > 0 ? horizon : 0);
        glVertex2i(0, horizon > 0 ? horizon : 0);
    glEnd();

    // 2. ZEMÝN (Alt Kýsým - Gri/Toprak)
    glColor3f(0.25f, 0.25f, 0.25f);
    glBegin(GL_QUADS);
        glVertex2i(0, horizon < SCREEN_HEIGHT ? horizon : SCREEN_HEIGHT);
        glVertex2i(SCREEN_WIDTH, horizon < SCREEN_HEIGHT ? horizon : SCREEN_HEIGHT);
        glVertex2i(SCREEN_WIDTH, SCREEN_HEIGHT);
        glVertex2i(0, SCREEN_HEIGHT);
    glEnd();

    // 3. DUVARLAR (Tuðla Dokusu)
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, wallTexture);

    for (int x = 0; x < SCREEN_WIDTH; x++) {
        double cameraX = 2.0 * x / (double)SCREEN_WIDTH - 1.0;
        double rayDirX = cam.dirX + cam.planeX * cameraX;
        double rayDirY = cam.dirY + cam.planeY * cameraX;

        int mapX = (int)cam.posX;
        int mapY = (int)cam.posY;

        double sideDistX, sideDistY;
        double deltaDistX = (rayDirX == 0) ? 1e30 : std::abs(1.0 / rayDirX);
        double deltaDistY = (rayDirY == 0) ? 1e30 : std::abs(1.0 / rayDirY);
        double perpWallDist;

        int stepX, stepY;
        int hit = 0;
        int side;

        if (rayDirX < 0) {
            stepX = -1;
            sideDistX = (cam.posX - mapX) * deltaDistX;
        } else {
            stepX = 1;
            sideDistX = (mapX + 1.0 - cam.posX) * deltaDistX;
        }
        if (rayDirY < 0) {
            stepY = -1;
            sideDistY = (cam.posY - mapY) * deltaDistY;
        } else {
            stepY = 1;
            sideDistY = (mapY + 1.0 - cam.posY) * deltaDistY;
        }

        while (hit == 0) {
            if (sideDistX < sideDistY) {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            } else {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }
            if (mapX < 0 || mapX >= World::MAP_W || mapY < 0 || mapY >= World::MAP_H) {
                hit = 1;
                break;
            }
            if (world.map[mapY][mapX] > 0) hit = 1;
        }

        if (side == 0) perpWallDist = (sideDistX - deltaDistX);
        else           perpWallDist = (sideDistY - deltaDistY);

        if (perpWallDist < 0.01) perpWallDist = 0.01;

        int lineHeight = (int)(SCREEN_HEIGHT / perpWallDist);
        int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2 + (int)cam.pitch;
        if (drawStart < 0) drawStart = 0;
        int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2 + (int)cam.pitch;
        if (drawEnd >= SCREEN_HEIGHT) drawEnd = SCREEN_HEIGHT - 1;

        double wallX;
        if (side == 0) wallX = cam.posY + perpWallDist * rayDirY;
        else           wallX = cam.posX + perpWallDist * rayDirX;
        wallX -= floor(wallX);

        double texX = wallX;
        if (side == 0 && rayDirX > 0) texX = 1.0 - texX;
        if (side == 1 && rayDirY < 0) texX = 1.0 - texX;

        if (side == 1) glColor3f(0.7f, 0.7f, 0.7f);
        else           glColor3f(1.0f, 1.0f, 1.0f);

        glBegin(GL_QUADS);
            glTexCoord2f(texX, 0.0f); glVertex2i(x, drawStart);
            glTexCoord2f(texX, 1.0f); glVertex2i(x, drawEnd);
            glTexCoord2f(texX, 1.0f); glVertex2i(x + 1, drawEnd);
            glTexCoord2f(texX, 0.0f); glVertex2i(x + 1, drawStart);
        glEnd();
    }

    // 4. EKRANIN ALTINA SÝLAH (HUD) ÇÝZÝMÝ
    if (gunTexture != 0) {
        glBindTexture(GL_TEXTURE_2D, gunTexture);
        glColor3f(1.0f, 1.0f, 1.0f);

        int gunW = 64; // Silahýn ekrandaki geniþliði
        int gunH = 64; // Silahýn ekrandaki yüksekliði
        int gunX = (SCREEN_WIDTH - gunW); // Ekranýn ortasý (biraz saða kayýk)
        int gunY = SCREEN_HEIGHT - gunH;         // Ekranýn alt kýsmý

        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex2i(gunX, gunY);
            glTexCoord2f(0.0f, 1.0f); glVertex2i(gunX, gunY + gunH);
            glTexCoord2f(1.0f, 1.0f); glVertex2i(gunX + gunW, gunY + gunH);
            glTexCoord2f(1.0f, 1.0f); glVertex2i(gunX + gunW, gunY);
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);
}
