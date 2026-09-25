#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include "map_data.h"
#include "sprites_table.h"

PSP_MODULE_INFO("DMC2DMap", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

#define VRAM  ((unsigned int*)0x44000000)
#define BUF_W 512
#define SCR_W 480
#define SCR_H 272
#define TILE  24

static inline unsigned int fix_cor(unsigned int c) {
    return (c & 0xFF00FF00) | ((c & 0xFF0000) >> 16) | ((c & 0xFF) << 16);
}

static inline void put(int x, int y, unsigned int cor) {
    if (x >= 0 && x < SCR_W && y >= 0 && y < SCR_H)
        VRAM[y * BUF_W + x] = cor;
}

static void fill(int x, int y, int w, int h, unsigned int cor) {
    for (int j = 0; j < h; j++) {
        int py = y + j;
        if (py < 0 || py >= SCR_H) continue;
        for (int i = 0; i < w; i++) {
            int px = x + i;
            if (px < 0 || px >= SCR_W) continue;
            VRAM[py * BUF_W + px] = cor;
        }
    }
}

// Paleta TEMPLO (ABGR)
static unsigned int cor_do_tile(unsigned char t) {
    if (t <= 0x01) return 0xFF3A3028;   // chão pedra escura
    if (t <= 0x0a) return 0xFF4A4038;   // chão pedra
    if (t <= 0x18) return 0xFF5A5048;   // chão pedra clara
    if (t == 0x1d || t == 0x1e || t == 0x1f) return 0xFF1A1A2A;  // parede escura
    if (t == 0x10) return 0xFF0A0A1A;   // coluna
    if (t >= 0x40 && t < 0x80) return 0xFF3A2A1A;  // decoração marrom
    if (t >= 0x80) return 0;            // invisível
    return 0xFF3A3028;
}

static int tile_solido(unsigned char t) {
    if (t == 0x1d || t == 0x1e || t == 0x1f) return 1;
    if (t == 0x10) return 1;
    if (t >= 0x40 && t < 0x80) return 1;
    return 0;
}

int main(void) {
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
    sceDisplaySetFrameBuf((void*)VRAM, BUF_W,
                          PSP_DISPLAY_PIXEL_FORMAT_8888,
                          PSP_DISPLAY_SETBUF_NEXTFRAME);

    float dx = 7.5f, dy = 27.0f;
    const float VEL = 0.15f;

    while (1) {
        SceCtrlData pad;
        sceCtrlReadBufferPositive(&pad, 1);

        float nx = dx, ny = dy;
        if (pad.Buttons & PSP_CTRL_LEFT)  nx -= VEL;
        if (pad.Buttons & PSP_CTRL_RIGHT) nx += VEL;
        if (pad.Buttons & PSP_CTRL_UP)    ny -= VEL;
        if (pad.Buttons & PSP_CTRL_DOWN)  ny += VEL;

        if (nx < 0) nx = 0; if (nx > 14) nx = 14;
        if (ny < 0) ny = 0; if (ny > 29) ny = 29;

        int tx = (int)nx, ty = (int)ny;
        if (tx >= 0 && tx < MAP_M0_W && ty >= 0 && ty < MAP_M0_H) {
            if (!tile_solido(map_m0[ty][tx])) { dx = nx; dy = ny; }
        }

        for (int i = 0; i < BUF_W * SCR_H; i++) VRAM[i] = 0xFF08080A;

        int cam_x = (int)(dx * TILE) - SCR_W / 2;
        int cam_y = (int)(dy * TILE) - SCR_H / 2;
        if (cam_x < 0) cam_x = 0; if (cam_y < 0) cam_y = 0;

        for (int tyy = 0; tyy < MAP_M0_H; tyy++) {
            for (int txx = 0; txx < MAP_M0_W; txx++) {
                int sx = txx * TILE - cam_x;
                int sy = tyy * TILE - cam_y;
                if (sx + TILE < 0 || sx > SCR_W) continue;
                if (sy + TILE < 0 || sy > SCR_H) continue;
                unsigned int cor = cor_do_tile(map_m0[tyy][txx]);
                if (cor == 0) continue;
                fill(sx + 1, sy + 1, TILE - 2, TILE - 2, cor);
            }
        }

        // Dante sprite real (frame MAIOR do spritesheet)
        const SpriteEntry* s = sprite_lookup("dante_main");
        int px = (int)(dx * TILE) - cam_x;
        int py = (int)(dy * TILE) - cam_y;

        if (s) {
            // Dante está em (16, 16) 64x64 do spritesheet (primeira pose grande)
            int fx = 16, fy = 16, fw = 64, fh = 64;
            for (int j = 0; j < fh; j++) {
                for (int i = 0; i < fw; i++) {
                    int sx = fx + i, sy = fy + j;
                    if (sy >= s->h || sx >= s->w) continue;
                    unsigned int cor = s->pixels[sy * s->w + sx];
                    if (cor >> 24) put(px - 20 + i, py - 40 + j, fix_cor(cor));
                }
            }
        } else {
            fill(px - 8, py - 8, 16, 16, 0xFF2020FF);
        }

        sceDisplayWaitVblankStart();
    }
    return 0;
}
