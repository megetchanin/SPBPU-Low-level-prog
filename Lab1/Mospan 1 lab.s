.equ GPIO_LEDs,   0xF0000000
.equ GPIO_SWs,    0xF0000004

.text
.globl _start

_start:
    # Чтение состояния ключей
    li      t0, GPIO_SWs
    lw      t1, 0(t0)

    # Проверка ключа 4 (бит 3)
    addi    t2, zero, 8
    and     t2, t1, t2
    addi    t3, zero, 0
    bne     t2, t3, turn_off

    # Инициализация цвета
    addi    t3, zero, 0

    # Ключ 1 (красный) - бит 0
    addi    t2, zero, 1
    and     t2, t1, t2
    addi    t4, zero, 0
    bne     t2, t4, add_red
    j       check_blue

add_red:
    li      t4, 0xFF0000
    or      t3, t3, t4

check_blue:
    # Ключ 2 (синий) - бит 1
    addi    t2, zero, 2
    and     t2, t1, t2
    addi    t4, zero, 0
    bne     t2, t4, add_blue
    j       check_green

add_blue:
    li      t4, 0x0000FF
    or      t3, t3, t4

check_green:
    # Ключ 3 (зеленый) - бит 2
    addi    t2, zero, 4
    and     t2, t1, t2
    addi    t4, zero, 0
    bne     t2, t4, add_green
    j       set_led

add_green:
    li      t4, 0x00FF00
    or      t3, t3, t4

set_led:
    # Запись цвета в светодиод
    li      t0, GPIO_LEDs
    sw      t3, 0(t0)

    j       _start

turn_off:
    # Выключение светодиода
    li      t0, GPIO_LEDs
    addi    t3, zero, 0
    sw      t3, 0(t0)

end:
    j       end