VERSION        = 0
VERSION_UPDATE = 1
VERSION_PATCH  = 2
CC             = g++
CFLAGS         = -std=c++17 -I/usr/include -I/usr/local/include
LDFLAGS        = /usr/lib/libmenu.a \
				/usr/lib/x86_64-linux-gnu/libpthread.a -lsndfile -lopenal
O              = obj
SDIR           = src
EXE            = nomadascii
EXE_DEBUG      = nomadascii_debug


ifndef debug
CFLAGS += -DRELEASE
LDFLAGS += /usr/lib/libncurses.a
else
LDFLAGS += /usr/lib/libncurses_g.a
endif

.PHONY: all clean clean.exe clean.objs clean.debug

OPTIMIZERS     = -finline-limit=10000 \
				-ffast-math \
				-frounding-math \

ERRORS         = -Werror=type-limits \
				-Werror=overflow \
				-Werror=return-type \

DEFINES        = -D_NOMAD_VERSION=$(VERSION) \
				-D_NOMAD_VERSION_UPDATE=$(VERSION_UPDATE) \
				-D_NOMAD_VERSION_PATCH=$(VERSION_PATCH)

CFLAGS += $(DEFINES) $(INCLUDE) $(ERRORS) $(OPTIMIZERS)

OBJS= \
	$(O)/g_animation.o \
	$(O)/g_bff.o \
	$(O)/g_combat.o \
	$(O)/g_game.o \
	$(O)/g_init.o \
	$(O)/g_items.o \
	$(O)/g_loop.o \
	$(O)/g_main.o \
	$(O)/g_math.o \
	$(O)/g_rng.o \
	$(O)/g_sound.o \
	$(O)/g_zone.o \
	$(O)/info.o \
	$(O)/m_hud.o \
	$(O)/m_tuilib.o \
	$(O)/m_inventory.o \
	$(O)/n_cheats.o \
	$(O)/n_shared.o \
	$(O)/p_common.o \
	$(O)/p_physics.o \
	$(O)/p_playr.o \
	$(O)/s_mmisc.o \
	$(O)/s_campaign.o \
	$(O)/s_mthink.o \
	$(O)/s_saveg.o \
	$(O)/s_world.o \
	$(O)/scf.o \
	$(O)/scf_lexer.o \

DEBUG= \
	$(O)/g_animation.debug.o \
	$(O)/g_bff.debug.o \
	$(O)/g_combat.debug.o \
	$(O)/g_game.debug.o \
	$(O)/g_init.debug.o \
	$(O)/g_items.debug.o \
	$(O)/g_loop.debug.o \
	$(O)/g_main.debug.o \
	$(O)/g_math.debug.o \
	$(O)/g_rng.debug.o \
	$(O)/g_sound.debug.o \
	$(O)/g_zone.debug.o \
	$(O)/info.debug.o \
	$(O)/m_hud.debug.o \
	$(O)/m_tuilib.debug.o \
	$(O)/m_inventory.debug.o \
	$(O)/n_cheats.debug.o \
	$(O)/n_shared.debug.o \
	$(O)/p_common.debug.o \
	$(O)/p_physics.debug.o \
	$(O)/p_playr.debug.o \
	$(O)/s_mmisc.debug.o \
	$(O)/s_campaign.debug.o \
	$(O)/s_mthink.debug.o \
	$(O)/s_saveg.debug.o \
	$(O)/s_world.debug.o \
	$(O)/scf.debug.o \
	$(O)/scf_lexer.debug.o \

ifdef debug
all: $(EXE_DEBUG)
else
all: $(EXE)
endif

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -s -Ofast $(OBJS) -o $(EXE) $(LDLIBS) $(LDFLAGS)
$(EXE_DEBUG): $(DEBUG)
	$(CC) $(CFLAGS) -Wall -Og -g $(DEBUG) $(LDFLAGS) -o $(EXE_DEBUG) $(LDLIBS)

$(O)/%.o: $(SDIR)/%.cpp
	$(CC) $(CFLAGS) -s -Wno-unused-result -Ofast -o $@ -c $<
$(O)/%.debug.o: $(SDIR)/%.cpp
	$(CC) $(CFLAGS) -Wpedantic -Og -g -o $@ -c $<

clean:
	rm -rf $(O)/*
	rm $(EXE)
clean.exe:
	rm $(EXE)
clean.debug:
	rm $(EXE_DEBUG)
	rm -rf $(DEBUG)
clean.objs:
	rm -rf $(OBJS)