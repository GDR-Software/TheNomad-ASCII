VERSION        = 0
VERSION_UPDATE = 0
VERSION_PATCH  = 1
CC             = g++
CFLAGS         = -g -std=c++17 -Wall
O              = obj
SDIR           = src
INCLUDE        = -IFiles/gamedata/DEPS/include
LDFLAGS        = Files/gamedata/DEPS/lib/libncurses.a -lpthread
EXE            = nomadascii

.PHONY: all clean clean.exe clean.objs

ERRORS         = \
				-Werror=type-limits \
				-Werror=uninitialized \
				-Werror=unused-but-set-parameter \
				-Wno-unused-result
DEFINES        = -D_NOMAD_VERSION=$(VERSION) \
				-D_NOMAD_VERSION_UPDATE=$(VERSION_UPDATE) \
				-D_NOMAD_VERSION_PATCH=$(VERSION_PATCH) \

CFLAGS += $(DEFINES) $(INCLUDE)

all: $(EXE)

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
#	$(O)/c_dungen.o \
#	$(O)/c_nemsis.o \
#	$(O)/c_sao.o \

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -Ofast $(OBJS) $(LDFLAGS) -o $(EXE)

$(O)/%.o: $(SDIR)/%.cpp
	$(CC) $(CFLAGS) $(ERRORS) -Ofast -o $@ -c $<
$(O)/%.o: $(SDIR)/cardinal/%.cpp
	$(CC) $(CFLAGS) $(ERRORS) -Ofast -o $@ -c $<

clean:
	rm -rf $(O)/*
	rm $(EXE)
clean.exe:
	rm $(EXE)
clean.objs:
	rm -rf $(O)/*