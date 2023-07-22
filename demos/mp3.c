#define MINIMP3_IMPLEMENTATION
#include "../thirdparty/minimp3.h"

#define AUDIO_IMPLEMENTATION
#include "../src/audio.h"

#include "../src/common.c"

// mingw: gcc -o mp3 mp3.c -lole32 -lxaudio2_8
// msvc : cl mp3.c ole32.lib

int main(int argc, char **argv) {

  const char *prog = argv[0];
  if(argc < 2) {
    fprintf(stderr, "ERROR: Please provide an argument!\n");
    fprintf(stderr, "USAGE: %s <mp3-file>\n", prog);
    return 1;
  }
  const char *input = argv[1];

  // Read the whole mp3-file
  unsigned char *buffer;
  uint32_t buffer_size;
  if(!slurp_file(input, &buffer, &buffer_size)) {
    error("Can not find file: '%s'", input);
  }

  mp3dec_t mp3d;
  mp3dec_init(&mp3d);
  mp3dec_frame_info_t info;

  // Use two buffers to ensure seemless playing of
  // the audio. The buffer-size is multiplied by 2
  // because of potential left-over samples, from
  // the previous iteration
  short pcm[2][MINIMP3_MAX_SAMPLES_PER_FRAME * 2];
  int samples_count[2] = {0};
  int current = 0;

  bool first = true;
  Audio audio;

  char *data = (char *) pcm[current];

  while(buffer_size) {

    // Decode frame
    int samples = mp3dec_decode_frame(&mp3d, buffer, buffer_size, (short *) data, &info);
    int frame_bytes = info.frame_bytes;

    // TODO: handle more errors/cases
    if(samples <= 0 || frame_bytes <= 0) {

      // In this case no mp3-frame could be decoded.
      // Therefore the file is probably in the wrong foramt
      if(first) {
	error("Can not decode file: '%s'. Make sure it is a valid 'mp3'-file!", input);
      } else {
	panic("decoded samples and consumed bytes does not match");
      }		
    }

    // Audio can only be initialized after the first frame is decoded
    if(first) {
	    
      if(!audio_init(&audio, AUDIO_FMT_S16, info.channels, info.hz)) {
	error("Failed to initialize Audio!");
      }
	    
      first = false;
    }

    // Advance source-buffer, advance target-buffer and keep
    // track of the accumulated samples
    buffer += frame_bytes;
    buffer_size -= frame_bytes;
    data += samples * 4;
    samples_count[current] += samples;

    // Just in case only play N samples, where N is divisible
    // by 128. I dont know if audio quality is ensured otherwise
    int total = samples_count[current];
    int diff = total % 128;
    int need = total - diff;

    audio_play(&audio, (unsigned char *) pcm[current], need );

    // Switch the current buffer.
    current = 1 - current;
	
    // In the case that there are still samples in the current buffer
    // copy them over and adjust the parameters accordingly for the
    // next iteration
    data = (char *) pcm[current];
    samples_count[current] = diff;
    if(diff > 0) {
      memcpy(data, pcm[1 - current] + (need * 4), diff * 4);
      data += diff * 4;
    }
  }

  audio_block(&audio);
  audio_free(&audio);
  free(buffer);
	
  return 0;
}

