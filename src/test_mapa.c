#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include "map_data.h"

PSP_MODULE_INFO("DMC2DMap", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

#define VRAM  ((unsigned int*)0x44000000)
#define BUF_W 512
#define SCR_W 480
#define SCR_H 272
#define TILE  24

static void fill(int x, int y, int w, int h, unsigned int cor) {
    for (int j = 0; j < h; j++) {
        int sy = y + j;
        if (sy < 0 || sy >= SCR_H) continue;
        for (int i = 0; i < w; i++) {
            int sx = x + i;
            if (sx < 0 || sx >= SCR_W) continue;
            VRAM[sy * BUF_W + sx] = cor;
        }
    }
}

static unsigned int cor_tile(unsigned char t) {
    if (t == 0x1d || t == 0x1e || t == 0x1f) return 0xFF604080;  // parede roxa
    if (t == 0x14 || t == 0x15)               return 0xFF808080;  // chao cinza
    if (t == 0x13 || t == 0x0e)               return 0xFFA0A0A0;  // chao claro
    if (t == 0x10)                            return 0xFF404040;  // coluna
    if (t >= 0x04 && t <= 0x07)               return 0xFF2060C0;  // decoracao azul
    if (t == 0x01 || t == 0x00)               return 0xFF50A050;  // chao verde
    return 0xFF303030;
}

int main(void) {
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
    sceDisplaySetFrameBuf((void*)VRAM, BUF_W,
                          PSP_DISPLAY_PIXEL_FORMAT_8888,
                          PSP_DISPLAY_SETBUF_NEXTFRAME);

    float dx = 7.5f, dy = 27.0f;
    const float VEL = 0.2f;

    while (1) {
        SceCtrlData pad;
        sceCtrlReadBufferPositive(&pad, 1);

        if (pad.Buttons & PSP_CTRL_LEFT)  dx -= VEL;
        if (pad.Buttons & PSP_CTRL_RIGHT) dx += VEL;
        if (pad.Buttons & PSP_CTRL_UP)    dy -= VEL;
        if (pad.Buttons & PSP_CTRL_DOWN)  dy += VEL;

        if (dx < 0) dx = 0;
        if (dx > 14) dx = 14;
        if (dy < 0) dy = 0;
        if (dy > 29) dy = 29;

        for (int i = 0; i < BUF_W * SCR_H; i++) VRAM[i] = 0xFF101020;

        int cam_x = (int)(dx * TILE) - SCR_W / 2;
        int cam_y = (int)(dy * TILE) - SCR_H / 2;
        if (cam_x < 0) cam_x = 0;
        if (cam_y < 0) cam_y = 0;

        for (int ty = 0; ty < MAP_M0_H; ty++) {
            for (int tx = 0; tx < MAP_M0_W; tx++) {
                int sx = tx * TILE - cam_x;
                int sy = ty * TILE - cam_y;
                if (sx + TILE < 0 || sx > SCR_W) continue;
                if (sy + TILE < 0 || sy > SCR_H) continue;
                fill(sx, sy, TILE, TILE, cor_tile(map_m0[ty][tx]));
            }
        }

        int px = (int)(dx * TILE) - cam_x;
        int py = (int)(dy * TILE) - cam_y;
        fill(px + 4, py + 4, 16, 16, 0xFFFF2020);

        sceDisplayWaitVblankStart();
    }
    return 0;
}
