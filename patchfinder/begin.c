#define put_word(A,B)   *(volatile unsigned int *)(A) = (B)
#define NOP 0x00000000

void _start() {
	if (*(volatile unsigned int *)0x10000020 != OS_VALUE_CHECK) {
		// red screen, thanks geekpersonman
		volatile unsigned char *scr_base = (*(void**)0xC0000010);
		volatile unsigned char *ptr;
		unsigned scr_size = 320*240*2;
		for (ptr = scr_base; ptr < scr_base + scr_size; ptr += 2) {
			*(volatile unsigned short*)ptr = 0b1111100000000000;
		}
		// delay
		for(int i = 0; i < 1000000; i++) { }
		return;
	}
