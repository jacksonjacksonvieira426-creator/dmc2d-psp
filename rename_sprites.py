#!/usr/bin/env python3
"""Renomeia sprites DMC2D com nomes semanticos."""
import os, re, sys

MAP = {
    "capcom_logo": "boot_capcom", "add": "title_main",
    "tl_dante": "title_dante", "tl_txt": "title_logo",
    "title": "font_big", "small": "font_small", "small_sp": "font_small_sp",
    "s0": "dante_main", "p0": "dante_fight", "d0": "dante_armor",
    "b0": "dante_atk_h", "b1": "dante_guns", "b2": "dante_dark", "b3": "dante_close",
    "a0": "enemy_spider", "h11": "tile_wall_col",
    "bar": "hud_hp_bar", "h3": "hud_combo_text", "mm": "hud_menu",
    "icon": "hud_icon_dante", "h2": "hud_arrow", "h8": "hud_arrow_sm",
    "ig3": "hud_bar_bg", "ig4": "hud_bar_fill",
    "h4": "item_pistols", "h0": "item_sword", "d2": "item_swords",
    "ig7": "item_pillar", "ig8": "item_statue1", "ig9": "item_statue2",
    "h5": "item_potion1", "h6": "item_potion2", "h7": "item_boot",
    "wi_e": "item_weapon_e", "wi_r": "item_weapon_r", "ig10": "item_crystal",
    "d3": "fx_slash", "v2": "fx_cut", "l1": "fx_particles",
    "h9": "fx_vortex", "ig2": "fx_fire", "ig5": "fx_fire_sm",
    "ig6": "fx_fire_sm2", "ig11": "fx_star",
    "ig0": "orb_red", "ig1": "orb_green", "h1": "orb_big",
    "d1": "orb_grid", "v1": "coin_grid",
    "ts0": "tile_forest", "ts1": "tile_wall", "ts2": "tile_dark",
    "ts3": "tile_stone", "h10": "tile_lines",
}

PASTA = "src/sprites_data"

def rename_arquivo(old, new):
    old_path = f"{PASTA}/{old}.c"
    new_path = f"{PASTA}/{new}.c"
    if not os.path.exists(old_path):
        print(f"  SKIP {old} (nao existe)")
        return False

    old_up, new_up = old.upper(), new.upper()
    with open(old_path) as f:
        txt = f.read()

    # Ordem importa!
    txt = txt.replace(f"_{old_up}_H", f"_{new_up}_H")   # guard
    txt = txt.replace(f"{old_up}_W", f"{new_up}_W")     # largura
    txt = txt.replace(f"{old_up}_H", f"{new_up}_H")     # altura
    txt = txt.replace(f"{old}_pixels", f"{new}_pixels") # array

    # Marca origem
    txt = txt.replace("// Gerado automaticamente de PNG",
                      f"// Original: {old}.c\n// Gerado automaticamente de PNG", 1)

    with open(new_path, "w") as f:
        f.write(txt)
    os.remove(old_path)
    return True

def atualizar_all():
    path = "src/sprites_all.c"
    with open(path) as f:
        txt = f.read()

    for old, new in MAP.items():
        # Includes
        txt = txt.replace(f'#include "sprites_data/{old}.c"',
                          f'#include "sprites_data/{new}.c"')
        # Entradas da tabela
        old_up, new_up = old.upper(), new.upper()
        txt = txt.replace(
            f'{{"{old}", {old}_pixels, {old_up}_W, {old_up}_H}}',
            f'{{"{new}", {new}_pixels, {new_up}_W, {new_up}_H}}'
        )
    with open(path, "w") as f:
        f.write(txt)

def atualizar_main():
    path = "src/main_test.c"
    if not os.path.exists(path):
        return
    with open(path) as f:
        txt = f.read()
    # Atualiza lookup do teste
    txt = txt.replace('sprite_lookup("tl_dante")', 'sprite_lookup("title_dante")')
    txt = txt.replace('sprite_lookup("title")', 'sprite_lookup("title_dante")')
    with open(path, "w") as f:
        f.write(txt)

# ── EXECUTA ──
print(f"Renomeando {len(MAP)} sprites...")
ok = 0
for old, new in MAP.items():
    if rename_arquivo(old, new):
        print(f"  {old:15s} -> {new}")
        ok += 1

print(f"\n{ok}/{len(MAP)} renomeados.")
print("Atualizando sprites_all.c...")
atualizar_all()
print("Atualizando main_test.c...")
atualizar_main()
print("\nPRONTO!")
