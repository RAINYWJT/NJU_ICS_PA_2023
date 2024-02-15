cmd_/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/device/disk.o := unused

source_/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/device/disk.o := src/device/disk.c

deps_/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/device/disk.o := \
  /home/rainy/ics2023/nemu/include/device/map.h \
  /home/rainy/ics2023/nemu/include/cpu/difftest.h \
    $(wildcard include/config/difftest.h) \
  /home/rainy/ics2023/nemu/include/common.h \
    $(wildcard include/config/target/am.h) \
    $(wildcard include/config/mbase.h) \
    $(wildcard include/config/msize.h) \
    $(wildcard include/config/isa64.h) \
  /home/rainy/ics2023/nemu/include/macro.h \
  /home/rainy/ics2023/nemu/include/debug.h \
  /home/rainy/ics2023/nemu/include/utils.h \
    $(wildcard include/config/target/native/elf.h) \
  /home/rainy/ics2023/nemu/include/difftest-def.h \
    $(wildcard include/config/isa/x86.h) \
    $(wildcard include/config/isa/mips32.h) \
    $(wildcard include/config/isa/riscv.h) \
    $(wildcard include/config/rv64.h) \
    $(wildcard include/config/rve.h) \
    $(wildcard include/config/isa/loongarch32r.h) \

/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/device/disk.o: $(deps_/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/device/disk.o)

$(deps_/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/device/disk.o):
