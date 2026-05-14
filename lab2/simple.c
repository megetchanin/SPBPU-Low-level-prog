void _start(void) {
    volatile char *uart = (volatile char *)0x10010000;
    uart[0] = 'H';
    uart[0] = 'i';
    uart[0] = '\n';
    while(1);
}
