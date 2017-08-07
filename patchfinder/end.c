	unsigned dummy;
	__asm volatile(
		"0: mrc p15, 0, r15, c7, c10, 3 @ test and clean DCache \n"
		" bne 0b \n"
		" mov %0, #0 \n"
		" mcr p15, 0, %0, c7, c7, 0 @ invalidate ICache and DCache \n" : "=r" (dummy));
}