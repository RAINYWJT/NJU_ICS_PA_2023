cmd_/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/monitor/sdb/ftrace_elf.o := unused

source_/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/monitor/sdb/ftrace_elf.o := src/monitor/sdb/ftrace_elf.c

deps_/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/monitor/sdb/ftrace_elf.o := \
    $(wildcard include/config/ftrace.h) \
  /home/rainy/ics2023/nemu/include/monitor/ftrace_elf.h \
  /home/rainy/ics2023/nemu/include/common.h \
    $(wildcard include/config/target/am.h) \
    $(wildcard include/config/mbase.h) \
    $(wildcard include/config/msize.h) \
    $(wildcard include/config/isa64.h) \
  /home/rainy/ics2023/nemu/include/macro.h \
  /home/rainy/ics2023/nemu/include/debug.h \
  /home/rainy/ics2023/nemu/include/utils.h \
    $(wildcard include/config/target/native/elf.h) \

/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/monitor/sdb/ftrace_elf.o: $(deps_/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/monitor/sdb/ftrace_elf.o)

$(deps_/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/monitor/sdb/ftrace_elf.o):
