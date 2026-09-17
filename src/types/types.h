#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>

/**
 * Поддерживаемые архитектуры исполняемых файлов
 */
typedef enum {
  ARCH_UNKNOWN,
  ARCH_ANY,
  ARCH_X86,
  ARCH_X86_64,
  ARCH_ARMV7,
  ARCH_AARCH64
} Architecture;

/**
 * Поддерживаемые форматы выходного отчета
 */
typedef enum {
  REPORT_UNKNOWN,
  REPORT_TXT,
  REPORT_PDF
} ReportFormat;

/**
 * Конфигурация запуска приложения
 */
typedef struct {
  /** Директория для сканирования */
  char *directory;

  /** Архитектура для фильтрации файлов */
  Architecture architecture;

  /** Формат выходного отчета */
  ReportFormat reportFormat;

  /** Список библиотек для поиска */
  char **libraries;

  /** Количество библиотек в списке */
  size_t libraryCount;

  /** Флаг вывода справки */
  int showHelp;
} AppConfig;

#endif
