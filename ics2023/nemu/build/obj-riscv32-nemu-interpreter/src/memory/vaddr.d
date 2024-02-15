cmd_/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/memory/vaddr.o := unused

source_/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/memory/vaddr.o := src/memory/vaddr.c

deps_/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/memory/vaddr.o := \
  /home/rainy/ics2023/nemu/include/isa.h \
  /home/rainy/ics2023/nemu/src/isa/riscv32/include/isa-def.h \
  /home/rainy/ics2023/nemu/include/common.h \
    $(wildcard include/config/target/am.h) \
    $(wildcard include/config/mbase.h) \
    $(wildcard include/config/msize.h) \
    $(wildcard include/config/isa64.h) \
  /home/rainy/ics2023/nemu/include/macro.h \
  /home/rainy/ics2023/nemu/include/debug.h \
  /home/rainy/ics2023/nemu/include/utils.h \
    $(wildcard include/config/target/native/elf.h) \
  /home/rainy/ics2023/nemu/include/memory/paddr.h \
    $(wildcard include/config/pc/reset/offset.h) \

/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/memory/vaddr.o: $(deps_/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/memory/vaddr.o)

$(deps_/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/memory/vaddr.o):
