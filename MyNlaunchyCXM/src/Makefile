HOST_GCC = gcc
GCC = arm-none-eabi-gcc
LD  = arm-none-eabi-ld
GCCFLAGS = -Os -Wall -Wextra -Werror -Wno-attributes -marm -mcpu=arm926ej-s -s -fPIE -ffreestanding -nostdlib -nostartfiles -std=gnu11
LDFLAGS  = -e nlaunch -PIE -T ldscript
OBJCOPY  = arm-none-eabi-objcopy
TNCN = nlaunch_classic.tns
TNCP = preloader_classic.tns
CFILES    = nlaunch.c patch.c patch_other.c ndless.c
HFILES    = nlaunch.h boot2.h
BUILDTNCO = buildtnco

all: classic clean

classic: ../CLASSIC/nlaunch.tnc ../CLASSIC/nlaunch.tno ../CLASSIC/nlaunch.tns

../CLASSIC/nlaunch.tnc: $(TNCN) $(TNCP) $(BUILDTNCO)
	./$(BUILDTNCO) -clr $(TNCN) $(TNCP) $@

../CLASSIC/nlaunch.tno: $(TNCN) $(TNCP) $(BUILDTNCO)
	./$(BUILDTNCO) -clr $(TNCN) $(TNCP) $@

../CLASSIC/nlaunch.tns: $(TNCN) $(TNCP) $(BUILDTNCO)
	./$(BUILDTNCO) -clr $(TNCN) $(TNCP) $@

nlaunch_classic.o: $(CFILES) $(HFILES)
	$(GCC) $(GCCFLAGS) -o $@ -c $<

preloader_classic.o: preloader.c $(HFILES)
	$(GCC) $(GCCFLAGS) -o $@ -c $<

$(TNCN): nlaunch_classic.o
	$(LD) $(LDFLAGS) $^ -o $(@:.tns=.elf)
	$(OBJCOPY) -O binary   $(@:.tns=.elf) $@

$(TNCP): preloader_classic.o
	$(LD) $(LDFLAGS) $^ -o $(@:.tns=.elf)
	$(OBJCOPY) -O binary   $(@:.tns=.elf) $@

$(BUILDTNCO): buildtnco.c
	$(HOST_GCC) -Wall -Wextra -Werror -Os -o $@ $<

clean:
	rm -f *.o *.elf *.tns
	rm -f $(BUILDTNCO)
