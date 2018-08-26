//
// Created by kim on 26.08.18.
//

#include "ELFModule.h"
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <dlfcn.h>
#include <Core/Debug.h>

extern "C" typedef Entity (*ModuleOfFunc)();

void read_elf_header64(s32 fd, Elf64_Ehdr *elf_header)
{
    assert(elf_header != NULL);
    assert(lseek(fd, (off_t)0, SEEK_SET) == (off_t)0);
    assert(read(fd, (void *)elf_header, sizeof(Elf64_Ehdr)) == sizeof(Elf64_Ehdr));
}


bool is_ELF64(Elf64_Ehdr eh)
{
    /* ELF magic bytes are 0x7f,'E','L','F'
     * Using  octal escape sequence to represent 0x7f
     */
    if(!strncmp((char*)eh.e_ident, "\177ELF", 4)) {
        /* IS a ELF file */
        return 1;
    } else {
        /* Not ELF file */
        return 0;
    }
}

void read_section_header_table64(s32 fd, Elf64_Ehdr eh, Elf64_Shdr sh_table[])
{
    u32 i;

    assert(lseek(fd, (off_t)eh.e_shoff, SEEK_SET) == (off_t)eh.e_shoff);

    for(i=0; i<eh.e_shnum; i++) {
        assert(read(fd, (void *)&sh_table[i], eh.e_shentsize)
               == eh.e_shentsize);
    }

}

char * read_section64(s32 fd, Elf64_Shdr sh)
{
    char* buff = (char*)malloc(sh.sh_size);
    if(!buff) {
        printf("%s:Failed to allocate %ldbytes\n",
               __func__, sh.sh_size);
    }

    assert(buff != NULL);
    assert(lseek(fd, (off_t)sh.sh_offset, SEEK_SET) == (off_t)sh.sh_offset);
    assert(read(fd, (void *)buff, sh.sh_size) == sh.sh_size);

    return buff;
}

bool scan_symbol_table64(void* soHandle, s32 fd,
                          Elf64_Ehdr eh,
                          Elf64_Shdr sh_table[],
                          u32 symbol_table)
{
    bool found = false;
    char *str_tbl;
    Elf64_Sym* sym_tbl;
    u32 i, symbol_count;

    sym_tbl = (Elf64_Sym*)read_section64(fd, sh_table[symbol_table]);

    /* Read linked string-table
     * Section containing the string table having names of
     * symbols of this section
     */
    u32 str_tbl_ndx = sh_table[symbol_table].sh_link;
    str_tbl = read_section64(fd, sh_table[str_tbl_ndx]);

    symbol_count = (sh_table[symbol_table].sh_size/sizeof(Elf64_Sym));

    for(i=0; i< symbol_count; i++) {
        if(ELF64_ST_BIND(sym_tbl[i].st_info) != STB_GLOBAL ||
           ELF64_ST_VISIBILITY(sym_tbl[i].st_other) != STV_DEFAULT ||
           ELF64_ST_TYPE(sym_tbl[i].st_info) != STT_FUNC) {
            continue;
        }

        auto symbolName = (str_tbl + sym_tbl[i].st_name);
        if(strstr(symbolName, "ModuleOf")) {
            auto func = (ModuleOfFunc)dlsym(soHandle, "_Z22ModuleOf_BgfxRenderingv");
            if(func) {
                printf("%s\n", symbolName);
                found |= IsEntityValid(func());
            }
        }
    }

    return found;
}

bool scan_symbols64(void* soHandle, s32 fd, Elf64_Ehdr eh, Elf64_Shdr sh_table[])
{
    u32 i;
    bool found = false;

    for(i=0; i<eh.e_shnum; i++) {
        if ((sh_table[i].sh_type==SHT_DYNSYM)) {
            found |= scan_symbol_table64(soHandle, fd, eh, sh_table, i);
        }
    }

    return found;
}

void read_elf_header(s32 fd, Elf32_Ehdr *elf_header)
{
    assert(elf_header != NULL);
    assert(lseek(fd, (off_t)0, SEEK_SET) == (off_t)0);
    assert(read(fd, (void *)elf_header, sizeof(Elf32_Ehdr)) == sizeof(Elf32_Ehdr));
}


bool is_ELF(Elf32_Ehdr eh)
{
    /* ELF magic bytes are 0x7f,'E','L','F'
     * Using  octal escape sequence to represent 0x7f
     */
    if(!strncmp((char*)eh.e_ident, "\177ELF", 4)) {
        printf("ELFMAGIC \t= ELF\n");
        /* IS a ELF file */
        return 1;
    } else {
        printf("ELFMAGIC mismatch!\n");
        /* Not ELF file */
        return 0;
    }
}

void read_section_header_table(s32 fd, Elf32_Ehdr eh, Elf32_Shdr sh_table[])
{
    u32 i;

    assert(lseek(fd, (off_t)eh.e_shoff, SEEK_SET) == (off_t)eh.e_shoff);

    for(i=0; i<eh.e_shnum; i++) {
        assert(read(fd, (void *)&sh_table[i], eh.e_shentsize)
               == eh.e_shentsize);
    }

}

char * read_section(s32 fd, Elf32_Shdr sh)
{
    char* buff = (char*)malloc(sh.sh_size);
    if(!buff) {
        printf("%s:Failed to allocate %dbytes\n",
               __func__, sh.sh_size);
    }

    assert(buff != NULL);
    assert(lseek(fd, (off_t)sh.sh_offset, SEEK_SET) == (off_t)sh.sh_offset);
    assert(read(fd, (void *)buff, sh.sh_size) == sh.sh_size);

    return buff;
}

bool scan_symbol_table(void* soHandle, s32 fd,
                        Elf32_Ehdr eh,
                        Elf32_Shdr sh_table[],
                        u32 symbol_table)
{
    bool found = false;
    char *str_tbl;
    Elf32_Sym* sym_tbl;
    u32 i, symbol_count;

    sym_tbl = (Elf32_Sym*)read_section(fd, sh_table[symbol_table]);

    /* Read linked string-table
     * Section containing the string table having names of
     * symbols of this section
     */
    u32 str_tbl_ndx = sh_table[symbol_table].sh_link;
    str_tbl = read_section(fd, sh_table[str_tbl_ndx]);

    symbol_count = (sh_table[symbol_table].sh_size/sizeof(Elf32_Sym));

    for(i=0; i< symbol_count; i++) {
        auto symbolName = (str_tbl + sym_tbl[i].st_name);
        if(memcmp(symbolName, "ModuleOf", 8) == 0) {
            auto func = (Entity (*)())dlsym(soHandle, symbolName);
            if(func) {
                found |= IsEntityValid(func());
            }
        }
    }
    return found;
}

bool scan_symbols(void* soHandle, s32 fd, Elf32_Ehdr eh, Elf32_Shdr sh_table[])
{
    bool found = false;
    u32 i;

    for(i=0; i<eh.e_shnum; i++) {
        if ((sh_table[i].sh_type==SHT_SYMTAB)
            || (sh_table[i].sh_type==SHT_DYNSYM)) {
            found |= scan_symbol_table(soHandle, fd, eh, sh_table, i);
        }
    }

    return found;
}

bool is64Bit(Elf32_Ehdr eh) {
    if (eh.e_ident[EI_CLASS] == ELFCLASS64)
        return true;
    else
        return false;
}

bool LoadElfModule(StringRef elfFile) {
    Elf32_Ehdr eh;
    auto fd = open(elfFile, O_RDONLY|O_SYNC);
    if(fd<0) {
        Log(0, LogSeverity_Error, "SO Library Not Found: %s", elfFile);
        return false;
    }

    auto handle = dlopen(elfFile, RTLD_NOW | RTLD_GLOBAL);

    /* ELF header : at start of file */
    read_elf_header(fd, &eh);
    if(!is_ELF(eh)) {
        Log(0, LogSeverity_Error, "SO Library is not in ELF format: %s", elfFile);
        return false;
    }
    if(is64Bit(eh) && sizeof(void*) == 8){
        Elf64_Ehdr eh64;	/* elf-header is fixed size */
        Elf64_Shdr* sh_tbl;	/* section-header table is variable size */

        read_elf_header64(fd, &eh64);

        /* Section header table :  */
        sh_tbl = (Elf64_Shdr*)malloc(eh64.e_shentsize * eh64.e_shnum);
        if(!sh_tbl) {
            printf("Failed to allocate %d bytes\n",
                   (eh64.e_shentsize * eh64.e_shnum));
        }
        read_section_header_table64(fd, eh64, sh_tbl);

        /* Symbol tables :
         * sh_tbl[i].sh_type
         * |`- SHT_SYMTAB
         *  `- SHT_DYNSYM
         */
        return scan_symbols64(handle, fd, eh64, sh_tbl);
    } else if(!is64Bit(eh) && sizeof(void*) == 4) {
        Elf32_Shdr* sh_tbl;	/* section-header table is variable size */

        /* Section header table :  */
        sh_tbl = (Elf32_Shdr*)malloc(eh.e_shentsize * eh.e_shnum);
        if(!sh_tbl) {
            printf("Failed to allocate %d bytes\n",
                   (eh.e_shentsize * eh.e_shnum));
        }
        read_section_header_table(fd, eh, sh_tbl);

        /* Symbol tables :
         * sh_tbl[i].sh_type
         * |`- SHT_SYMTAB
         *  `- SHT_DYNSYM
         */
        return scan_symbols(handle, fd, eh, sh_tbl);
    } else {
        Log(0, LogSeverity_Error, "SO Library has unsupported architecture: %s", elfFile);
    }

    return false;
}