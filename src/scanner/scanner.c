#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "elf/elf-parser.h"
#include "matcher/matcher.h"
#include "scanner.h"

static char *join_path(const char *directory, const char *name) {
  size_t length = strlen(directory) + strlen(name) + 2;
  char *path = malloc(length);
  if (path != NULL) snprintf(path, length, "%s/%s", directory, name);
  return path;
}

static int scan_folder(const char *directory, const AppConfig *config, LibraryResults *results, ScanStats *stats) {
  DIR *dir = opendir(directory);
  if (dir == NULL) {
    fprintf(stderr, "Cannot open directory %s: %s\n", directory, strerror(errno));
    return 0;
  }

  struct dirent *entry;
  int success = 1;

  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

    char *path = join_path(directory, entry->d_name);
    if (path == NULL) {
      success = 0;
      break;
    }

    struct stat st;
    if (lstat(path, &st) != 0) {
      free(path);
      continue;
    }

    if (S_ISDIR(st.st_mode)) {
      scan_folder(path, config, results, stats);
    } else if (S_ISREG(st.st_mode) && (st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))) {
      stats->filesChecked++;

      ElfInfo info;
      if (read_elf_info(path, &info)) {
        if (info.executable && (config->architecture == ARCH_ANY || config->architecture == info.architecture)) {
          stats->elfExecutables++;
          if (!match_executable(path, &info, results)) success = 0;
        }
        free_elf_info(&info);
      }
    }

    free(path);
    if (!success) break;
  }

  closedir(dir);
  return success;
}

int scan_directory(const AppConfig *config, LibraryResults *results, ScanStats *stats) {
  memset(stats, 0, sizeof(*stats));
  return scan_folder(config->directory, config, results, stats);
}
