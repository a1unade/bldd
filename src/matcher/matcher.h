#ifndef MATCHER_H
#define MATCHER_H

#include "types/types.h"

/**
 * Создает список библиотек для поиска
 * Если передан путь к .so, определяет ее SONAME и архитектуру
 *
 * @return 1 при успехе, 0 при ошибке.
 */
int init_results(const AppConfig *config, LibraryResults *results);

/** Добавляет исполняемый файл в подходящие результаты */
int match_executable(const char *path, const ElfInfo *info, LibraryResults *results);

/** Освобождает результаты */
void free_results(LibraryResults *results);

#endif
