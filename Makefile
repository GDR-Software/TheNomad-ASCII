VERSION        = 0
VERSION_UPDATE = 1
VERSION_PATCH  = 2
ADDFLAGS       =
CC             = g++
CFLAGS         = -std=c++17 -Isrc -finline-limit=10000000 # kinda stupid, yes, but SPEED IS KEY
CFLAGS += $(ADDFLAGS)
CFLAGS       += -I/usr/include -I/usr/local/include
LIBDIR        = /usr/lib/x86_64-linux-gnu
LDFLAGS       = /usr/lib/libmenu.a -LFiles/deps $(LIBDIR)/libpthread.a $(LIBDIR)/libogg.a -lsndfile /usr/local/lib/libopenal.a /usr/lib/libncurses.a \
				-lboost_thread -lboost_chrono
O              = obj
SDIR           = src

.PHONY: all clean clean.exe clean.objs

ERRORS         = -Werror=type-limits \
				-Werror=overflow \
				-Werror=return-type \

DEFINES        = -D_NOMAD_VERSION=$(VERSION) \
				-D_NOMAD_VERSION_UPDATE=$(VERSION_UPDATE) \
				-D_NOMAD_VERSION_PATCH=$(VERSION_PATCH) \

CFLAGS += $(DEFINES) $(INCLUDE) $(ERRORS)

ifeq ($(build),debug)
COMPILE_CC= $(CC) $(CFLAGS) -Og -g -o $@ -c $<
COMPILE_EXE= $(CC) $(CFLAGS) -Og -g $(OBJS) -o $(EXE) $(LDFLAGS)
EXE= nomadascii_debug
else
COMPILE_CC= $(CC) $(CFLAGS) -Ofast -s -o $@ -c $<
COMPILE_EXE= $(CC) $(CFLAGS) -Ofast -s $(OBJS) -o $(EXE) $(LDFLAGS)
EXE= nomadascii
endif

all: $(EXE)

ifneq ($(build),debug)
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
	$(O)/scf_lexer.o
else
OBJS= \
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
	$(O)/scf_lexer.debug.o
endif

$(O)/%.o: $(SDIR)/%.cpp
	$(COMPILE_CC)
$(O)/%.debug.o: $(SDIR)/%.cpp
	$(COMPILE_CC)
$(EXE): $(OBJS)
	$(COMPILE_EXE)

clean:
	rm $(OBJS)
	rm $(EXE)
	rm n_pch.h.gch
clean.exe:
	rm $(EXE)
clean.objs:
	rm -rf $(OBJS)