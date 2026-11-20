#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>

/** Поддерживаемые архитектуры ELF-файлов */
typedef enum {
  ARCH_UNKNOWN,
  ARCH_ANY,
  ARCH_X86,
  ARCH_X86_64,
  ARCH_ARMV7,
  ARCH_AARCH64
} Architecture;

/** Формат выходного отчета */
typedef enum {
  REPORT_UNKNOWN,
  REPORT_TXT,
  REPORT_PDF
} ReportFormat;

/** Параметры запуска программы */
typedef struct {
  char *directory;
  Architecture architecture;
  ReportFormat reportFormat;
  char **libraries;
  size_t libraryCount;
  int showHelp;
} AppConfig;

/** Информация, прочитанная из одного ELF-файла */
typedef struct {
  Architecture architecture;
  int executable;
  char *soname;
  char **needed;
  size_t neededCount;
} ElfInfo;

/** Одна библиотека и исполняемые файлы, которые ее используют */
typedef struct {
  char *name;
  Architecture architecture;
  char **executables;
  size_t count;
  size_t capacity;
} LibraryResult;

/** Массив результатов по библиотекам и архитектурам */
typedef struct {
  LibraryResult *items;
  size_t count;
} LibraryResults;

/** Счетчики сканирования для отчета */
typedef struct {
  size_t filesChecked;
  size_t elfExecutables;
} ScanStats;

#endif
