#include <zephyr/sys/printk.h>
#include <zephyr/kernel.h>
#include "led_control.h"   // Importamos tu módulo de LEDs

void main(void)
{
    printk("Sistema iniciado\n");

    // Inicializar el LED RGB usando tu módulo
    led_init();

    // Encender un color inicial (por ejemplo azul)
    led_set_red();

    while (1) {
        printk("main.c funcionando...\n");
        k_sleep(K_SECONDS(1));
    }
}
