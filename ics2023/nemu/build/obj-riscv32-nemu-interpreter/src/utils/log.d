cmd_/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/utils/log.o := unused

source_/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/utils/log.o := src/utils/log.c

deps_/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/utils/log.o := \
    $(wildcard include/config/ftrace.h) \
    $(wildcard include/config/trace.h) \
    $(wildcard include/config/trace/start.h) \
    $(wildcard include/config/trace/end.h) \
  /home/rainy/ics2023/nemu/include/common.h \
    $(wildcard include/config/target/am.h) \
    $(wildcard include/config/mbase.h) \
    $(wildcard include/config/msize.h) \
    $(wildcard include/config/isa64.h) \
  /home/rainy/ics2023/nemu/include/macro.h \
  /home/rainy/ics2023/nemu/include/debug.h \
  /home/rainy/ics2023/nemu/include/utils.h \
    $(wildcard include/config/target/native/elf.h) \

/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/utils/log.o: $(deps_/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/utils/log.o)

$(deps_/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/utils/log.o):
