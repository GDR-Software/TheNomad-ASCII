# The Nomad (ASCII)
The Nomad ASCII is a project that I have been developing for a long time. This is the current "working" repository. I'm occasionally pulling code from my local repo, but its somewhat new code.

 The game is compiled for an x86_64 arch, will be compiled on an i586 and riscv64 and x64 arch's in the future

__NOTICE__: DO NOT DOWNLOAD THIS REPOSITORY!!! IT IS STILL IN DEVELOPMENT!!!!

## Building it Yourself
The following dependencies are required:
- ncurses-6.3
- pthreads (posix-threads)

So you'll need a static library of ncurses-6.3, and with a custom configuration, so follow these steps to set up this game for your own stuff:

```
wget https://ftp.gnu.org/gnu/ncurses/ncurses-6.3.tar.gz
tar -xf ncurses-6.3.tar.gz && cd ncurses-6.3
```

next run ./configure in the ncurses directory with these options, otherwise, the game will not run:
(also make sure to make the prefix your pwd)

```
./configure pwd/Files/gamedata/DEPS --without-curses-h --with-exp-win32 --with-sp-funcs --with-pthread --enable-expanded --enable-assertions --enable-ext-putwin --enable-wattr-macros --disable-leaks --with-dmalloc --with-chtype=uint16_t --enable-sigwinch --with-normal --with-debug --with-profile
```

Then run make install. Then also do the following:
```
cd include && make install
./MKkey_defs.sh && ./MKhashsize.sh ./MKparametrized ./MKncurses_def.sh
cd ../ncurses && make install
```

After building ncurses, cd back into the main game directory, and run make, if you don't have an arch that is one of the following: x86_64, x86, x64, i586, i386, i486, i686, or riscv64, you'll have to use either -D_NOMAD_32 or -D_NOMAD_64 with the makefile depending on what your own PC's arch is. Consult n_shared.h if you are wondering the sizes and types of this game.

## Current Build
The current working game has the following issues that will be addressed after the game is completed (as they aren't game-breaking):
- BFF file parser can't yet load in custom mobs or bosses
- game is somewhat slower than I would like it to be
- debug file logging

## Guide To The Code's Layout
So, as you may have noticed, the files are labeled with various prefixes, here's how they work:
- g_* this is general game code used by almost everything
- m_* this is map code, so whatever is visual, such as the HUD, biomes, and pints
- c_* this is the campaign code, so the code only used in the campaign
- p_* this is the entity, used really only in the level playloop. this includes physics, AI, and stuff like that

## The Replit
Yeah, I'm writing this on a replit, partly because I don't have a PC at the moment, and also due to the philosophy: if the thing can run on a replit, it can run almost anywhere.
