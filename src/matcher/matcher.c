#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "elf/elf-parser.h"
#include "matcher.h"

static const Architecture architectures[] = { ARCH_X86, ARCH_X86_64, ARCH_ARMV7, ARCH_AARCH64 };

static int add_library(LibraryResults *results, const char *name, Architecture arch) {
  for (size_t i = 0; i < results->count; i++) {
    if (results->items[i].architecture == arch && strcmp(results->items[i].name, name) == 0) return 1;
  }

  LibraryResult *items = realloc(results->items, (results->count + 1) * sizeof(LibraryResult));
  if (items == NULL) return 0;
  results->items = items;

  LibraryResult *result = &results->items[results->count];
  memset(result, 0, sizeof(*result));
  result->name = strdup(name);
  if (result->name == NULL) return 0;
  result->architecture = arch;
  results->count++;
  return 1;
}

int init_results(const AppConfig *config, LibraryResults *results) {
  memset(results, 0, sizeof(*results));

  for (size_t i = 0; i < config->libraryCount; i++) {
    const char *input = config->libraries[i];
    const char *name = input;
    Architecture arch = config->architecture;
    ElfInfo info;
    int isPath = strchr(input, '/') != NULL || access(input, F_OK) == 0;

    if (isPath) {
      if (!read_elf_info(input, &info)) {
        fprintf(stderr, "Cannot read library ELF: %s\n", input);
        free_results(results);
        return 0;
      }

      if (info.executable) {
        fprintf(stderr, "Not a shared library: %s\n", input);
        free_elf_info(&info);
        free_results(results);
        return 0;
      }

      if (arch != ARCH_ANY && arch != info.architecture) {
        fprintf(stderr, "Architecture of %s does not match -a\n", input);
        free_elf_info(&info);
        free_results(results);
        return 0;
      }

      arch = info.architecture;
      name = info.soname;
      if (name == NULL || name[0] == '\0') {
        const char *slash = strrchr(input, '/');
        name = slash == NULL ? input : slash + 1;
      }
    }

    int success = 1;
    if (arch == ARCH_ANY) {
      for (size_t j = 0; j < sizeof(architectures) / sizeof(architectures[0]); j++) {
        if (!add_library(results, name, architectures[j])) {
          success = 0;
          break;
        }
      }
    } else {
      success = add_library(results, name, arch);
    }

    if (isPath) free_elf_info(&info);
    if (!success) {
      fprintf(stderr, "Out of memory\n");
      free_results(results);
      return 0;
    }
  }

  return 1;
}

static int add_executable(LibraryResult *result, const char *path) {
  for (size_t i = 0; i < result->count; i++) {
    if (strcmp(result->executables[i], path) == 0) return 1;
  }

  if (result->count == result->capacity) {
    size_t capacity = result->capacity == 0 ? 8 : result->capacity * 2;
    char **items = realloc(result->executables, capacity * sizeof(char *));
    if (items == NULL) return 0;
    result->executables = items;
    result->capacity = capacity;
  }

  char *copy = strdup(path);
  if (copy == NULL) return 0;
  result->executables[result->count++] = copy;
  return 1;
}

int match_executable(const char *path, const ElfInfo *info, LibraryResults *results) {
  for (size_t i = 0; i < results->count; i++) {
    LibraryResult *result = &results->items[i];
    if (result->architecture != info->architecture) continue;

    for (size_t j = 0; j < info->neededCount; j++) {
      if (strcmp(info->needed[j], result->name) == 0) {
        if (!add_executable(result, path)) return 0;
        break;
      }
    }
  }
  return 1;
}

void free_results(LibraryResults *results) {
  for (size_t i = 0; i < results->count; i++) {
    LibraryResult *result = &results->items[i];
    free(result->name);
    for (size_t j = 0; j < result->count; j++) free(result->executables[j]);
    free(result->executables);
  }
  free(results->items);
  memset(results, 0, sizeof(*results));
}
