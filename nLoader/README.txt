THE CODE HERE IS NOT FINISHED YET. PLEASE WAIT UNTIL 1.1 IS RELEASED OFFICIALLY BEFORE USING IT.
nLoader
=======

nLoader is a custom bootloader for the TI-Nspire CX (not monochrome or CX II).
It enables loading Ndless on boot ("untethered jailbreak"), skipping OS integrity checks, launching arbitrary unsigned Boot2 images, and downgrading to arbitrary OS versions via the boot-time maintenance menu.

See this tutorial on how to install nLoader: https://tiplanet.org/forum/viewtopic.php?t=21094
Important updates to the tutorial:
* It's recommended to replace tinspirecx_boot2_4.4.0.8.img with tinspirecx_boot2_4.5.0.14.img.
* PLEASE PAY ATTENTION TO THE ORDER OF STEP D4 AND D5. nMaker has a bug causing it not to display the on-screen prompts correctly sometimes.
* It is now possible to downgrade the OS via the maintenance menu, despite the tutorial claiming otherwise.

If you are on OS 4.5.5 or newer, you can downgrade your OS or install Ndless by creating an nLoader image file directly using make_for_emulator.sh, then flashing it with a 3.3V USB to serial adapter: https://tiplanet.org/forum/viewtopic.php?p=219587#p219587

To use an OS older than 3.6, install a newer OS, then use nSwitchOS: https://tiplanet.org/forum/archives_voir.php?id=877138

If your calculator reboots when you try to calculate something, you've hit a heavy protection TI added to the OS to prevent changing the model of a calculator. nLoader does not work around this, for ethical reasons; see https://parrotgeek.com/nloader_note.html.

Compilation requirements
========================

UNIX-like environment with /dev/zero and /dev/null, arm-none-eabi toolchain in your $PATH, awk (The One True Awk version 20200816 tested), bash (3.2.57 tested), dd, dirname, GNU make (3.81 tested), printf (the command), Python 3 (3.9.6 tested), tr, xxd
Most of these are installed by default in any Unix-like OS, except possibly GNU make, Python 3 and xxd (which is sometimes confusingly contained in the vim-common package).
Tested environment: macOS 13.2 (Intel) with arm-gnu-toolchain-12.2.rel1-darwin-x86_64-arm-none-eabi.pkg from https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads and Xcode 14.3

make_for_nMaker.sh will produce an image which can be provided to nMaker along with tinspirecx_boot2_4.4.0.8.img.tns or tinspirecx_boot2_4.5.0.14.img.tns (only).
make_for_emulator.sh requires tinspirecx_boot2_4.5.0.14.img.tns and boot1.5_4.4.img.tns to be in this folder (you can edit the script to use 4.4.0.8 instead). It will produce an image which can be directly provided to Firebird or nspire-emu as a boot2 image.

Warnings like "arm-none-eabi-ld: warning: foo.elf has a LOAD segment with RWX permissions" are harmless (and irrelevant in the bare-metal environment nLoader runs in).

Exploit explanation
===================

nLoader exploits an amusingly simple vulnerability in how Boot1.5 loads Boot2 images, which was fixed (after nLoader's release) in Boot1.5 4.5, bundled with OS 4.5.1.

Boot images for the TI-Nspire have a header that specifies, among other things, their base address and whether the contents are compressed.
Normally, Boot2 images are compressed, and Boot1.5 decompresses them to a temporary buffer before verifying their signature and copying them to the specified base address.
All seems good, right? Well, what about uncompressed images?
I have never seen an official TI boot image that is uncompressed, so that code path is likely not well-tested.
For uncompressed images, Boot1.5 directly copies the image data to the specified base address before checking the signature, which is a reasonable optimization.
This is identical to the flow in Boot1. It made sense for Boot1, because Boot1 is running from either the SoC's ROM or NOR flash, and not SDRAM, and is therefore read-only.
However, there is a simple precaution TI failed to take when adapting this code to Boot1.5: the address is never checked for overlap with the running Boot1.5!
Boot1.5 has a base address of 0x11200000, so if you make a boot2 image that overlaps with 0x11200000, it will overwrite Boot1.5's code in RAM while it is running and run your code instead.

One non-obvious thing to me during the development of this exploit was how to copy the entire loader into RAM before Boot1.5 crashes due to being overwritten. I realized I could set the base address below 0x11200000, and align the data in the file such that 0x11200000 was still correctly overwritten. To avoid including a small piece of copyrighted Boot1.5 code in the exploit images, I simply overwrote the beginning of Boot1.5 with a nop sled followed by a jump to the address of the loader.

To find the size of the nop sled for a given Boot1.5 version:
In an emulator, feed boot1.5 an image of all A's with base address 0x11200000 (use mkexp.sh to create it).
It will crash with "undefined instruction 41414141".
The nop sled size is (crashed_PC - 0x11200000).
I used 0xE1A00000 (the real nop instruction) but zeros would probably work too.

The code after the nop sled to jump to the loader has to be no larger than 8 bytes, otherwise it will crash. I don't know why this is, but I would guess it has to do with CPU caches.

On Firebird emulating HW-W it doesn't work; it restarts the exploit by itself, and then claims "wrong boot2 version". I have no idea why this happens. It works on real hardware, as well as Firebird emulating HW-A and J.
