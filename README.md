# TI-Nspire

A collection of tools and exploits for TI-Nspire calculators. 

## nLoader

Custom bootloader for the TI-Nspire CX (not monochrome or CX II). 

See [nLoader/README.txt](../../raw/main/nLoader/README.txt).

## MyNlaunchyCXM

Modified version of [nLaunchy CXM](https://tiplanet.org/forum/archives_voir.php?id=19507), with:

* No support for CX/CM (monochrome calculators only)
* No PP1234567 on the screen during boot
* Better progress messages
* Boot2 version in about screen spoofed as 3.0.1.131
* If OS fails to load, it launches download mode
* Touchpad fix for EVT touchpad/SimpKey keypads (in CLASSIC_TOUCHPAD_FIX_EVT folder; untested)

Install using the instructions in  [MyNlaunchyCXM/readme.md](MyNlaunchyCXM/readme.md). Parts of it are outdated, but the installation instructions still work.

## tiny_overclock_non_cx

Minuscule Ndless program (for OS 3.1+) to overclock monochrome calculators to 150MHz. 

Put it in the `ndless/startup` folder or just run it once. There is no UI.

## touchpad_proto_fix

A permanent fix for the keyboard not working correctly on OS 2.1+ on **prototype** TI-Nspire CAS Touchpad calculators.

See: https://tiplanet.org/forum/viewtopic.php?t=21050

