# DMC 2D - Manual

## Status
- ✅ 151 métodos extraídos
- ✅ 54 sprites renomeados (nomes semânticos)
- ✅ Pipeline PSP → PPSSPP validado
- ✅ Dante renderizado com cores corretas

## Métodos traduzidos
- c.run() — game loop (estrutura)
- c.a(int) — dispatcher bitmask
- c.d(int) — sprite loader por direção
- c.p() — save RMS → arquivo
- c.a(Z) — reset/pause
- c.b() — cleanup thread (stub)
- c.d() — reset de partida

## Formato cor
PNG (ARGB) → fix_cor() → PSP (ABGR)

## Próximo
Traduzir c.keyPressed(int) — input real
