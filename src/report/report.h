#ifndef REPORT_H
#define REPORT_H

#include "types/types.h"

/** Сортирует библиотеки по архитектуре и количеству использований */
void sort_results(LibraryResults *results);

/** Записывает текстовый отчет по найденным зависимостям */
int write_report(const AppConfig *config, LibraryResults *results, const ScanStats *stats);

#endif
