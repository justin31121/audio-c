#define DECODER_IMPLEMENTATION
#include "../src/decoder.h"

#define AUDIO_IMPLEMENTATION
#include "../src/audio.h"

#include "../src/common.c"

// mingw: gcc -o audio audio.c -lole32 -lxaudio2_8 -lavformat -lavcodec -lavutil -lswresample
// msvc : cl audio.c ole32.lib avformat.lib avcodec.lib avutil.lib swresample.lib

#define DECODE_FROM_MEMORY

int main(int argc, char **argv) {

  const char *prog = argv[0];
  if(argc < 2) {
    fprintf(stderr, "ERROR: Please provide an argument!\n");
    fprintf(stderr, "USAGE: %s <audio-file>\n", prog);
    return 1;
  }
  const char *input = argv[1];

  int channels;
  int sample_rate;
  unsigned char *samples;
  unsigned int samples_count;

#ifdef DECODE_FROM_MEMORY

  unsigned char *data;
  uint32_t size;
  if(!slurp_file(input, &data, &size)) {
    error("Can not find file: '%s'\n", input);
  }

  if(!decoder_slurp_memory(data, size,
			   DECODER_FMT_S16, 1.0f,
			   &channels, &sample_rate,
			   &samples, &samples_count)) {
    error("Can not decode file: '%s'. Make sure it a valid audio-file\n", input);
  }

    
#else
  if(!decoder_slurp_file(input,
			 DECODER_FMT_S16, 1.0f,
			 &channels, &sample_rate,
			 &samples, &samples_count)) {
    error("Can not decode file: '%s'. Make sure it a valid audio-file\n", input);
  }

#endif


  Audio audio;
  if(!audio_init(&audio, AUDIO_FMT_S16, channels, sample_rate)) {
    error("Failed to initialize Audio!");
  }

  audio_play(&audio, samples, samples_count);

  audio_block(&audio);
  audio_free(&audio);

  free(samples);

#ifdef DECODE_FROM_MEMORY    
  free(data);    
#else
#endif
    
  return 0;
}
