#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "elf/elf-parser.h"
#include "report.h"

static int compare_results(const void *a, const void *b) {
  const LibraryResult *left = a;
  const LibraryResult *right = b;

  if (left->architecture != right->architecture)
    return (int)left->architecture - (int)right->architecture;

  if (left->count < right->count) return 1;
  if (left->count > right->count) return -1;
  return strcmp(left->name, right->name);
}

static int compare_paths(const void *a, const void *b) {
  const char *const *left = a;
  const char *const *right = b;
  return strcmp(*left, *right);
}

void sort_results(LibraryResults *results) {
  qsort(results->items, results->count, sizeof(LibraryResult), compare_results);
  for (size_t i = 0; i < results->count; i++) {
    LibraryResult *result = &results->items[i];
    qsort(result->executables, result->count, sizeof(char *), compare_paths);
  }
}

static void print_report(FILE *stream, const AppConfig *config, LibraryResults *results, const ScanStats *stats) {
  fprintf(stream, "Report on dynamically used libraries by ELF executables\n");
  fprintf(stream, "Scan directory: %s\n", config->directory);
  fprintf(stream, "Executable file candidates: %zu\n", stats->filesChecked);
  fprintf(stream, "Supported ELF executables: %zu\n", stats->elfExecutables);
  fprintf(stream, "Dependencies: direct DT_NEEDED entries only\n\n");

  Architecture currentArch = ARCH_UNKNOWN;
  size_t printed = 0;

  for (size_t i = 0; i < results->count; i++) {
    LibraryResult *result = &results->items[i];

    if (result->count == 0) continue;

    if (result->architecture != currentArch) {
      currentArch = result->architecture;

      fprintf(stream, "---------- %s ----------\n\n", architecture_name(currentArch));
    }

    fprintf(stream, "%s (%zu execs)\n", result->name, result->count);

    for (size_t j = 0; j < result->count; j++) {
      fprintf(stream, "          -> %s\n", result->executables[j]);
    }

    fprintf(stream, "\n");
    printed++;
  }

  if (printed == 0) fprintf(stream, "No matching dependencies found.\n");
}

int write_report(const AppConfig *config, LibraryResults *results, const ScanStats *stats) {
  FILE *file = fopen("report.txt", "w");

  if (file == NULL) {
    perror("Cannot create report.txt");
    return 0;
  }

  // Вывод отчета в консоль
  print_report(stdout, config, results, stats);

  // Сохранение отчета в файл
  print_report(file, config, results, stats);

  if (fclose(file) != 0) {
    perror("Cannot save report.txt");
    return 0;
  }

  return 1;
}
