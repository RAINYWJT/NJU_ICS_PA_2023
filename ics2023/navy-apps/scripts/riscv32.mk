include $(NAVY_HOME)/scripts/riscv/common.mk
CFLAGS  += -march=rv32g -mabi=ilp32  #overwrite
LDFLAGS += -melf32lriscv
