#define AUDIO_IMPLEMENTATION
#include "../src/audio.h"

#define WAV_IMPLEMENTATION
#include "../src/wav.h"

#include "../src/common.c"

// mingw: gcc -o wav wav.c -lole32 -lxaudio2_8
// msvc : cl wav.c ole32.lib

int main(int argc, char **argv) {

  const char *prog = argv[0];
  if(argc < 2) {
    fprintf(stderr, "ERROR: Please provide an argument!\n");
    fprintf(stderr, "USAGE: %s <wav-file>\n", prog);
    return 1;
  }
  const char *input = argv[1];

  // Read the whole wav-file
  Wav wav;
  unsigned char *wav_data;
  uint32_t wav_size;    
  if(!wav_slurp(&wav, input, &wav_data, &wav_size)) {
    error("Can not load file: '%s'. Make sure it is a valid 'wav'-file!", input);
  }
  printf("channels: %d, sample_rate: %d, wav_size: %d\n", wav.channels, wav.sample_rate, wav_size);

  // Initialize audio
  Audio audio;
  if(!audio_init(&audio, AUDIO_FMT_S16, wav.channels, wav.sample_rate)) {
    error("Failed to initialize Audio!");
  }

  // Player audio
  audio_play(&audio, wav_data, (int) wav_size / audio.sample_size );

  audio_block(&audio);
    
  audio_free(&audio);   
  free(wav_data);    
    
  return 0;
}
