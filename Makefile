VERSION        = 0
VERSION_UPDATE = 0
VERSION_PATCH  = 1
CC             = g++
CFLAGS         = -g -std=c++17 -I/usr/include
O              = obj
SDIR           = src
LDFLAGS        = /usr/lib/libncurses.a /usr/lib/x86_64-linux-gnu/libpthread.a
LDLIBS         = -lmpg123
EXE            = nomadascii

.PHONY: all clean clean.exe clean.objs

ERRORS         = \
				-Werror=type-limits \
				-Werror=unused-but-set-parameter
DEFINES        = -D_NOMAD_VERSION=$(VERSION) \
				-D_NOMAD_VERSION_UPDATE=$(VERSION_UPDATE) \
				-D_NOMAD_VERSION_PATCH=$(VERSION_PATCH)

CFLAGS += $(DEFINES) $(INCLUDE) $(ERRORS)

OBJS= \
	$(O)/n_shared.o \
	$(O)/g_main.o \
	$(O)/g_game.o \
	$(O)/g_init.o \
	$(O)/scf.o \
	$(O)/scf_lexer.o \
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
	$(O)/s_world.o \

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -Ofast $(OBJS) $(LDFLAGS) -o $(EXE) $(LDLIBS)

$(O)/%.o: $(SDIR)/%.cpp
	$(CC) $(CFLAGS) -Wno-unused-result -Ofast -o $@ -c $<

clean:
	rm -rf $(O)/*
	rm $(EXE)
clean.exe:
	rm $(EXE)
clean.objs:
	rm -rf $(OBJS)