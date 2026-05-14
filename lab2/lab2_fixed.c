#define UART_ADDR 0x10013000

void _start(void) {
    volatile char *uart = (volatile char *)UART_ADDR;
    
    // Выводим приветствие
    char *msg = "Hello from Frygat! Lab2\nEnabling timer...\nWaiting...\nTimer OK!\nProgram finished!\n";
    while (*msg) {
        *uart = *msg++;
    }
    
    // Бесконечный цикл
    while(1);
}
