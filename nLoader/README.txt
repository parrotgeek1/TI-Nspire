nLoader is a custom bootloader for the TI-Nspire CX (not monochrome or CX II).
It enables loading Ndless on boot ("untethered jailbreak"), skipping OS integrity checks, launching arbitrary unsigned Boot2 images, and downgrading to arbitrary OS versions.

Compilation Requirements: 
UNIX-like environment, arm-none-eabi toolchain in your $PATH, bash, python3, zip, unzip, awk, binutils, xxd (sometimes in vim-common package)
Tested with https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads version gcc-arm-11.2-2022.02

make_for_nMaker.sh will produce an image which can be provided to nMaker along with tinspirecx_boot2_4.4.0.8.img.tns or tinspirecx_boot2_4.5.0.14.img.tns (only).
make_for_emulator.sh requires tinspirecx_boot2_4.5.0.14.img.tns to be in the boot1.5_exploit folder (you can edit the script to use 4.4.0.8 instead). It will produce an image which can be directly provided to Firebird or nspire-emu as a boot2 image.

nLoader exploits an amusingly simple vulnerability in how Boot1.5 loads Boot2 images, which was fixed (after nLoader's release) in Boot1.5 4.5, bundled with OS 4.5.1.

Boot images for the TI-Nspire have a header that specifies, among other things, their base address and whether the contents are compressed.
Normally, Boot2 images are compressed, and Boot1.5 decompresses them to a temporary buffer before verifying their signature and copying them to the specified base address.
All seems good, right? Well, what about uncompressed images?
I have never seen an official TI boot image that is uncompressed, so that code path is likely not well-tested.
In that case, Boot1.5 directly copies the image data to the specified base address before checking the signature, which is a reasonable optimization.
This is identical to the flow in Boot1. It made sense for Boot1, because Boot1 is running from either the SoC's ROM or NOR flash, and not SDRAM, and is therefore read-only.
However, there is a simple precaution TI failed to take when adapting this code to Boot1.5: the address is never checked for overlap with the running Boot1.5!
Boot1.5 has a base address of 0x11200000, so if you make a boot2 image that overlaps with 0x11200000, it will overwrite Boot1.5's code in RAM while it is running and run your code instead.

One non-obvious thing to me during the development of this exploit was how to copy the entire loader into RAM before Boot1.5 crashes due to being overwritten. I realized I could set the base address below 0x11200000, and align the data in the file such that 0x11200000 was still correctly overwritten. To avoid including a small piece of copyrighted Boot1.5 code in the exploit images, I simply overwrote the beginning of Boot1.5 with a nop sled followed by a jump to the address of the loader.

To find the size of the nop sled for a given Boot1.5 version:
In an emulator, feed boot1.5 an image of all A's with base address 0x11200000 (use mkexp.sh to create it).
It will crash with "undefined instruction 41414141".
The nop sled size is (crashed_PC - 0x11200000).
I used 0xE1A00000 (the real nop instruction) but zeros would probably work too.

My code to jump to the loader had to be exactly 8 bytes, otherwise it would crash. I don't know why this is, but I would guess it has to do with CPU caches.

On Firebird emulating HW-W it doesn't work; it restarts the exploit, and then claims "wrong boot2 version". It works on real hardware.
