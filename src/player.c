#include "ksnd.h"
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include "../temp/data.inc"

#define SAMPLE_RATE 44100

// Edit this text to include your song name, your name etc.

#define SONG_INFO_TEXT "Song is now playing.\n\nAny key to stop playing."

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  KPlayer *player;
  KSong * song;

#if ENABLE_WAV_WRITER
  int is_wave_writer = 0;
  char output_file[80] = {0};

  // Enable wav writer if output file is specified

  if (sscanf(GetCommandLine(), "%*s %79s", output_file) == 1)
    is_wave_writer = 1;

  if (is_wave_writer)
    player = KSND_CreatePlayerUnregistered(SAMPLE_RATE);
  else
    player = KSND_CreatePlayer(SAMPLE_RATE);
#else
  player = KSND_CreatePlayer(SAMPLE_RATE);
#endif

  song = KSND_LoadSongFromMemory(player, data, sizeof(data));

  KSND_PlaySong(player, song, 0);

#if ENABLE_WAV_WRITER
  if (is_wave_writer)
    printf("Writing %s...\n", output_file);
  else
    puts(SONG_INFO_TEXT);
#else
  puts(SONG_INFO_TEXT);
#endif

  int p = 0, l =  KSND_GetSongLength(song), prev_p = 0;

#if ENABLE_WAV_WRITER
  short int buffer[16384];
  FILE *f = NULL;
  size_t riffsize = 0, chunksize = 0;

  // Write the RIFF header with temporary size (will be finalized later)
  // 44.1 kHz 16-bit stereo

  if (is_wave_writer)
  {
    f = fopen(output_file, "wb");

    const int channels = 2;
    unsigned int tmp = 0;

    fwrite("RIFF", 4, 1, f);

    riffsize = ftell(f);

    fwrite(&tmp, 4, 1, f);
    fwrite("WAVEfmt ", 8, 1, f);

    tmp = 16;

    fwrite(&tmp, 4, 1, f);

    tmp = 1;

    fwrite(&tmp, 2, 1, f);

    tmp = channels;

    fwrite(&channels, 2, 1, f);

    tmp = SAMPLE_RATE;

    fwrite(&tmp, 4, 1, f);

    tmp = SAMPLE_RATE * channels * sizeof(buffer[0]);

    fwrite(&tmp, 4, 1, f);

    tmp = channels * sizeof(buffer[0]);

    fwrite(&tmp, 2, 1, f);

    tmp = 16;

    fwrite(&tmp, 2, 1, f);

    fwrite("data", 4, 1, f);

    chunksize = ftell(f);

    tmp = 0;
    fwrite(&tmp, 4, 1, f);
  }
#endif

  while (1)
  {
    prev_p = p;
    p = KSND_GetPlayPosition(player);

    if (p >= l-1 || p < prev_p)
      break;

#if ENABLE_WAV_WRITER
    if (is_wave_writer)
    {
      KSND_FillBuffer(player, buffer, sizeof(buffer));
      fwrite(buffer, sizeof(buffer[0]), sizeof(buffer) / sizeof(buffer[0]), f);
    }
    else if (_kbhit())
      break;
#else
  if (_kbhit())
      break;
#endif
  }

#if ENABLE_WAV_WRITER
  // Finalize the wave header with the final file size

  if (is_wave_writer)
  {
    size_t sz = ftell(f) - 8;

    fseek(f, riffsize, SEEK_SET);
    fwrite(&sz, sizeof(sz), 1, f);

    sz = sz + 8 - (chunksize + 4);

    fseek(f, chunksize, SEEK_SET);

    fwrite(&sz, sizeof(sz), 1, f);

    fclose(f);
  }
#endif

  // Note: not everything is deinitialized, the process exits anyways.

  ExitProcess(0);

  return 0;
}
