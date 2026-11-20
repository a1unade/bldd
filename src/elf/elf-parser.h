#ifndef ELF_PARSER_H
#define ELF_PARSER_H

#include "types/types.h"

/**
 * Читает ELF-заголовок и записи DT_NEEDED / DT_SONAME
 *
 * @return 1 если прочитан ELF, 0 если файл не подходит или поврежден
 */
int read_elf_info(const char *path, ElfInfo *info);

/** Освобождает строки, полученные при чтении ELF */
void free_elf_info(ElfInfo *info);

/** Возвращает понятное название архитектуры */
const char *architecture_name(Architecture architecture);

#endif
