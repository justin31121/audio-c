#define DECODER_IMPLEMENTATION
#include "../src/decoder.h"

#define AUDIO_IMPLEMENTATION
#include "../src/audio.h"

#include "../src/common.c"

// mingw: gcc -o audio audio.c -lole32 -lxaudio2_8 -lavformat -lavcodec -lavutil -lswresample
// msvc : cl audio.c ole32.lib avformat.lib avcodec.lib avutil.lib swresample.lib

//#define DECODE_FROM_MEMORY

int main(int argc, char **argv) {

  const char *prog = argv[0];
  if(argc < 2) {
    fprintf(stderr, "ERROR: Please provide an argument!\n");
    fprintf(stderr, "USAGE: %s <audio-file>\n", prog);
    return 1;
  }
  const char *input = argv[1];

  Decoder_Read read_func  = NULL;
  Decoder_Seek seek_func  = NULL;
  void *opaque  = NULL;

#ifdef DECODE_FROM_MEMORY

  unsigned char *data;
  uint32_t size;
  if(!slurp_file(input, &data, &size)) {
    error("Can not find file: '%s'\n", input);
  }

  Decoder_Memory memory = {
    .data = data,
    .pos = 0,
    .size = size,
  };

  read_func = decoder_memory_read;
  seek_func = decoder_memory_seek;
  opaque = &memory;
    
#else
    
  FILE *f = fopen(input, "rb");
  if(!f) {
    error("Can not find file: '%s'\n", input);
  }

  read_func = decoder_file_read;
  seek_func = decoder_file_seek;
  opaque = f;
    
#endif 

  Decoder decoder;
  int channels;
  int sample_rate;
  if(!decoder_init(&decoder,
		   read_func, seek_func, opaque,		     
		   DECODER_FMT_S16, 1.0f, 1152,
		   &channels, &sample_rate)) {
    error("Can not decode file: '%s'. Make sure it a valid audio-file\n", input);
  }

  Audio audio;
  if(!audio_init(&audio, AUDIO_FMT_S16, channels, sample_rate)) {
    error("Failed to initialize Audio!");
  }

  unsigned char buffer[2][1152 * 4 * 2];
  int current = 0;
  int samples = 0;

  int out_samples;
  while(decoder_decode(&decoder, &out_samples, buffer[current] + samples * audio.sample_size)) {
    samples += out_samples;
    if(samples > 1024) {
      audio_play(&audio, buffer[current], samples);
      current = 1 - current;
      samples = 0;
    }
  }

  audio_block(&audio);
  audio_free(&audio);
  decoder_free(&decoder);

#ifdef DECODE_FROM_MEMORY    
  free(data);    
#else
  fclose(f);
#endif    
    
  return 0;
}
