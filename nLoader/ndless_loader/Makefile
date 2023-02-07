CFLAGS = -O3 -Wall -Werror -Wextra -Wconversion -marm -mcpu=arm926ej-s -nodefaultlibs -nostartfiles -s -ffreestanding -std=gnu11 -fomit-frame-pointer -Wno-implicit-fallthrough
LDFLAGS = -e ndless_loader -T ldscript -flto

all: ../ndless_loader.bin

ndless_loader.o: ndless_loader.c
	arm-none-eabi-gcc $(CFLAGS) -o $@ -c $<

../ndless_loader.bin: ndless_loader.o
	arm-none-eabi-ld $(LDFLAGS) $^ -o $(<:.o=.elf)
	arm-none-eabi-objcopy -O binary $(<:.o=.elf) $@

cleantemp:
	rm -f *.o *.elf

clean: cleantemp
	rm -f ../ndless_loader.bin

.PHONY: clean cleantemp
