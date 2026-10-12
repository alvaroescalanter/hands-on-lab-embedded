#include <zephyr/sys/printk.h>
#include <zephyr/kernel.h>
#include "led_control.h"   // Importamos tu módulo de LEDs
#include "fsm.h" // Imp módulo de FSM para obtener el modo actual del sistema

void main(void)
{
    printk("Sistema iniciado\n");

    // Inicializar el LED RGB usando tu módulo
    led_init();

    // Encender un color inicial R - G - B - Y
    fsm_set_mode(MODE_NORMAL);

    // Actualizar LED según el modo
    led_update_mode();

    while (1) {
        printk("Modo actual: %d\n", fsm_get_mode());
        k_sleep(K_SECONDS(1));
    }
}