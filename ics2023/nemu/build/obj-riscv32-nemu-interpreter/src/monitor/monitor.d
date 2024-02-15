cmd_/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/monitor/monitor.o := unused

source_/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/monitor/monitor.o := src/monitor/monitor.c

deps_/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/monitor/monitor.o := \
    $(wildcard include/config/trace.h) \
    $(wildcard include/config/target/am.h) \
    $(wildcard include/config/device.h) \
    $(wildcard include/config/ftrace.h) \
    $(wildcard include/config/isa/loongarch32r.h) \
    $(wildcard include/config/itrace.h) \
    $(wildcard include/config/isa/x86.h) \
    $(wildcard include/config/isa/mips32.h) \
    $(wildcard include/config/isa/riscv.h) \
    $(wildcard include/config/rv64.h) \
  /home/rainy/ics2023/nemu/include/isa.h \
  /home/rainy/ics2023/nemu/src/isa/riscv32/include/isa-def.h \
  /home/rainy/ics2023/nemu/include/common.h \
    $(wildcard include/config/mbase.h) \
    $(wildcard include/config/msize.h) \
    $(wildcard include/config/isa64.h) \
  /home/rainy/ics2023/nemu/include/macro.h \
  /home/rainy/ics2023/nemu/include/debug.h \
  /home/rainy/ics2023/nemu/include/utils.h \
    $(wildcard include/config/target/native/elf.h) \
  /home/rainy/ics2023/nemu/include/memory/paddr.h \
    $(wildcard include/config/pc/reset/offset.h) \
  /home/rainy/ics2023/nemu/include/monitor/ftrace_elf.h \

/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/monitor/monitor.o: $(deps_/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/monitor/monitor.o)

$(deps_/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/monitor/monitor.o):
