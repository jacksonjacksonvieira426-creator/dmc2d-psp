// Teste relampago: desenha o title.png na tela
#include <pspkernel.h>
#include <pspdisplay.h>
#include <string.h>
#include "sprites_table.h"

PSP_MODULE_INFO("DMC2DTest", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

#define VRAM ((unsigned int*)0x44000000)
#define BUF_WIDTH 512

int main(void) {
    const SpriteEntry* s = sprite_lookup("title");
    if (!s) { sceKernelExitGame(); return 0; }

    // Fundo preto
    for (int i = 0; i < 512 * 272; i++) VRAM[i] = 0xFF000000;

    // Desenha title centralizado
    int ox = (480 - s->w) / 2;
    int oy = (272 - s->h) / 2;
    for (int y = 0; y < s->h; y++) {
        for (int x = 0; x < s->w; x++) {
            unsigned int cor = s->pixels[y * s->w + x];
            if (cor >> 24)
                VRAM[(oy + y) * BUF_WIDTH + (ox + x)] = cor;
        }
    }

    while (1) sceDisplayWaitVblankStart();
    sceKernelExitGame();
    return 0;
}
