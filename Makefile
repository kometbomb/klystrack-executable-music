# The source klystrack song file

SONGFILE ?= song.kt

# The resulting binary will be written to these two files

EXECFILE ?= bin/song.exe
COMPRESSEDEXEC ?= bin/song_compressed.exe

# klystron PROFILE should be "size" for size optimized binary but you can change it here

PROFILE = size
EXTFLAGS = -nostdlib -s -DUSESDLMUTEXES -DSTEREOOUTPUT -DUSENATIVEAPIS

# Uncomment the below line to enable the WAV writer

#EXTFLAGS += -DENABLE_WAV_WRITER=1

# Here are the feature flags you can use to disable unused features to minimize the size.

# Make the wavetable use lower resolution
#EXTFLAGS += -DLOWRESWAVETABLE

# Disable almost everything
#EXTFLAGS += -DCYD_DISABLE_WAVETABLE -DCYD_DISABLE_FX -DCYD_DISABLE_LFSR \
	-DCYD_DISABLE_MULTIPLEX -DCYD_DISABLE_BUZZ -DCYD_DISABLE_FILTER \
	-DCYD_DISABLE_VIBRATO -DCYD_DISABLE_PWM -DCYD_DISABLE_ENVELOPE \
	-DCYD_DISABLE_INACCURACY

# You need kkrunchy for the compressed binary (http://www.farbrausch.de/~fg/kkrunchy/)
# If it's not available, the step will be skipped and the song will not be compressed

KKRUNCHY := kkrunchy_k7.exe -new
LIBS := -lwinmm -lmsvcrt -lgcc -lkernel32 -luser32
EXTFLAGS += $(LIBS)

all: $(EXECFILE) $(COMPRESSEDEXEC)
	@echo "Song binary compiled"
	@echo " - Binary written to '"$(EXECFILE)"'"
	@echo " - Compressed binary written to '"$(COMPRESSEDEXEC)"'"

$(COMPRESSEDEXEC): $(EXECFILE)
	@cp $(EXECFILE) $(COMPRESSEDEXEC)
	@$(KKRUNCHY) $(COMPRESSEDEXEC)

$(EXECFILE): src/player.c temp/data.inc
	@mkdir -p bin
	@make -C ./klystron ksnd CFG=$(PROFILE) EXTFLAGS="$(EXTFLAGS)"
	@gcc -Os $(EXTFLAGS) -o $(EXECFILE) src/player.c -lksndstatic -Wall $(LIBS) -I ./klystron/src/lib -L ./klystron/bin.$(PROFILE)

temp/data.inc: temp/bin2c.exe $(SONGFILE)
	@mkdir -p temp
	@echo "static unsigned char data[] = {" > temp/data.inc
	@temp/bin2c.exe $(SONGFILE) >> temp/data.inc
	@echo "};" >> temp/data.inc

clean:
	@make -C ./klystron clean
	@rm -rf bin temp $(COMPRESSEDEXEC) $(EXECFILE)

temp/bin2c.exe: src/bin2c.c
	@mkdir -p temp
	@gcc -Wall src/bin2c.c -o temp/bin2c.exe
