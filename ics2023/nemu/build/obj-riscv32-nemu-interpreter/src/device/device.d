cmd_/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/device/device.o := unused

source_/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/device/device.o := src/device/device.c

deps_/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/device/device.o := \
    $(wildcard include/config/target/am.h) \
    $(wildcard include/config/has/vga.h) \
    $(wildcard include/config/has/keyboard.h) \
    $(wildcard include/config/has/serial.h) \
    $(wildcard include/config/has/timer.h) \
    $(wildcard include/config/has/audio.h) \
    $(wildcard include/config/has/disk.h) \
    $(wildcard include/config/has/sdcard.h) \
  /home/rainy/ics2023/nemu/include/common.h \
    $(wildcard include/config/mbase.h) \
    $(wildcard include/config/msize.h) \
    $(wildcard include/config/isa64.h) \
  /home/rainy/ics2023/nemu/include/macro.h \
  /home/rainy/ics2023/nemu/include/debug.h \
  /home/rainy/ics2023/nemu/include/utils.h \
    $(wildcard include/config/target/native/elf.h) \
  /home/rainy/ics2023/nemu/include/device/alarm.h \

/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/device/device.o: $(deps_/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/device/device.o)

$(deps_/home/rainy/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/device/device.o):
