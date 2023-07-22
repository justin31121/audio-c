#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

typedef unsigned char u8;
typedef char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef unsigned long u64;
typedef long s64;

#define panic(...) do{ fprintf(stderr, "%s:%d: error: %s: ", __FILE__, __LINE__, __func__); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); exit(1);}while(0)

#define error(...) do{ fprintf(stderr, "ERROR: "); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); exit(1);}while(0)

bool slurp_file(const char *name, unsigned char **buffer, uint32_t *buffer_size) {
  FILE *f = fopen(name, "rb");
  if(!f) {
    return false;
  }

  if(fseek(f, 0, SEEK_END) < 0) {
    fclose(f);
    return false;
  }

  long m = ftell(f);
  if(m < 0) {
    fclose(f);
    return false;
  }  

  if(fseek(f, 0, SEEK_SET) < 0) {
    fclose(f);
    return false;
  }

  *buffer = (unsigned char *) malloc((size_t) m + 1);
  if(!(*buffer)) {
    fclose(f);
    return false;
  }

  size_t _m = (size_t) m;
  size_t n = fread(*buffer, 1, _m, f);
  if(n != _m) {
    fclose(f);
    return false;
  }
  (*buffer)[n] = 0;

  *buffer_size = (uint32_t) n;

  fclose(f);
  return true;
}
