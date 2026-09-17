#ifndef CLI_H
#define CLI_H

#include "types/types.h"

/**
 * Парсит аргументы командной строки и заполняет конфигурацию приложения
 *
 * @param argc Количество аргументов командной строки
 * @param argv Массив аргументов командной строки
 * @param config Указатель на конфигурацию приложения
 *
 * @return 1 при успешном разборе аргументов, 0 при ошибке
 */
int parse_args(int argc, char *argv[], AppConfig *config);

/**
 * Выводит справку по использованию приложения и примеры запуска
 */
void print_help();

#endif
