
#include <stdio.h>

#include "../thirdparty/stb_vorbis.c"

#define AUDIO_IMPLEMENTATION
#include "../src/audio.h"

#include "../src/common.c"

char tmp_buffer[1024 * 1024];

int main(int argc, char **argv) {

  const char *prog = argv[0];
  if(argc < 2) {
    fprintf(stderr, "ERROR: Please provide an argument!\n");
    fprintf(stderr, "USAGE: %s <vorbis-file>\n", prog);
    return 1;
  }
  const char *input = argv[1];

  u8 *buf;
  u32 size;
  if(!slurp_file(input, &buf, &size)) {
    error("Can not find file: '%s'", input);
  }

  stb_vorbis_alloc alloc = { tmp_buffer, sizeof(tmp_buffer)};

  int error;
  stb_vorbis *v = stb_vorbis_open_memory(buf, size, &error, &alloc);
  if(!v) {
    if(error == VORBIS_outofmem) {
      panic("out of memory");
    }
    error("Can not decode file: '%s'. Make sure it is a valid 'vorbis'-file!", input);
  }
  

  Audio audio;
  if(!audio_init(&audio, AUDIO_FMT_S16, v->channels, v->sample_rate)) return 1;

  short outputs[2][1024];
  int current = 0;

  int n;
  while(( n = stb_vorbis_get_samples_short_interleaved(v, v->channels, outputs[current], 1024))) {
    audio_play(&audio, (unsigned char *)outputs[current], n);
    current = 1 - current;
  }

  audio_block(&audio);
  audio_free(&audio);
  stb_vorbis_close(v);
  free(buf);
  
  return 0;
}
