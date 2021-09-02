void _start() {
	*(volatile unsigned*)0x900B0000 = 0x00000002; //CPU_SPEED_150MHZ
	*(volatile unsigned*)0x900B000C = 4;
}
