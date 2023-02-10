VERSION        = 0
VERSION_UPDATE = 0
VERSION_PATCH  = 1
ifndef compile_clang
ifndef win32
CC             = g++
else
CC             = wine ../mingw32/bin/i686-w64-mingw32-g++.exe
endif
else
ifdef win32
CC             = wine ../mingw32/bin/clang++.exe
else
endif
endif
ifndef win32
ifndef replit
CFLAGS         = -std=c++17 -I/usr/include
else
CFLAGS         = -std=c++17 -IFiles/gamedata/DEPS/include/ -DREPLIT
endif
else
CFLAGS         = -std=c++17 -I../mingw32/include -I/home/noah/Downloads/include
endif
O              = obj
SDIR           = src
ifndef win32
ifndef replit
LDFLAGS        = /usr/lib/libmenu.a /usr/lib/libncurses.a \
				/usr/lib/x86_64-linux-gnu/libpthread.a -lmpg123
else
LDFLAGS        = Files/gamedata/DEPS/lib/libmenu.a Files/gamedata/DEPS/lib/libncurses.a -lpthread
endif
else
LDFLAGS        =  ../mingw32/bin/libwinpthread-1.dll ../mingw32/bin/libncursesw6.dll libmenuw.dll.a
endif
ifndef win32
EXE            = nomadascii
EXE_DEBUG      = nomadascii_debug
else
EXE            = nomadascii.exe
EXE_DEBUG      = nomadascii_debug.exe
endif

ifndef debug
CFLAGS += -DRELEASE
endif

.PHONY: all clean clean.exe clean.objs clean.debug

ERRORS         = \
				-Werror=type-limits 
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
	$(O)/g_math.o \
	$(O)/s_mthink.o \
	$(O)/s_nomads.o \
	$(O)/g_combat.o \
	$(O)/g_items.o \
	$(O)/s_mission.o \
	$(O)/s_world.o \
	$(O)/g_loadbff.o \

DEBUG= \
	$(O)/n_shared.debug.o \
	$(O)/g_main.debug.o \
	$(O)/g_game.debug.o \
	$(O)/g_init.debug.o \
	$(O)/scf.debug.o \
	$(O)/scf_lexer.debug.o \
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
	$(O)/g_math.debug.o \
	$(O)/s_mthink.debug.o \
	$(O)/s_nomads.debug.o \
	$(O)/g_combat.debug.o \
	$(O)/g_items.debug.o \
	$(O)/s_mission.debug.o \
	$(O)/s_world.debug.o \
	$(O)/g_loadbff.debug.o \

ifdef debug
all: $(EXE_DEBUG)
else
all: $(EXE)
endif

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -Ofast $(OBJS) $(LDFLAGS) -o $(EXE) $(LDLIBS)
$(EXE_DEBUG): $(DEBUG)
	$(CC) $(CFLAGS) -Wall -Og -g $(DEBUG) $(LDFLAGS) -o $(EXE_DEBUG) $(LDLIBS)

$(O)/%.o: $(SDIR)/%.cpp
	$(CC) $(CFLAGS) -Wno-unused-result -Ofast -o $@ -c $<
$(O)/%.debug.o: $(SDIR)/%.cpp
	$(CC) $(CFLAGS) -Wall -Og -g -o $@ -c $<

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