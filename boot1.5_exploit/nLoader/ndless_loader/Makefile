GCC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
GCCFLAGS = -O3 -Wall -Werror -Wextra -marm -mcpu=arm926ej-s -nodefaultlibs -nostartfiles -s -ffreestanding -std=gnu11 -fomit-frame-pointer -Wno-implicit-fallthrough
LDFLAGS = -e ndless_loader -T ldscript -flto
OBJCOPY := "$(shell which arm-elf-objcopy 2>/dev/null)"
ifeq (${OBJCOPY},"")
	OBJCOPY := arm-none-eabi-objcopy
endif
BIN = ../ndless_loader.bin

all: $(BIN)

ndless_loader.o: ndless_loader.c
	$(GCC) $(GCCFLAGS) -o $@ -c $<

$(BIN): ndless_loader.o
	$(LD) $(LDFLAGS) $^ -o $(<:.o=.elf)
	$(OBJCOPY) -O binary $(<:.o=.elf) $@

cleanrem:
	rm -f *.o *.elf

clean: cleanrem
	rm -f $(BIN)
