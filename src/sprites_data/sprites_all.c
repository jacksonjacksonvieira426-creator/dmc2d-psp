// Auto-gerado — NAO EDITAR
#include <string.h>
#include "sprites_table.h"

#include "a0.c"
#include "add.c"
#include "b0.c"
#include "b1.c"
#include "b2.c"
#include "b3.c"
#include "bar.c"
#include "capcom_logo.c"
#include "d0.c"
#include "d1.c"
#include "d2.c"
#include "d3.c"
#include "h0.c"
#include "h1.c"
#include "h10.c"
#include "h11.c"
#include "h2.c"
#include "h3.c"
#include "h4.c"
#include "h5.c"
#include "h6.c"
#include "h7.c"
#include "h8.c"
#include "h9.c"
#include "icon.c"
#include "ig0.c"
#include "ig1.c"
#include "ig10.c"
#include "ig11.c"
#include "ig2.c"
#include "ig3.c"
#include "ig4.c"
#include "ig5.c"
#include "ig6.c"
#include "ig7.c"
#include "ig8.c"
#include "ig9.c"
#include "l1.c"
#include "mm.c"
#include "p0.c"
#include "s0.c"
#include "small.c"
#include "small_sp.c"
#include "title.c"
#include "tl_dante.c"
#include "tl_txt.c"
#include "ts0.c"
#include "ts1.c"
#include "ts2.c"
#include "ts3.c"
#include "v1.c"
#include "v2.c"
#include "wi_e.c"
#include "wi_r.c"

const SpriteEntry sprites_table[] = {
    {"a0", a0_pixels, A0_W, A0_H},
    {"add", add_pixels, ADD_W, ADD_H},
    {"b0", b0_pixels, B0_W, B0_H},
    {"b1", b1_pixels, B1_W, B1_H},
    {"b2", b2_pixels, B2_W, B2_H},
    {"b3", b3_pixels, B3_W, B3_H},
    {"bar", bar_pixels, BAR_W, BAR_H},
    {"capcom_logo", capcom_logo_pixels, CAPCOM_LOGO_W, CAPCOM_LOGO_H},
    {"d0", d0_pixels, D0_W, D0_H},
    {"d1", d1_pixels, D1_W, D1_H},
    {"d2", d2_pixels, D2_W, D2_H},
    {"d3", d3_pixels, D3_W, D3_H},
    {"h0", h0_pixels, H0_W, H0_H},
    {"h1", h1_pixels, H1_W, H1_H},
    {"h10", h10_pixels, H10_W, H10_H},
    {"h11", h11_pixels, H11_W, H11_H},
    {"h2", h2_pixels, H2_W, H2_H},
    {"h3", h3_pixels, H3_W, H3_H},
    {"h4", h4_pixels, H4_W, H4_H},
    {"h5", h5_pixels, H5_W, H5_H},
    {"h6", h6_pixels, H6_W, H6_H},
    {"h7", h7_pixels, H7_W, H7_H},
    {"h8", h8_pixels, H8_W, H8_H},
    {"h9", h9_pixels, H9_W, H9_H},
    {"icon", icon_pixels, ICON_W, ICON_H},
    {"ig0", ig0_pixels, IG0_W, IG0_H},
    {"ig1", ig1_pixels, IG1_W, IG1_H},
    {"ig10", ig10_pixels, IG10_W, IG10_H},
    {"ig11", ig11_pixels, IG11_W, IG11_H},
    {"ig2", ig2_pixels, IG2_W, IG2_H},
    {"ig3", ig3_pixels, IG3_W, IG3_H},
    {"ig4", ig4_pixels, IG4_W, IG4_H},
    {"ig5", ig5_pixels, IG5_W, IG5_H},
    {"ig6", ig6_pixels, IG6_W, IG6_H},
    {"ig7", ig7_pixels, IG7_W, IG7_H},
    {"ig8", ig8_pixels, IG8_W, IG8_H},
    {"ig9", ig9_pixels, IG9_W, IG9_H},
    {"l1", l1_pixels, L1_W, L1_H},
    {"mm", mm_pixels, MM_W, MM_H},
    {"p0", p0_pixels, P0_W, P0_H},
    {"s0", s0_pixels, S0_W, S0_H},
    {"small", small_pixels, SMALL_W, SMALL_H},
    {"small_sp", small_sp_pixels, SMALL_SP_W, SMALL_SP_H},
    {"title", title_pixels, TITLE_W, TITLE_H},
    {"tl_dante", tl_dante_pixels, TL_DANTE_W, TL_DANTE_H},
    {"tl_txt", tl_txt_pixels, TL_TXT_W, TL_TXT_H},
    {"ts0", ts0_pixels, TS0_W, TS0_H},
    {"ts1", ts1_pixels, TS1_W, TS1_H},
    {"ts2", ts2_pixels, TS2_W, TS2_H},
    {"ts3", ts3_pixels, TS3_W, TS3_H},
    {"v1", v1_pixels, V1_W, V1_H},
    {"v2", v2_pixels, V2_W, V2_H},
    {"wi_e", wi_e_pixels, WI_E_W, WI_E_H},
    {"wi_r", wi_r_pixels, WI_R_W, WI_R_H},
};

const SpriteEntry* sprite_lookup(const char* nome) {
    for (int i = 0; i < SPRITES_COUNT; i++)
        if (strcmp(sprites_table[i].nome, nome) == 0)
            return &sprites_table[i];
    return 0;
}
