#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "led_thread.h"   // Importamos tu módulo de LEDs

/* FSM states */
typedef enum {
    ST_NORMAL,
    ST_BLUE
} estado_t;

/* Initial state */
static estado_t estadoActual = ST_NORMAL;

int main(void)
{
    printk("Embedded Platforms and Communications for IoT\n");
    printk("Hands-on Exercise - Brightness Control\n");

    while (1) {

        switch (estadoActual) {

                case ST_NORMAL:
                    // Normal mode
                    break;

                case ST_BLUE:
                    // Blue mode
                    break;

                default:
                    break;
        }

    }

    return 0;
}
