#include <getopt.h>
#include <stdio.h>
#include <string.h>

#include "cli.h"

static Architecture parse_architecture(const char *value);
static ReportFormat parse_report_format(const char *value);

int parse_args(int argc, char *argv[], AppConfig *config) {
  config->directory = ".";
  config->architecture = ARCH_ANY;
  config->reportFormat = REPORT_TXT;
  config->libraries = NULL;
  config->libraryCount = 0;
  config->showHelp = 0;

  static struct option longOptions[] = {
      {"help", no_argument, NULL, 'h'},
      {NULL, 0, NULL, 0}
  };

  int option;

  while ((option = getopt_long(argc, argv, "d:a:f:h", longOptions, NULL)) != -1) {
    switch (option) {
      case 'd':
        config->directory = optarg;
        break;

      case 'a':
        config->architecture = parse_architecture(optarg);

        if (config->architecture == ARCH_UNKNOWN) {
          fprintf(stderr, "Unknown architecture: %s\n", optarg);
          return 0;
        }

        break;

      case 'f':
        config->reportFormat = parse_report_format(optarg);

        if (config->reportFormat == REPORT_UNKNOWN) {
          fprintf(stderr, "Unknown report format: %s\n", optarg);
          return 0;
        }

        break;

      case 'h':
        config->showHelp = 1;
        return 1;

      default:
        return 0;
    }
  }

  config->libraries = &argv[optind];
  config->libraryCount = argc - optind;

  if (config->libraryCount == 0) {
    fprintf(stderr, "No libraries specified\n");
    return 0;
  }

  return 1;
}

void print_help() {
  printf(
    "Usage: bldd [options] <library>...\n"
    "\n"
    "Options:\n"
    "  -d <directory>       Директория для сканирования\n"
    "  -a <architecture>    x86, x86_64, armv7, aarch64\n"
    "  -f <format>          txt, pdf\n"
    "  -h, --help           Показать справку\n"
  );
}

static Architecture parse_architecture(const char *value) {
  if (strcmp(value, "x86") == 0) {
    return ARCH_X86;
  }

  if (strcmp(value, "x86_64") == 0) {
    return ARCH_X86_64;
  }

  if (strcmp(value, "armv7") == 0) {
    return ARCH_ARMV7;
  }

  if (strcmp(value, "aarch64") == 0) {
    return ARCH_AARCH64;
  }

  return ARCH_UNKNOWN;
}

static ReportFormat parse_report_format(const char *value) {
  if (strcmp(value, "txt") == 0) {
    return REPORT_TXT;
  }

  if (strcmp(value, "pdf") == 0) {
    return REPORT_PDF;
  }

  return REPORT_UNKNOWN;
}
