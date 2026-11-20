#include <libelf.h>
#include <stdio.h>
#include <stdlib.h>

#include "cli/cli.h"
#include "matcher/matcher.h"
#include "report/report.h"
#include "scanner/scanner.h"

int main(int argc, char *argv[]) {
  AppConfig config;

  if (!parse_args(argc, argv, &config)) return EXIT_FAILURE;

  if (config.showHelp) {
    print_help();
    return EXIT_SUCCESS;
  }

  if (elf_version(EV_CURRENT) == EV_NONE) {
    fprintf(stderr, "Cannot initialize libelf\n");
    return EXIT_FAILURE;
  }

  LibraryResults results;
  if (!init_results(&config, &results)) return EXIT_FAILURE;

  ScanStats stats;
  if (!scan_directory(&config, &results, &stats)) {
    free_results(&results);
    return EXIT_FAILURE;
  }

  sort_results(&results);
  if (!write_report(&config, &results, &stats)) {
    free_results(&results);
    return EXIT_FAILURE;
  }

  printf("Done. Checked %zu files, found %zu ELF executables.\n",
         stats.filesChecked, stats.elfExecutables);
  printf("Report saved to report.txt\n");

  free_results(&results);
  return EXIT_SUCCESS;
}
