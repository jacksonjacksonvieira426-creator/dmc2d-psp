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

// Classifica tile por FAIXA (mais confiável que ID exato)
static unsigned int cor_do_tile(unsigned char t) {
    // Chão andável (maioria dos IDs baixos)
    if (t <= 0x01) return 0xFF3A7040;   // chão verde escuro
    if (t <= 0x0a) return 0xFF3A7040;   // chão verde
    if (t <= 0x18) return 0xFF506050;   // chão cinza esverdeado
    // Paredes (IDs 0x1d-0x1f e altos)
    if (t == 0x1d || t == 0x1e || t == 0x1f) return 0xFF303050;  // parede roxa escura
    if (t == 0x10) return 0xFF202030;   // coluna
    // Decoração
    if (t >= 0x40 && t < 0x80) return 0xFF4060A0;  // decoração azul
    // Especiais (flip/invisible)
    if (t >= 0x80) return 0;  // 0 = não desenha (vazio)
    return 0xFF404040;
}

// Colisão: retorna 1 se for parede
static int tile_solido(unsigned char t) {
    // Só IDs conhecidos como parede
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

        // Movimento com colisão
        float nx = dx, ny = dy;
        if (pad.Buttons & PSP_CTRL_LEFT)  nx -= VEL;
        if (pad.Buttons & PSP_CTRL_RIGHT) nx += VEL;
        if (pad.Buttons & PSP_CTRL_UP)    ny -= VEL;
        if (pad.Buttons & PSP_CTRL_DOWN)  ny += VEL;

        // Clamp no grid
        if (nx < 0) nx = 0;
        if (nx > 14) nx = 14;
        if (ny < 0) ny = 0;
        if (ny > 29) ny = 29;

        // Verifica colisão
        int tx = (int)nx;
        int ty = (int)ny;
        if (tx >= 0 && tx < MAP_M0_W && ty >= 0 && ty < MAP_M0_H) {
            if (!tile_solido(map_m0[ty][tx])) {
                dx = nx;
                dy = ny;
            }
        }

        // Limpa
        for (int i = 0; i < BUF_W * SCR_H; i++) VRAM[i] = 0xFF101010;

        int cam_x = (int)(dx * TILE) - SCR_W / 2;
        int cam_y = (int)(dy * TILE) - SCR_H / 2;
        if (cam_x < 0) cam_x = 0;
        if (cam_y < 0) cam_y = 0;

        // Desenha mapa (cores sólidas)
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

        // Dante como sprite
        const SpriteEntry* s = sprite_lookup("dante_main");
        int px = (int)(dx * TILE) - cam_x;
        int py = (int)(dy * TILE) - cam_y;

        if (s) {
            // Recorta frame 0 do spritesheet (primeira célula 32x32)
            int fw = 32, fh = 32;
            for (int j = 0; j < fh; j++) {
                for (int i = 0; i < fw; i++) {
                    if (j >= s->h || i >= s->w) continue;
                    unsigned int cor = s->pixels[j * s->w + i];
                    if (cor >> 24) put(px + i, py + j, fix_cor(cor));
                }
            }
        } else {
            // Fallback: quadrado vermelho
            fill(px + 8, py + 8, 16, 16, 0xFF2020FF);
        }

        sceDisplayWaitVblankStart();
    }
    return 0;
}
