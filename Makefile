VERSION        = 0
VERSION_UPDATE = 0
VERSION_PATCH  = 1
CC             = g++
CFLAGS         = -g -std=c++17 -Wall
O              = obj
SDIR           = src
INCLUDE        = -IFiles/gamedata/DEPS/include
LDFLAGS        = Files/gamedata/DEPS/lib/libncurses.a
EXE            = nomadascii

ERRORS         = \
				-Werror=type-limits \
				-Werror=uninitialized \
				-Werror=unused-but-set-parameter \
				-Wno-unused-result
DEFINES        = -D_NOMAD_VERSION=$(VERSION) \
				-D_NOMAD_VERSION_UPDATE=$(VERSION_UPDATE) \
				-D_NOMAD_VERSION_PATCH=$(VERSION_PATCH)

OBJS= \
	$(O)/n_shared.o \
	$(O)/g_main.o \
	$(O)/g_game.o \
	$(O)/g_init.o \
	$(O)/scf.o \
	$(O)/c_saveg.o \
	$(O)/g_zone.o \
	$(O)/p_playr.o \
	$(O)/p_common.o \
	$(O)/info.o \
	$(O)/c_mmisc.o \
	$(O)/m_hud.o \
	$(O)/m_inventory.o \
	$(O)/p_physics.o \
	$(O)/m_biome.o \
	$(O)/g_loop.o \
	$(O)/c_behave.o \
	$(O)/g_rng.o \
	$(O)/m_tuilib.o \

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -Os $(OBJS) $(LDFLAGS) -o $(EXE)

$(O)/%.o: $(SDIR)/%.cpp
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDE) $(ERRORS) -Ofast -o $@ -c $<