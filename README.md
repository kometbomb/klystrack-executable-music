# klystrack-executable-music

This is a boilerplate for creating self-playing klystrack songs. The result is a binary that will use the the Windows native API to play music with libksnd. So far only Windows binaries are supported.

## Required stuff

* GCC, make etc.
* Recommended: [kkrunchy](http://www.farbrausch.de/~fg/kkrunchy/) will be used to compress the binary

## How to use

1. Clone this repo along with the klystron submodule (`git clone --recurse-submodules https://github.com/kometbomb/klystrack-executable-music`)
2. Copy your song file in the directory or specify the path to it in the enviroment variable SONGFILE
3. `make` (or e.g. `SONGFILE=/path/to/my_song.kt make`)
4. You should have your executable file in the `bin/` directory along with the kkrunchy compressed version

Feel free to edit `src/player.c` for your needs.

## WAV writer

The player can be compiled with the ENABLE_WAV_WRITER define set to 1 (see the makefile) which enables a simple WAV writer. The song binary file can then be run with an extra argument that is the destination wave file path. E.g. `song.exe wavefile.wav`.

## Advanced use

You will need to fiddle with the ksnd compilation options to disable any unused player features to save bytes. These can be edited in the makefile. See [the manual](https://github.com/kometbomb/klystron/wiki/libksnd#making-the-lib-smaller) for a list of these options. You need to `make clean` if you want to recompile libksnd with the new options.

You may want to checkout an older version of klystron in case there are backwards compatibility issues.
