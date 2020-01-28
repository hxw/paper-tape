// trim-runout.c

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
          "usage: %s [-5|-7|-8] [-t read-timeout-sec] [-r runout-count] [-o "
          "output-file] input-file\n",
          progname);
  exit(1);
}

uint8_t recorded[1024 * 1024];

int main(int argc, char *argv[]) {
  if (argc > 0) {
    progname = argv[0];
  }

  int read_timeout = 5;
  int runout_count = 120;
  int mask = 0xff;
  int ch = 0;

  FILE *input = stdin;   // default to stdin
  FILE *output = stdout; // default to stdout

  while ((ch = getopt(argc, argv, "h578t:r:o:")) != -1) {
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
    case 't':
      read_timeout = atoi(optarg);
      if (read_timeout < 1) {
        usage("read timeout cannot be negative");
      }
      break;
    case 'r':
      runout_count = atoi(optarg);
      if (runout_count < 0) {
        usage("runout-count cannot be negative");
      }
      break;
    case 'o':
      output = fopen(optarg, "wx");
      if (NULL == output) {
        usage("failed to create: %s: %s", optarg, strerror(errno));
      }
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

  bool starting = true;
  memset(recorded, 0, sizeof(recorded));
  size_t i = 0;

  while (!feof(input)) {

    struct timeval timeout = {
        .tv_sec = read_timeout,
        .tv_usec = 0,
    };
    fd_set in;
    FD_ZERO(&in);
    FD_SET(fileno(input), &in);
    if (select(FD_SETSIZE, &in, NULL, NULL, &timeout) < 0) {
      break;
    }
    if (!FD_ISSET(fileno(input), &in)) {
      break;
    }

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

      if (starting && 0 == n) {
        continue;
      } else {
        starting = false;
      }

      recorded[i] = n;
      ++i;
    }
  }

  // trim last corrupt byte
  if (i > 1 && 0 != recorded[i - 1]) {
    --i;
  }

  if (i > 0) {
    for (; i > 1; --i) {
      if (0 != recorded[i - 1]) {
        break;
      }
    }
  }

  for (size_t j = 0; j < runout_count; ++j) {
    fprintf(output, "%02x\n", 0);
  }

  for (size_t j = 0; j < i; ++j) {
    fprintf(output, "%02x\n", recorded[j]);
  }

  for (size_t j = 0; j < runout_count; ++j) {
    fprintf(output, "%02x\n", 0);
  }

  fclose(input);
  fclose(output);

  return 0;
}
