// elliott2ascii.c

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *letters[32] = {"\n", "A", "B", "C", "D", "E",  "F",  "G",
                           "H",  "I", "J", "K", "L", "M",  "N",  "O",
                           "P",  "Q", "R", "S", "T", "U",  "V",  "W",
                           "X",  "Y", "Z", "",  " ", "\r", "\n", ""};

#if 0
const char *figures[32] = {"\n", "1", "2", "*", "4", "$",  "=",  "7",
                           "8",  "'", ",", "+", ":", "-",  ".",  "%",
                           "0",  "(", ")", "3", "?", "5",  "6",  "/",
                           "@",  "9", "#", "",  " ", "\r", "\n", ""};
#else
const char *figures[32] = {"\n", "1", "2", "*", "4", "<",  "=",  "7",
                           "8",  "'", ",", "+", ":", "-",  ".",  ">",
                           "0",  "(", ")", "3", "?", "5",  "6",  "/",
                           "@",  "9", "→", "",  " ", "\r", "\n", ""};
#endif

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

  fprintf(stderr, "usage: %s hex5-file\n", progname);
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

  bool letter_shift = false;

  while (!feof(f)) {
    char buffer[16]; // only expect 3 chars max
    memset(buffer, 0, sizeof(buffer));
    if (NULL == fgets(buffer, sizeof(buffer), f)) {
      break;
    }

    if (isxdigit(buffer[0]) && isxdigit(buffer[1])) {

      unsigned int n = 0;
      sscanf(buffer, "%x", &n);
      if (27 == n) {
        letter_shift = false;
      } else if (31 == n) {
        letter_shift = true;
      }

      if (letter_shift) {
        printf("%s", letters[n & 0x1f]);
      } else {
        printf("%s", figures[n & 0x1f]);
      }
    }
  }

  return 0;
}
