cmd_/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/nemu-main.o := unused

source_/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/nemu-main.o := src/nemu-main.c

deps_/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/nemu-main.o := \
    $(wildcard include/config/target/amcd.h) \
  /home/rainy/ics2023/nemu/include/common.h \
    $(wildcard include/config/target/am.h) \
    $(wildcard include/config/mbase.h) \
    $(wildcard include/config/msize.h) \
    $(wildcard include/config/isa64.h) \
  /home/rainy/ics2023/nemu/include/macro.h \
  /home/rainy/ics2023/nemu/include/debug.h \
  /home/rainy/ics2023/nemu/include/utils.h \
    $(wildcard include/config/target/native/elf.h) \
  src/monitor/sdb/sdb.h \

/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/nemu-main.o: $(deps_/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/nemu-main.o)

$(deps_/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/nemu-main.o):
