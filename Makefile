VERSION        = 0
VERSION_UPDATE = 0
VERSION_PATCH  = 1
CC             = g++
CFLAGS         = -g -std=c++17
O              = obj
SDIR           = src
INCLUDE        = -IFiles/gamedata/DEPS/include -I/usr/include
LDFLAGS        = Files/gamedata/DEPS/lib/libncurses.a -L/usr/lib/x86_64-linux-gnu
LDLIBS         = -lpthread -lasound
ifndef debug
EXE            = nomadascii
else
EXE            = nomadascii_debug
endif

.PHONY: all clean clean.exe clean.objs clean.dbg

ERRORS         = \
				-Werror=type-limits \
				-Werror=unused-but-set-parameter
DEFINES        = -D_NOMAD_VERSION=$(VERSION) \
				-D_NOMAD_VERSION_UPDATE=$(VERSION_UPDATE) \
				-D_NOMAD_VERSION_PATCH=$(VERSION_PATCH)

CFLAGS += $(DEFINES) $(INCLUDE) $(ERRORS)

DEBUG= \
	$(O)/n_shared.debug.o \
	$(O)/g_main.debug.o \
	$(O)/g_game.debug.o \
	$(O)/g_init.debug.o \
	$(O)/scf.debug.o \
	$(O)/s_saveg.debug.o \
	$(O)/g_zone.debug.o \
	$(O)/p_playr.debug.o \
	$(O)/p_common.debug.o \
	$(O)/info.debug.o \
	$(O)/s_mmisc.debug.o \
	$(O)/m_hud.debug.o \
	$(O)/m_inventory.debug.o \
	$(O)/p_physics.debug.o \
	$(O)/g_loop.debug.o \
	$(O)/s_behave.debug.o \
	$(O)/g_rng.debug.o \
	$(O)/m_tuilib.debug.o \
	$(O)/s_enemy.debug.o \
	$(O)/g_math.debug.o \
	$(O)/s_mthink.debug.o \
	$(O)/s_nomads.debug.o \
	$(O)/g_combat.debug.o \
	$(O)/g_items.debug.o \
	$(O)/s_mission.debug.o \
#	$(O)/c_dungen.debug.o \
#	$(O)/c_nemsis.debug.o \
#	$(O)/c_sao.debug.o \

OBJS= \
	$(O)/n_shared.o \
	$(O)/g_main.o \
	$(O)/g_game.o \
	$(O)/g_init.o \
	$(O)/scf.o \
	$(O)/s_saveg.o \
	$(O)/g_zone.o \
	$(O)/p_playr.o \
	$(O)/p_common.o \
	$(O)/info.o \
	$(O)/s_mmisc.o \
	$(O)/m_hud.o \
	$(O)/m_inventory.o \
	$(O)/p_physics.o \
	$(O)/g_loop.o \
	$(O)/s_behave.o \
	$(O)/g_rng.o \
	$(O)/m_tuilib.o \
	$(O)/s_enemy.o \
	$(O)/g_math.o \
	$(O)/s_mthink.o \
	$(O)/s_nomads.o \
	$(O)/g_combat.o \
	$(O)/g_items.o \
	$(O)/s_mission.o \
#	$(O)/c_dungen.o \
#	$(O)/c_nemsis.o \
#	$(O)/c_sao.o \

ifndef replit
OBJS += $(O)/g_sound.o
DEBUG += $(O)/g_sound.debug.o
endif

ifndef debug
$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -Ofast $(OBJS) $(LDFLAGS) -o $(EXE) $(LDLIBS)
else
$(EXE): $(DEBUG)
	$(CC) $(CFLAGS) -Og $(DEBUG) $(LDFLAGS) -o $(EXE) $(LDLIBS)
endif

$(O)/%.o: $(SDIR)/%.cpp
	$(CC) $(CFLAGS) -Wno-unused-result -Ofast -o $@ -c $<
$(O)/%.debug.o: $(SDIR)/%.cpp
	$(CC) $(CFLAGS) -Wall -D_NOMAD_DEBUG -Og -o $@ -c $<

clean:
	rm -rf $(O)/*
	rm $(EXE)
	rm nomadascii_debug
clean.exe:
	rm $(EXE)
clean.objs:
	rm -rf $(OBJS)
clean.dbg:
	rm -f $(DEBUG)
	rm nomadascii_debug