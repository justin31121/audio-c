#include <stdio.h>

#define MF_DECODER_IMPLEMENTATION
#include "../src/mf_decoder.h"

#define AUDIO_IMPLEMENTATION
#include "../src/audio.h"

#include "../src/common.c"

// mingw: gcc -o win32 win32.c -lmf -lmfplat -lmfuuid -lmfreadwrite -lole32 -lxaudio2_8
// msvc : cl win32.c ole32.lib mf.lib mfplat.lib mfuuid.lib mfreadwrite.lib

int main(int argc, char **argv) {

  const char *prog = argv[0];
  if(argc < 2) {
    fprintf(stderr, "ERROR: Please provide an argument!\n");
    fprintf(stderr, "USAGE: %s <audio-file>\n", prog);
    return 1;
  }
  const char *input = argv[1];

    
  MF_Decoder decoder;
  int channels, sample_rate;

#ifdef DECODER_FROM_MEMORY
  u8 *buffer;
  u32 size;
  if(!slurp_file(input, &buffer, &size)) {
    error("Can not load file: '%s'", input);
  }

  if(!mf_decoder_init_memory(&decoder, buffer, size, MF_DECODER_FMT_S16,
			     &channels, &sample_rate)) {
    fprintf(stderr,
	    "ERROR: Can not decode file: '%s'\n"
	    "       Make sure the file is an audio file\n",  input);

    return 1;
  }
#else    
  if(!mf_decoder_init(&decoder, input, MF_DECODER_FMT_S16,
		      &channels, &sample_rate)) {
    fprintf(stderr,
	    "ERROR: Can not decode file: '%s'\n"
	    "       Make sure the filepath is valid\n"
	    "       and the file is an audio file.\n",  input);
    return 1;
  }
#endif

  printf("channels: %d, sample_rate: %d\n", channels, sample_rate);

  Audio audio;
  if(!audio_init(&audio, AUDIO_FMT_S16, channels, sample_rate)) {
    error("Can not initialize audio!");
  }

  unsigned char *samples;
  int out_samples;
  while(mf_decoder_decode(&decoder, &samples, &out_samples)) {      
    audio_play(&audio, samples, out_samples);    
  }

  audio_block(&audio);
  audio_free(&audio);

  mf_decoder_free(&decoder);

#ifdef DECODER_FROM_MEMORY
  free(buffer);
#endif
    
  return 0;
}
