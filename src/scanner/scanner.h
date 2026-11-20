#ifndef SCANNER_H
#define SCANNER_H

#include "types/types.h"

/**
 * Рекурсивно обходит каталог и ищет ELF-программы
 * Символические ссылки не обходит, чтобы не получить циклы
 *
 * @return 1 при успехе, 0 при ошибке.
 */
int scan_directory(const AppConfig *config, LibraryResults *results, ScanStats *stats);

#endif
