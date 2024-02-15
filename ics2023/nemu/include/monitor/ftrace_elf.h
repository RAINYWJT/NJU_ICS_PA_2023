#include <common.h>
#include <stdio.h>
#include <elf.h>

typedef struct ElfFunc{
    char func_name[128]; 
    paddr_t addr;      
    size_t size;        
}ElfFunc;

void init_ftrace(char* file_trace_log);

void display_ftrace(vaddr_t addr, vaddr_t ip, uint32_t value);

void fill_buffer(Elf32_Sym* symtab, int symtab_size, Elf32_Shdr strtab_header, char* name_str, ElfFunc* buf);

void count_functions(Elf32_Sym* symtab, int symtab_size);

void read_strtab(FILE* fp, Elf32_Shdr strtab_header, char** name_str);

void read_symtab_entries(FILE* fp, Elf32_Shdr symtab_header, Elf32_Shdr strtab_header, Elf32_Sym** symtab);

int find_symtab_index(Elf32_Shdr* section_headers, int shnum);

void read_section_headers(FILE* fp, Elf32_Ehdr elf_header, Elf32_Shdr** section_headers);

void read_elf_header(FILE* fp, Elf32_Ehdr* elf_header);

void open_file(FILE** fp, const char* file_trace_log);

