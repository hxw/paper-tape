// splitter.c

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>

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

  fprintf(stderr,
          "usage: %s [-5|-7|-8] [-s split-count] "
          "[-l leader-count] [-t trailer-count] "
          "[-o output-prefix] input-file\n",
          progname);
  exit(1);
}

uint8_t recorded[1024 * 1024];

int main(int argc, char *argv[]) {
  if (argc > 0) {
    progname = argv[0];
  }

  int read_timeout = 5;
  int split_count = 50;
  int leader_count = 30;
  int trailer_count = 30;
  int mask = 0xff;
  int ch = 0;

  FILE *input = stdin; // default to stdin
  const char *output_prefix = "split-tape";

  while ((ch = getopt(argc, argv, "h578s:l:t:o:")) != -1) {
    switch (ch) {
    case '5':
      mask = 0x1f;
      break;
    case '7':
      mask = 0x7f;
      break;
    case '8':
      mask = 0xff;
      break;
    case 's':
      split_count = atoi(optarg);
      if (split_count < 0) {
        usage("split-count cannot be negative");
      }
      break;
    case 'l':
      leader_count = atoi(optarg);
      if (leader_count < 0) {
        usage("leader-count cannot be negative");
      }
      break;
    case 't':
      trailer_count = atoi(optarg);
      if (trailer_count < 0) {
        usage("trailer-count cannot be negative");
      }
      break;
    case 'o':
      if (0 == strlen(optarg)) {
        usage("cannot have empty prefix");
      }
      output_prefix = strdup(optarg);
      break;
    case 'h':
    case '?':
    default:
      usage(NULL);
    }
  }
  argc -= optind;
  argv += optind;

  // optional input file
  if (argc >= 1) {
    input = fopen(argv[0], "r");
    if (NULL == input) {
      usage("cannot open: %s", argv[0]);
    }
  }

  memset(recorded, 0, sizeof(recorded));
  size_t i = 0;

  while (!feof(input)) {

    char buffer[16]; // only expect 3 chars max
    memset(buffer, 0, sizeof(buffer));
    if (NULL == fgets(buffer, sizeof(buffer), input)) {
      break;
    }

    if (isxdigit(buffer[0]) && isxdigit(buffer[1])) {
      unsigned int n = 0;
      sscanf(buffer, "%x", &n);

      // trim to 5/7/8 bits
      n &= mask;

      recorded[i] = n;
      ++i;
    }
  }

  size_t recorded_bytes = i;
  i = 0;

  for (int output_count = 0; true; ++output_count) {

    // skip leading runout
    while (0 == recorded[i]) {
      ++i;
      if (i >= recorded_bytes) {
        break;
      }
    }

    if (i >= recorded_bytes) {
      break;
    }

    // search for runout_count zeros or end of buffer
    size_t start = i;
    size_t finish = i;

    int blanks = 0;

    for (size_t j = start; j < recorded_bytes; ++j) {
      if (0 == recorded[j]) {
        ++blanks;
      } else {
        blanks = 0;
      }
      if (blanks >= split_count) {
        finish = j;
        break;
      }
    }

    if (finish > start) {

      while (finish > start && 0 == recorded[finish - 1]) {
        --finish;
      }
      if (finish <= start) {
        break;
      }

      char *output_file = NULL;
      asprintf(&output_file, "%s-%04d", output_prefix, output_count);

      FILE *output = fopen(output_file, "wx");
      if (NULL == output) {
        usage("failed to create: %s: %s", output_file, strerror(errno));
      }

      for (size_t k = 0; k < leader_count; ++k) {
        fprintf(output, "%02x\n", 0);
      }

      for (size_t k = start; k < finish; ++k) {
        fprintf(output, "%02x\n", recorded[k]);
      }

      for (size_t k = 0; k < trailer_count; ++k) {
        fprintf(output, "%02x\n", 0);
      }

      fclose(output);
      free(output_file);

      i = finish;

    } else {
      break;
    }
  }

  fclose(input);

  return 0;
}
