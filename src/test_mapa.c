#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include "sprites_table.h"

PSP_MODULE_INFO("DMC2DSheet", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

#define VRAM  ((unsigned int*)0x44000000)
#define BUF_W 512
#define SCR_W 480
#define SCR_H 272

static inline unsigned int fix_cor(unsigned int c) {
    return (c & 0xFF00FF00) | ((c & 0xFF0000) >> 16) | ((c & 0xFF) << 16);
}

int main(void) {
    sceDisplaySetFrameBuf((void*)VRAM, BUF_W,
                          PSP_DISPLAY_PIXEL_FORMAT_8888,
                          PSP_DISPLAY_SETBUF_NEXTFRAME);

    // Mostra o s0 (dante_main) INTEIRO, 1:1
    const SpriteEntry* s = sprite_lookup("dante_fight");
    if (!s) { sceKernelExitGame(); return 0; }

    while (1) {
        // Fundo
        for (int i = 0; i < BUF_W * SCR_H; i++) VRAM[i] = 0xFF101010;

        // Desenha o spritesheet em escala 1.4x pra preencher a tela
        // (339x288 → ~475x403, corta o excesso)
        int scale_num = 14;   // 1.4x
        int scale_den = 10;
        for (int j = 0; j < SCR_H; j++) {
            int sy = (j * scale_den) / scale_num;
            if (sy >= s->h) continue;
            for (int i = 0; i < SCR_W; i++) {
                int sx = (i * scale_den) / scale_num;
                if (sx >= s->w) continue;
                unsigned int cor = s->pixels[sy * s->w + sx];
                if (cor >> 24)
                    VRAM[j * BUF_W + i] = fix_cor(cor);
            }
        }

        sceDisplayWaitVblankStart();
    }
    return 0;
}
