// vis.c

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *progname = "";

void usage(const char *message, ...) {
  if (NULL != message) {
    fprintf(stderr, "error: ");
    va_list ap;
    va_start(ap, message);
    (void)vfprintf(stderr, message, ap);
    va_end(ap);
    fprintf(stderr, "\n");
  }

  fprintf(stderr, "usage: %s hex8-file\n", progname);
  exit(1);
}

int main(int argc, char *argv[]) {
  if (argc > 0) {
    progname = argv[0];
  }
  if (argc < 2) {
    usage(NULL);
  }

  FILE *f = fopen(argv[1], "r");
  if (NULL == f) {
    usage("cannot open: %s", argv[1]);
  }

  int line_count = 0;
  while (!feof(f)) {
    ++line_count;
    char buffer[16]; // only expect 3 chars max
    memset(buffer, 0, sizeof(buffer));
    if (NULL == fgets(buffer, sizeof(buffer), f)) {
      break;
    }

    if (isxdigit(buffer[0]) && isxdigit(buffer[1])) {

      printf("%8d  ", line_count);

      unsigned int n = 0;
      sscanf(buffer, "%x", &n);

      for (int i = 0; i < 8; ++i) {
        if (3 == i) {
          printf(".");
        }
        if (0 != (n & 0x01)) {
          printf("O");
        } else {
          printf(" ");
        }
        n >>= 1;
      }
      printf("\n");
    }
  }

  return 0;
}
