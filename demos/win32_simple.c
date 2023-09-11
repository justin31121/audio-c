#include <stdio.h>

#define MF_DECODER_IMPLEMENTATION
#include "../src/mf_decoder.h"

#define AUDIO_IMPLEMENTATION
#include "../src/audio.h"

#include "../src/common.c"

int main(int argc, char **argv) {

  const char *prog = argv[0];
  if(argc < 2) {
    fprintf(stderr, "ERROR: Please provide an argument!\n");
    fprintf(stderr, "USAGE: %s <audio-file>\n", prog);
    return 1;
  }
  const char *input = argv[1];
  
  unsigned char *buffer;
  unsigned int size;
  if(!slurp_file(input, &buffer, &size)) {
    error("Can not load file: '%s'", input);
  }
    
  MF_Decoder decoder;
  unsigned char *samples;
  unsigned int samples_count;
  int channels, sample_rate;

  if(!mf_decoder_slurp_memory((const char *) buffer, size,
			      MF_DECODER_FMT_S16,
			      &channels, &sample_rate,
			      &samples, &samples_count)) {
    fprintf(stderr,
	    "ERROR: Can not decode file: '%s'\n"
	    "       Make sure the filepath is an\n"
	    "       audio file.\n",  input);
    return 1;
  }

  Audio audio;
  if(!audio_init(&audio, AUDIO_FMT_S16, channels, sample_rate)) {
    error("Can not initialize audio!");
  }  
  
  audio_play(&audio, samples, samples_count);

  audio_block(&audio);
  audio_free(&audio);

  free(samples);
  free(buffer);


  return 0;
}
