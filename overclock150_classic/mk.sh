rm -f a.elf part2
arm-none-eabi-gcc -mcpu=arm926ej-s -nostdlib -fPIC -ffreestanding -Os oc.c -o a.elf
arm-none-eabi-objcopy --only-section=.text --output-target binary a.elf part2
(printf "PRG\0"; cat part2) > "overclock150.tns"
rm -f a.elf part2