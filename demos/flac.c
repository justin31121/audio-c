#include "../src/common.c"

#define DR_FLAC_IMPLEMENTATION
#include "../thirdparty/dr_flac.h"

#define AUDIO_IMPLEMENTATION
#include "../src/audio.h"

int main(int argc, char **argv) {

  const char *prog = argv[0];
  if(argc < 2) {
    fprintf(stderr, "ERROR: Please provide an argument!\n");
    fprintf(stderr, "USAGE: %s <vorbis-file>\n", prog);
    return 1;
  }
  const char *input = argv[1];

  u8 *buffer;
  u32 size;
  if(!slurp_file(input, &buffer, &size)) {
    error("Can not find file: '%s'", input);
  }

  drflac *flac = drflac_open_memory(buffer, size, NULL);
  if(!flac) {
    error("Can not decode file: '%s'. Make sure it is a valid 'flac'-file!", input);
  }

  Audio audio;
  if(!audio_init(&audio, AUDIO_FMT_S16, flac->channels, flac->sampleRate)) {
    error("Failed to initialize Audio!");
  }

  short buf[2][1024 * 2];
  int current = 0;

  size_t n;
  while((n = drflac_read_pcm_frames_s16(flac, 1024, buf[current])) > 0) {
    audio_play(&audio, (unsigned char *) buf[current], n);
    current = 1 - current;
  }

  audio_block(&audio);
  audio_free(&audio);
  drflac_close(flac);
  free(buffer);
  
  return 0;
}
