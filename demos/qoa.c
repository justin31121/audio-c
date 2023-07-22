#include <stdio.h>

#define AUDIO_IMPLEMENTATION
#include "../src/audio.h"

#define QOA_IMPLEMENTATION
#include "../thirdparty/qoa.h"

#include "../src/common.c"

// mingw: gcc -o qoa qoa.c -lole32 -lxaudio2_8
// msvc : cl qoa.c ole32.lib

int main(int argc, char **argv) {

  const char *prog = argv[0];
  if(argc < 2) {
    fprintf(stderr, "ERROR: Please provide an argument!\n");
    fprintf(stderr, "USAGE: %s <qoa-file>\n", prog);
    return 1;
  }
  const char *input = argv[1];

  u8 *buffer;
  u32 buffer_size;
  if(!slurp_file(input, &buffer, &buffer_size)) {
    error("Can not find file: '%s'", input);
  }

  qoa_desc qoa;
  short *samples = qoa_decode(buffer, (s32) buffer_size, &qoa);
  if(!samples) {
    error("Can not decode file: '%s'. Make sure it is a valid 'qoa'-file!", input);
  }
  printf("channels: %u, sample_rate: %u, samples: %u\n",
	 qoa.channels,
	 qoa.samplerate,
	 qoa.samples);

  Audio audio;
  if(!audio_init(&audio, AUDIO_FMT_S16, qoa.channels, qoa.samplerate)) {
    error("Failed to initialize Audio!");
  }

  audio_play(&audio, (u8 *) samples, qoa.samples);
  audio_block(&audio);
  
  return 0;
}
