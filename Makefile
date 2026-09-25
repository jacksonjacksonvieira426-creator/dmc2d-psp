TARGET = dmc2d_map
OBJS = src/test_mapa.o

CFLAGS = -O2 -G0 -Wall -I src
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)
LIBS = -lpspdisplay -lpspge -lpspctrl -lpspvfpu -lpsppower -lpsprtc -lm -lc

BUILD_PRX = 1
EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = DMC2D Test

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
