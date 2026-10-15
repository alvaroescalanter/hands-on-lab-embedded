#include <zephyr/sys/printk.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <errno.h>
#include "led_thread.h"   // Importamos tu módulo de LEDs

// Obtener los nodos de los LEDs desde el Device Tree
#define LED_RED_NODE DT_ALIAS(led_red)
#define LED_GREEN_NODE DT_ALIAS(led_green)
#define LED_BLUE_NODE DT_ALIAS(led_blue)

// Crear estructuras de especificación GPIO para cada LED
static const struct gpio_dt_spec led_red = GPIO_DT_SPEC_GET(LED_RED_NODE, gpios);
static const struct gpio_dt_spec led_green = GPIO_DT_SPEC_GET(LED_GREEN_NODE, gpios);
static const struct gpio_dt_spec led_blue = GPIO_DT_SPEC_GET(LED_BLUE_NODE, gpios);

K_MSGQ_DEFINE(led_msgq, sizeof(led_msg_t), 10, 4);

void led_init(void)
{
    // Verificar que los GPIO están listos
    if (!gpio_is_ready_dt(&led_red) ||
        !gpio_is_ready_dt(&led_green) ||
        !gpio_is_ready_dt(&led_blue)) {
        
        printk("Error: LED GPIO no listo\n"); 
        return; // Retornar sin inicializar si algún GPIO no está listo
    }

    gpio_pin_configure_dt(&led_red, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&led_green, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&led_blue, GPIO_OUTPUT_INACTIVE);

    // Apagar todos los LEDs (ánodo común → HIGH = apagado)
    led_set_off();
}
void led_set_blue(void)
{
    gpio_pin_set_dt(&led_red,   1);
    gpio_pin_set_dt(&led_green, 1);
    gpio_pin_set_dt(&led_blue,  0);
}

void led_set_red(void)
{
    gpio_pin_set_dt(&led_red,   0);
    gpio_pin_set_dt(&led_green, 1);
    gpio_pin_set_dt(&led_blue,  1);
}

void led_set_green(void)
{
    gpio_pin_set_dt(&led_red,   1);
    gpio_pin_set_dt(&led_green, 0);
    gpio_pin_set_dt(&led_blue,  1);
}

void led_set_yellow(void)
{
    gpio_pin_set_dt(&led_red,   0);
    gpio_pin_set_dt(&led_green, 0);
    gpio_pin_set_dt(&led_blue,  1);
}

void led_set_off(void)
{
    gpio_pin_set_dt(&led_red,   1);
    gpio_pin_set_dt(&led_green, 1);
    gpio_pin_set_dt(&led_blue,  1);
}
// Función para actualizar el estado del LED según el modo del sistema
void led_thread_func(void *p1, void *p2, void *p3)
{
    led_msg_t msg;

    while (1) {
        // Esperar un mensaje en la cola
        k_msgq_get(&led_msgq, &msg, K_FOREVER);

        // Actualizar el estado del LED según el modo recibido
        switch (msg.mode) {

        case MODE_NORMAL:
            if (msg.adc_value < 1365) {          // <33%
                led_set_red();
            } else if (msg.adc_value < 2730) {   // 33–66%
                led_set_yellow();
            } else {                             // >66%
                led_set_green();
            }
            break;

        case MODE_BLUE:
            led_set_blue();
            break;

        case MODE_OFF:
            led_set_off();
            break;
        }
    }
}
K_THREAD_DEFINE(led_thread, 1024, led_thread_func,
                NULL, NULL, NULL,
                5, 0, 0);// Definición del hilo LED con prioridad 5 y sin opciones especiales