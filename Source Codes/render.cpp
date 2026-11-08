#include "render.h"
#include "core.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <GL/gl.h>
#include <vector>

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

    unsigned int dataPos = *(unsigned int*)&header[10];
    unsigned int width = *(unsigned int*)&header[18];
    unsigned int height = *(unsigned int*)&header[22];
    unsigned short bits = *(unsigned short*)&header[28];
    unsigned int compression = *(unsigned int*)&header[30];

    if (bits != 24 || compression != 0 ||
        width == 0 || height == 0) {
        fclose(file);
        return 0;
    }

    unsigned int rowSize = (width * 3 + 3) & ~3;
    unsigned int imageSize = rowSize * height;

    unsigned char *src = new unsigned char[imageSize];

    if (fseek(file, dataPos, SEEK_SET) != 0 ||
        fread(src, 1, imageSize, file) != imageSize) {
        delete[] src;
        fclose(file);
        return 0;
    }

    fclose(file);

    unsigned char *rgba =
        new unsigned char[width * height * 4];

    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            unsigned int srcPos = y * rowSize + x * 3;
            unsigned int dstPos = (y * width + x) * 4;

            unsigned char b = src[srcPos];
            unsigned char g = src[srcPos + 1];
            unsigned char r = src[srcPos + 2];

            rgba[dstPos] = r;
            rgba[dstPos + 1] = g;
            rgba[dstPos + 2] = b;

            // Magenta transparency
            if (r == 255 && g == 0 && b == 255)
                rgba[dstPos + 3] = 0;
            else
                rgba[dstPos + 3] = 255;
        }
    }

    delete[] src;

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_S, GL_CLAMP);

    glTexParameteri(GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        rgba
    );

    delete[] rgba;

    return textureID;
}

static std::vector<double> zBuffer;

static GLuint wallTexture = 0;
static GLuint gunTexture = 0;
static GLuint enemyTexture = 0;
static bool texturesLoaded = false;

void RenderFrame(Camera& cam, World& world) {
    if (!texturesLoaded) {
        wallTexture = LoadBMP("image\\Textures\\wall.bmp");
        gunTexture = LoadBMP("image\\Textures\\gun.bmp");
        enemyTexture = LoadBMP("image\\Textures\\enemy1.bmp");
        texturesLoaded = true;
    }

    glClear(GL_COLOR_BUFFER_BIT);

    // 1. GÖKYÜZÜ (Üst Kýsým - Mavi)
    int horizon = SCREEN_HEIGHT / 2 + (int)cam.pitch;
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.2f, 0.2f, 0.3f);
    glBegin(GL_QUADS);
        glVertex2i(0, 0);
        glVertex2i(SCREEN_WIDTH, 0);
        glVertex2i(SCREEN_WIDTH, horizon > 0 ? horizon : 0);
        glVertex2i(0, horizon > 0 ? horizon : 0);
    glEnd();

    // 2. ZEMÝN (Alt Kýsým - Gri/Toprak)
    glColor3f(0.0f, 0.25f, 0.0f);
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
    // 4. WEAPON HUD
    glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
if (gunTexture != 0) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, gunTexture);

    glColor3f(1.0f, 1.0f, 1.0f);

    // Silah boyutlarý
    int gunW = 512;
    int gunH = 512;

    // Ekranýn tam ortasý
    int gunX = (SCREEN_WIDTH - gunW) / 2;

    // Ekranýn alt kýsmý
    int gunY = SCREEN_HEIGHT - gunH;

    glBegin(GL_QUADS);

        glTexCoord2f(0.0f, 0.0f);
        glVertex2i(gunX, gunY);

        glTexCoord2f(0.0f, 1.0f);
        glVertex2i(gunX, gunY + gunH);

        glTexCoord2f(1.0f, 1.0f);
        glVertex2i(gunX + gunW, gunY + gunH);

        glTexCoord2f(1.0f, 0.0f);
        glVertex2i(gunX + gunW, gunY);

    glEnd();
}

    glDisable(GL_TEXTURE_2D);
}
