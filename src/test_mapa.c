#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspctrl.h>

PSP_MODULE_INFO("DMC2DMap", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

#define VRAM  ((unsigned int*)0x44000000)
#define BUF_W 512
#define SCR_W 480
#define SCR_H 272
#define TILE  24

#include "map_data.h"
#include "ts0.c"
#include "ts1.c"
#include "ts2.c"
#include "ts3.c"

static inline unsigned int fix_cor(unsigned int c) {
    return (c & 0xFF00FF00) | ((c & 0xFF0000) >> 16) | ((c & 0xFF) << 16);
}

static inline void put(int x, int y, unsigned int cor) {
    if (x >= 0 && x < SCR_W && y >= 0 && y < SCR_H)
        VRAM[y * BUF_W + x] = cor;
}

// Pega o pixel do tile certo (id 0-255 → 4 tilesets de 25)
static unsigned int pixel_do_tile(unsigned char id, int px, int py) {
    int ts_idx = id / 25;    // qual tileset (0-3)
    int local  = id % 25;    // posição no tileset
    if (ts_idx > 3) { ts_idx = 0; local = 0; }

    int tx = (local % 5) * TILE + px;
    int ty = (local / 5) * TILE + py;

    const unsigned int* ts_pix = NULL;
    int ts_w = 0;
    switch (ts_idx) {
        case 0: ts_pix = ts0_pixels; ts_w = TS0_W; break;
        case 1: ts_pix = ts1_pixels; ts_w = TS1_W; break;
        case 2: ts_pix = ts2_pixels; ts_w = TS2_W; break;
        case 3: ts_pix = ts3_pixels; ts_w = TS3_W; break;
    }
    if (!ts_pix) return 0;
    return ts_pix[ty * ts_w + tx];
}

static void desenha_tile(int sx, int sy, unsigned char id) {
    for (int j = 0; j < TILE; j++) {
        int py = sy + j;
        if (py < 0 || py >= SCR_H) continue;
        for (int i = 0; i < TILE; i++) {
            int px = sx + i;
            if (px < 0 || px >= SCR_W) continue;
            unsigned int cor = pixel_do_tile(id, i, j);
            if (cor >> 24) VRAM[py * BUF_W + px] = fix_cor(cor);
        }
    }
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
        if (dx < 0) dx = 0; if (dx > 14) dx = 14;
        if (dy < 0) dy = 0; if (dy > 29) dy = 29;

        for (int i = 0; i < BUF_W * SCR_H; i++) VRAM[i] = 0xFF101010;

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
                desenha_tile(sx, sy, map_m0[ty][tx]);
            }
        }

        int px = (int)(dx * TILE) - cam_x;
        int py = (int)(dy * TILE) - cam_y;
        for (int j = 0; j < 16; j++)
            for (int i = 0; i < 16; i++)
                put(px + 4 + i, py + 4 + j, 0xFF2020FF);

        sceDisplayWaitVblankStart();
    }
    return 0;
}
