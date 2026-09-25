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

// Só ts0 e ts1 (que ficaram melhores)
#include "ts0.c"
#include "ts1.c"

static inline unsigned int fix_cor(unsigned int c) {
    return (c & 0xFF00FF00) | ((c & 0xFF0000) >> 16) | ((c & 0xFF) << 16);
}

static inline void put(int x, int y, unsigned int cor) {
    if (x >= 0 && x < SCR_W && y >= 0 && y < SCR_H)
        VRAM[y * BUF_W + x] = cor;
}

// Classifica tile em tipo (chao/parede/decor/ignora)
static int tipo_do_tile(unsigned char id) {
    // IDs conhecidos:
    if (id == 0x01 || id == 0x04 || id == 0x05 || id == 0x06 || id == 0x07) return 0; // chão verde
    if (id == 0x0a || id == 0x0e || id == 0x13) return 1; // chão secundário
    if (id == 0x14 || id == 0x15 || id == 0x16) return 2; // chão cinza principal
    if (id == 0x1d || id == 0x1e || id == 0x1f) return 3; // parede lateral
    if (id == 0x10) return 4; // coluna
    if (id >= 0x80) return -1; // FLAG (não desenha)
    return 5; // outros
}

static unsigned int cor_do_tipo(int tipo) {
    switch (tipo) {
        case 0: return 0xFF304020;  // chão verde escuro (ABGR)
        case 1: return 0xFF505040;  // chão secundário
        case 2: return 0xFF808080;  // chão cinza principal
        case 3: return 0xFF402030;  // parede (borda)
        case 4: return 0xFF202020;  // coluna
        default: return 0xFF404040;
    }
}

// Desenha tile com textura do ts0 (mais confiável)
static void desenha_tile_real(int sx, int sy, unsigned char id) {
    // Mapeia id pra posição no ts0 (25 tiles, índice 0-24)
    int local = id % 25;
    int tx = (local % 5) * TILE;
    int ty = (local / 5) * TILE;
    for (int j = 0; j < TILE; j++) {
        int py = sy + j;
        if (py < 0 || py >= SCR_H) continue;
        for (int i = 0; i < TILE; i++) {
            int px = sx + i;
            if (px < 0 || px >= SCR_W) continue;
            unsigned int cor = ts0_pixels[(ty + j) * TS0_W + (tx + i)];
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
    const float VEL = 0.15f;

    while (1) {
        SceCtrlData pad;
        sceCtrlReadBufferPositive(&pad, 1);
        if (pad.Buttons & PSP_CTRL_LEFT)  dx -= VEL;
        if (pad.Buttons & PSP_CTRL_RIGHT) dx += VEL;
        if (pad.Buttons & PSP_CTRL_UP)    dy -= VEL;
        if (pad.Buttons & PSP_CTRL_DOWN)  dy += VEL;
        if (dx < 0) dx = 0; if (dx > 14) dx = 14;
        if (dy < 0) dy = 0; if (dy > 29) dy = 29;

        // Fundo escuro
        for (int i = 0; i < BUF_W * SCR_H; i++) VRAM[i] = 0xFF101010;

        int cam_x = (int)(dx * TILE) - SCR_W / 2;
        int cam_y = (int)(dy * TILE) - SCR_H / 2;
        if (cam_x < 0) cam_x = 0;
        if (cam_y < 0) cam_y = 0;

        // Duas passadas: primeiro chão, depois detalhes
        for (int pass = 0; pass < 2; pass++) {
            for (int ty = 0; ty < MAP_M0_H; ty++) {
                for (int tx = 0; tx < MAP_M0_W; tx++) {
                    int sx = tx * TILE - cam_x;
                    int sy = ty * TILE - cam_y;
                    if (sx + TILE < 0 || sx > SCR_W) continue;
                    if (sy + TILE < 0 || sy > SCR_H) continue;

                    unsigned char id = map_m0[ty][tx];
                    int tipo = tipo_do_tile(id);

                    if (pass == 0) {
                        // Chão sempre
                        if (tipo >= 0 && tipo <= 2) {
                            desenha_tile_real(sx, sy, id);
                        }
                    } else {
                        // Detalhes por cima
                        if (tipo == 3 || tipo == 4 || tipo == 5) {
                            desenha_tile_real(sx, sy, id);
                        }
                    }
                }
            }
        }

        // Dante (sprite real)
        const SpriteEntry* dante = sprite_lookup("dante_main");
        if (dante) {
            int px = (int)(dx * TILE) - cam_x;
            int py = (int)(dy * TILE) - cam_y;
            // Desenha só um pedaço do spritesheet (o frame 0)
            for (int j = 0; j < 32; j++) {
                for (int i = 0; i < 32; i++) {
                    if (j >= dante->h || i >= dante->w) continue;
                    unsigned int cor = dante->pixels[j * dante->w + i];
                    if (cor >> 24) put(px + i, py + j, fix_cor(cor));
                }
            }
        } else {
            // fallback: quadrado vermelho
            int px = (int)(dx * TILE) - cam_x;
            int py = (int)(dy * TILE) - cam_y;
            for (int j = 0; j < 16; j++)
                for (int i = 0; i < 16; i++)
                    put(px + 4 + i, py + 4 + j, 0xFF2020FF);
        }

        sceDisplayWaitVblankStart();
    }
    return 0;
}
