#include <elf.h>
#include <fcntl.h>
#include <gelf.h>
#include <libelf.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "elf-parser.h"

const char *architecture_name(Architecture architecture) {
  switch (architecture) {
    case ARCH_X86: return "x86";
    case ARCH_X86_64: return "x86_64";
    case ARCH_ARMV7: return "armv7";
    case ARCH_AARCH64: return "aarch64";
    default: return "unknown";
  }
}

static Architecture get_architecture(const GElf_Ehdr *header) {
  if (header->e_machine == EM_386 && header->e_ident[EI_CLASS] == ELFCLASS32) return ARCH_X86;
  if (header->e_machine == EM_X86_64 && header->e_ident[EI_CLASS] == ELFCLASS64) return ARCH_X86_64;
  if (header->e_machine == EM_ARM && header->e_ident[EI_CLASS] == ELFCLASS32) return ARCH_ARMV7;
  if (header->e_machine == EM_AARCH64 && header->e_ident[EI_CLASS] == ELFCLASS64) return ARCH_AARCH64;

  return ARCH_UNKNOWN;
}

static int has_interpreter(Elf *elf) {
  size_t count = 0;
  if (elf_getphdrnum(elf, &count) != 0) return 0;

  for (size_t i = 0; i < count; i++) {
    GElf_Phdr phdr;
    if (gelf_getphdr(elf, (int)i, &phdr) != NULL && phdr.p_type == PT_INTERP) return 1;
  }

  return 0;
}

static int add_needed(ElfInfo *info, const char *name) {
  char *copy = strdup(name);
  if (copy == NULL) return 0;

  char **newList = realloc(info->needed, (info->neededCount + 1) * sizeof(char *));
  if (newList == NULL) {
    free(copy);
    return 0;
  }

  info->needed = newList;
  info->needed[info->neededCount++] = copy;
  return 1;
}

static int read_dynamic_section(Elf *elf, ElfInfo *info) {
  Elf_Scn *section = NULL;

  while ((section = elf_nextscn(elf, section)) != NULL) {
    GElf_Shdr shdr;
    if (gelf_getshdr(section, &shdr) == NULL || shdr.sh_type != SHT_DYNAMIC) continue;

    if (shdr.sh_entsize == 0) return 0;

    Elf_Data *data = elf_getdata(section, NULL);
    if (data == NULL) return 0;

    size_t count = shdr.sh_size / shdr.sh_entsize;
    for (size_t i = 0; i < count; i++) {
      GElf_Dyn dyn;
      if (gelf_getdyn(data, (int)i, &dyn) == NULL) return 0;
      if (dyn.d_tag == DT_NULL) break;
      if (dyn.d_tag != DT_NEEDED && dyn.d_tag != DT_SONAME) continue;

      const char *name = elf_strptr(elf, shdr.sh_link, (size_t)dyn.d_un.d_val);
      if (name == NULL) return 0;

      if (dyn.d_tag == DT_NEEDED) {
        if (!add_needed(info, name)) return 0;
      } else {
        char *copy = strdup(name);
        if (copy == NULL) return 0;
        free(info->soname);
        info->soname = copy;
      }
    }
  }

  return 1;
}

int read_elf_info(const char *path, ElfInfo *info) {
  memset(info, 0, sizeof(*info));
  info->architecture = ARCH_UNKNOWN;

  int fd = open(path, O_RDONLY);
  if (fd < 0) return 0;

  Elf *elf = elf_begin(fd, ELF_C_READ, NULL);
  if (elf == NULL || elf_kind(elf) != ELF_K_ELF) {
    if (elf != NULL) elf_end(elf);
    close(fd);
    return 0;
  }

  GElf_Ehdr header;
  if (gelf_getehdr(elf, &header) == NULL) {
    elf_end(elf);
    close(fd);
    return 0;
  }

  info->architecture = get_architecture(&header);
  if (info->architecture == ARCH_UNKNOWN) {
    elf_end(elf);
    close(fd);
    return 0;
  }

  int success = read_dynamic_section(elf, info);

  if (success)
    info->executable = (header.e_type == ET_EXEC) || (header.e_type == ET_DYN && has_interpreter(elf) && info->soname == NULL);

  elf_end(elf);
  close(fd);

  if (!success) {
    free_elf_info(info);
    return 0;
  }

  return 1;
}

void free_elf_info(ElfInfo *info) {
  for (size_t i = 0; i < info->neededCount; i++) free(info->needed[i]);
  free(info->needed);
  free(info->soname);
  memset(info, 0, sizeof(*info));
}
