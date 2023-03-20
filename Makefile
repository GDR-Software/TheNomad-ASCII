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

ifdef release
COMPILE_CC= $(CC) $(CFLAGS) -Ofast -s -o $@ -c $<
COMPILE_EXE= $(CC) $(CFLAGS) -Ofast -s $(OBJS) -o $(EXE) $(LDFLAGS)
EXE= nomadascii
endif
ifdef debug
COMPILE_CC= $(CC) $(CFLAGS) -Og -g -o $@ -c $<
COMPILE_EXE= $(CC) $(CFLAGS) -Og -g $(OBJS) -o $(EXE) $(LDFLAGS)
EXE= nomadascii_debug
endif

all: $(EXE)

ifdef release
OBJS= \
	$(O)/g_animation.$(SUFFIX).o \
	$(O)/g_bff.$(SUFFIX).o \
	$(O)/g_combat.$(SUFFIX).o \
	$(O)/g_game.$(SUFFIX).o \
	$(O)/g_init.$(SUFFIX).o \
	$(O)/g_items.$(SUFFIX).o \
	$(O)/g_loop.$(SUFFIX).o \
	$(O)/g_main.$(SUFFIX).o \
	$(O)/g_math.$(SUFFIX).o \
	$(O)/g_rng.$(SUFFIX).o \
	$(O)/g_sound.$(SUFFIX).o \
	$(O)/g_zone.$(SUFFIX).o \
	$(O)/info.$(SUFFIX).o \
	$(O)/m_hud.$(SUFFIX).o \
	$(O)/m_tuilib.$(SUFFIX).o \
	$(O)/m_inventory.$(SUFFIX).o \
	$(O)/n_cheats.$(SUFFIX).o \
	$(O)/n_shared.$(SUFFIX).o \
	$(O)/p_common.$(SUFFIX).o \
	$(O)/p_physics.$(SUFFIX).o \
	$(O)/p_playr.$(SUFFIX).o \
	$(O)/s_mmisc.$(SUFFIX).o \
	$(O)/s_campaign.$(SUFFIX).o \
	$(O)/s_mthink.$(SUFFIX).o \
	$(O)/s_saveg.$(SUFFIX).o \
	$(O)/s_world.$(SUFFIX).o \
	$(O)/scf.$(SUFFIX).o \
	$(O)/scf_lexer.$(SUFFIX).o
endif
ifdef debug
OBJS= \
	$(O)/g_animation.debug.$(SUFFIX).o \
	$(O)/g_bff.debug.$(SUFFIX).o \
	$(O)/g_combat.debug.$(SUFFIX).o \
	$(O)/g_game.debug.$(SUFFIX).o \
	$(O)/g_init.debug.$(SUFFIX).o \
	$(O)/g_items.debug.$(SUFFIX).o \
	$(O)/g_loop.debug.$(SUFFIX).o \
	$(O)/g_main.debug.$(SUFFIX).o \
	$(O)/g_math.debug.$(SUFFIX).o \
	$(O)/g_rng.debug.$(SUFFIX).o \
	$(O)/g_sound.debug.$(SUFFIX).o \
	$(O)/g_zone.debug.$(SUFFIX).o \
	$(O)/info.debug.$(SUFFIX).o \
	$(O)/m_hud.debug.$(SUFFIX).o \
	$(O)/m_tuilib.debug.$(SUFFIX).o \
	$(O)/m_inventory.debug.$(SUFFIX).o \
	$(O)/n_cheats.debug.$(SUFFIX).o \
	$(O)/n_shared.debug.$(SUFFIX).o \
	$(O)/p_common.debug.$(SUFFIX).o \
	$(O)/p_physics.debug.$(SUFFIX).o \
	$(O)/p_playr.debug.$(SUFFIX).o \
	$(O)/s_mmisc.debug.$(SUFFIX).o \
	$(O)/s_campaign.debug.$(SUFFIX).o \
	$(O)/s_mthink.debug.$(SUFFIX).o \
	$(O)/s_saveg.debug.$(SUFFIX).o \
	$(O)/s_world.debug.$(SUFFIX).o \
	$(O)/scf.debug.$(SUFFIX).o \
	$(O)/scf_lexer.debug.$(SUFFIX).o
endif

$(O)/%.$(SUFFIX).o: $(SDIR)/%.cpp
	$(COMPILE_CC)
$(O)/%.debug.$(SUFFIX).o: $(SDIR)/%.cpp
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