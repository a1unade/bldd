#include <stdio.h>
#include <stdlib.h>

#include "cli/cli.h"

int main(int argc, char *argv[]) {
  AppConfig config;

  if (!parse_args(argc, argv, &config)) {
    fprintf(stderr, "Use --help for usage information\n");
    return EXIT_FAILURE;
  }

  if (config.showHelp) {
    print_help();
    return EXIT_SUCCESS;
  }

  printf("Directory: %s\n", config.directory);
  printf("Architecture: %d\n", config.architecture);
  printf("Report format: %d\n", config.reportFormat);
  printf("Libraries count: %zu\n", config.libraryCount);

  for (size_t i = 0; i < config.libraryCount; i++) {
    printf("Library: %s\n", config.libraries[i]);
  }

  return EXIT_SUCCESS;
}
