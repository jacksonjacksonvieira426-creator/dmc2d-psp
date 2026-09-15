// Teste relampago: desenha title.png na tela (v2)
#include <pspkernel.h>
#include <pspdisplay.h>
#include <string.h>
#include "sprites_table.h"

PSP_MODULE_INFO("DMC2DTest", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

#define VRAM ((unsigned int*)0x44000000)
#define BUF_WIDTH 512
#define SCR_WIDTH 480
#define SCR_HEIGHT 272

int main(void) {
    const SpriteEntry* s = sprite_lookup("title");
    if (!s) { sceKernelExitGame(); return 0; }

    // Avisa o PSP/PPSSPP qual eh o framebuffer
    sceDisplaySetFrameBuf((void*)VRAM, BUF_WIDTH,
                          PSP_DISPLAY_PIXEL_FORMAT_8888,
                          PSP_DISPLAY_SETBUF_NEXTFRAME);

    while (1) {
        // Fundo preto — desenha TODO frame
        for (int i = 0; i < BUF_WIDTH * SCR_HEIGHT; i++)
            VRAM[i] = 0xFF000000;

        // Desenha title centralizado
        int ox = (SCR_WIDTH  - s->w) / 2;
        int oy = (SCR_HEIGHT - s->h) / 2;
        if (ox < 0) ox = 0;
        if (oy < 0) oy = 0;

        for (int y = 0; y < s->h && (oy + y) < SCR_HEIGHT; y++) {
            for (int x = 0; x < s->w && (ox + x) < SCR_WIDTH; x++) {
                unsigned int cor = s->pixels[y * s->w + x];
                if (cor >> 24)
                    VRAM[(oy + y) * BUF_WIDTH + (ox + x)] = cor;
            }
        }

        sceDisplayWaitVblankStart();
    }
    sceKernelExitGame();
    return 0;
}
