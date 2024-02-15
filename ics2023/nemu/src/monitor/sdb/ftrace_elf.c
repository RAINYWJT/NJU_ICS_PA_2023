#include <stdio.h>
#include <monitor/ftrace_elf.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

ElfFunc buf[1024];
int f_num=0;
int count=0;

void open_file(FILE** fp, const char* file_trace_log) {
    *fp = fopen(file_trace_log, "r");
    if (*fp == NULL) {
        printf("not found the file!\n");
        assert(0);
    }
}

void read_elf_header(FILE* fp, Elf32_Ehdr* elf_header) {
    int flag = fread(elf_header, sizeof(Elf32_Ehdr), 1, fp);
    if (!flag) {
        printf("Head fail!\n");
        assert(0);
    }
    if (elf_header->e_ident[0] != 0x7f || elf_header->e_ident[1] != 0x45 || elf_header->e_ident[2] != 0x4c || elf_header->e_ident[3] != 0x46) {
        printf("Not an elf!\n");
        assert(0);
    }
}

void read_section_headers(FILE* fp, Elf32_Ehdr elf_header, Elf32_Shdr** section_headers) {
    *section_headers = (Elf32_Shdr*) malloc(sizeof(Elf32_Shdr) * elf_header.e_shnum);
    int flag = fseek(fp, elf_header.e_shoff, SEEK_SET);
    if (flag) {
        printf("fail to find sections\n");
        assert(0);
    }
    flag = fread(*section_headers, sizeof(Elf32_Shdr) * elf_header.e_shnum, 1, fp);
    if (!flag) {
        printf("fail to read sections\n");
        assert(0);
    }
}

int find_symtab_index(Elf32_Shdr* section_headers, int shnum) {
    int symtab_index = 0;
    for (int i = 0; i < shnum; i++) {
        if (section_headers[i].sh_type == SHT_SYMTAB) {
            symtab_index = i;
            break;
        }
    }
    if (symtab_index == 0) {
        printf("Not found the symtab!\n");
    }
    return symtab_index;
}

void read_symtab_entries(FILE* fp, Elf32_Shdr symtab_header, Elf32_Shdr strtab_header, Elf32_Sym** symtab) {
    int symtab_size = symtab_header.sh_size / symtab_header.sh_entsize;
    *symtab = (Elf32_Sym*) malloc(sizeof(Elf32_Sym) * symtab_size);

    for (int i = 0; i < symtab_size; i++) {
        rewind(fp);
        int flag = fseek(fp, symtab_header.sh_offset + i * sizeof(Elf32_Sym), SEEK_SET);
        if (flag) {
            printf("fail to find sections\n");
            assert(0);
        }
        flag = fread(&(*symtab)[i], sizeof(Elf32_Sym), 1, fp);
        if (!flag) {
            printf("fail to read sections\n");
            assert(0);
        }
    }
}

void read_strtab(FILE* fp, Elf32_Shdr strtab_header, char** name_str) {
    *name_str = (char*) malloc(strtab_header.sh_size);

    rewind(fp);
    int flag = fseek(fp, strtab_header.sh_offset, SEEK_SET);
    if (flag) {
        printf("fail to find sections!\n");
        assert(0);
    }
    flag = fread(*name_str, strtab_header.sh_size, 1, fp);
    if (!flag) {
        printf("fail to read sections\n");
        assert(0);
    }
}

void count_functions(Elf32_Sym* symtab, int symtab_size) {
    for (int i = 0; i < symtab_size; i++) {
        if (ELF32_ST_TYPE(symtab[i].st_info) == STT_FUNC) {
            f_num++;
        }
    }
}

void fill_buffer(Elf32_Sym* symtab, int symtab_size, Elf32_Shdr strtab_header, char* name_str, ElfFunc* buf) {
    int index = 0;
    for (int i = 0; i < symtab_size; i++) {
        if (ELF32_ST_TYPE(symtab[i].st_info) == STT_FUNC) {
            buf[index].addr = symtab[i].st_value;
            buf[index].size = symtab[i].st_size;
            strncpy(buf[index].func_name, name_str + symtab[i].st_name, 64);
            index++;
        }
    }
}

void init_ftrace(char* file_trace_log) {
    FILE* fp;
    open_file(&fp, file_trace_log);
    Elf32_Ehdr elf_header;
    read_elf_header(fp, &elf_header);
    Elf32_Shdr* section_headers;
    read_section_headers(fp, elf_header, &section_headers);
    int symtab_index = find_symtab_index(section_headers, elf_header.e_shnum);
    Elf32_Shdr symtab_header = section_headers[symtab_index];
    Elf32_Shdr strtab_header = section_headers[symtab_header.sh_link];
    Elf32_Sym* symtab;
    read_symtab_entries(fp, symtab_header, strtab_header, &symtab);
    char* name_str;
    read_strtab(fp, strtab_header, &name_str);
    count_functions(symtab, symtab_header.sh_size / symtab_header.sh_entsize);
    fill_buffer(symtab, symtab_header.sh_size / symtab_header.sh_entsize, strtab_header, name_str, buf);
    free(section_headers);
    free(symtab);
    free(name_str);
    fclose(fp);
}


typedef struct CallInfo {
    vaddr_t addr;
    char func_name[1000];
} CallInfo;

#define MAX_CALL_STACK_SIZE 1000
CallInfo call_stack[MAX_CALL_STACK_SIZE]; 
int stack_top = -1;

void push_callstack(vaddr_t addr, const char* func_name) {
    if (stack_top < MAX_CALL_STACK_SIZE - 1) {
        stack_top++;
        call_stack[stack_top].addr = addr;
        strcpy(call_stack[stack_top].func_name, func_name);
    }
}

void pop_callstack() {
    if (stack_top >= 0) {
        stack_top--;
    }
}

void display_ftrace(vaddr_t addr, vaddr_t ip, uint32_t value) {
    #ifdef CONFIG_FTRACE
    //printf("%s\n",ANSI_FMT("========================================display ftrace======================================",ANSI_BG_CYAN));
    if (!CONFIG_FTRACE) {
        return;
    }
    if (value == 0x00008067) {
        int match_index = -1; 
        for (int i = stack_top; i >= 0; i--) {
            if (ip >= call_stack[i].addr && ip <= call_stack[i].addr + buf[i].size) {
                match_index = i;
                break;
            }
        }
        if (match_index != -1) {
            for (int i = stack_top; i > match_index; i--) {
                printf("0x%x%*sret  [%s @0x%x]\n", ip, count, " ", call_stack[i].func_name, call_stack[i].addr);
                pop_callstack();
                count--; 
            }
            printf("0x%x%*sret  [%s @0x%x]\n", ip, count, " ", call_stack[match_index].func_name, call_stack[match_index].addr);
            pop_callstack();
            count--; 
            return;
        }
    }
    for (int i = 0; i < f_num; i++) {
        if (addr == buf[i].addr) {
            printf("0x%x%*scall  [%s @0x%x]\n", ip, count, " ", buf[i].func_name, addr);
            push_callstack(addr, buf[i].func_name);
            count++; 
            return;
        }
    }
    //printf("%s\n",ANSI_FMT("=========================================display end========================================",ANSI_BG_CYAN));
    #endif
}

