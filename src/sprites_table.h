// Auto-gerado
#ifndef SPRITES_TABLE_H
#define SPRITES_TABLE_H

typedef struct {
    const char* nome;
    const unsigned int* pixels;
    int w, h;
} SpriteEntry;

#define SPRITES_COUNT 54

extern const SpriteEntry sprites_table[];
const SpriteEntry* sprite_lookup(const char* nome);
#endif
