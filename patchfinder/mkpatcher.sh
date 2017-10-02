#!/bin/bash
if [ "$#" != "3" ]; then
	echo "usage: $0 <decrypted OS dump> <output.tns> <N or C for non-cas/cas>"
	exit 1
fi
echo "Compiling patchfinder"
javac Find900a0000.java
rm -f patches.c cut odump.txt a.elf part2 "$2"
echo "Cut OS out of RAM dump"
javac CutOS.java
java CutOS "$1" cut
echo "Disassembling OS dump"
arm-none-eabi-objdump -EL -D -b binary --adjust-vma=0x10000000 -m arm -M reg-names-raw cut > odump.txt
echo "Getting OS version check value"
echo "#define OS_VALUE_CHECK 0x`dd if=cut bs=4 count=1 skip=8 2>/dev/null | hexdump | head -n 1 | awk '{print $5$4$3$2}'`" > patches.c
cat begin.c >> patches.c
echo "Finding CAS patches"
java Find900a0000 odump.txt "$3" >> patches.c
cat end.c >> patches.c
echo "Compiling patcher"
arm-none-eabi-gcc -mcpu=arm926ej-s -nostdlib -fPIC -ffreestanding -Os patches.c -o a.elf
arm-none-eabi-objcopy --only-section=.text --output-target binary a.elf part2
(printf "PRG\0"; cat part2) > "$2"
#rm -f cut odump.txt a.elf part2 *.class
echo "Done!"
