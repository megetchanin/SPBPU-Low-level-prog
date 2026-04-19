# Лабораторная работа №2

## Требования

- riscv64-unknown-elf-gcc (с поддержкой rv32)
- qemu-system-riscv32
- make

## Сборка и запуск
make        
make run    

Для отладки через GDB:
make debug  

Очистка артефактов сборки:
make clean


## Ожидаемый вывод
Hello world from Danila Rylov!!!
TIMER_IRQ!
Program finished

Между первой и последующими строками — пауза ~5 секунд (срабатывание таймера).
