#include <zephyr/sys/printk.h>
#include <zephyr/drivers/gpio.h>
#include <errno.h>
#include "rgb_thread.h"

/* Obtener los nodos RGB desde el Device Tree */
#define RGB_RED_NODE DT_ALIAS(rgb_red)
#define RGB_GREEN_NODE DT_ALIAS(rgb_green)
#define RGB_BLUE_NODE DT_ALIAS(rgb_blue)

/* Crear estructuras de configuración GPIO para cada color RGB */
static const struct gpio_dt_spec rgb_red = GPIO_DT_SPEC_GET(RGB_RED_NODE, gpios);
static const struct gpio_dt_spec rgb_green = GPIO_DT_SPEC_GET(RGB_GREEN_NODE, gpios);
static const struct gpio_dt_spec rgb_blue = GPIO_DT_SPEC_GET(RGB_BLUE_NODE, gpios);

static rgb_msg_t rgbMessage = {0};

K_MSGQ_DEFINE(rgb_msgq, sizeof(rgb_msg_t), 10, 4);

K_THREAD_DEFINE(rgb_thread, 1024, rgb_thread_func,
                NULL, NULL, NULL,
                5, 0, 0);

// Función para actualizar el estado del LED según el modo del sistema
void rgb_thread_func(void *p1, void *p2, void *p3)
{

    rgb_init();

    while (1) {

        /* Espera un mensaje de la cola */
        if(!k_msgq_get(&rgb_msgq, &rgbMessage, K_FOREVER)){

          // Actualizar el estado del LED según el modo recibido
            switch (rgbMessage) {

                case RGB_BLUE:
                        rgb_set_blue();
                   break;

                case RGB_RED:
                        rgb_set_red();
                   break;

                case RGB_YELLOW:
                        rgb_set_yellow();
                   break;

                case RGB_GREEN:
                        rgb_set_green();
                    break;

                 case RGB_OFF:
                        rgb_set_off();
                    break;

                default:
                   break;
            }


        }

    }
}

void rgb_init(void)
{
    // Verificar que los GPIO están listos
    if (!gpio_is_ready_dt(&rgb_red) ||
        !gpio_is_ready_dt(&rgb_green) ||
        !gpio_is_ready_dt(&rgb_blue)) {
        
        printk("Error: LED RGB GPIO no listo\n"); 
        return; // Retornar sin inicializar si algún GPIO no está listo
    }
    /*Inicialmente apagados*/
    gpio_pin_configure_dt(&rgb_red, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&rgb_green, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&rgb_blue, GPIO_OUTPUT_INACTIVE);

}

void rgb_set_blue(void)
{
    gpio_pin_set_dt(&rgb_red,   0);
    gpio_pin_set_dt(&rgb_green, 0);
    gpio_pin_set_dt(&rgb_blue,  1);
}

void rgb_set_red(void)
{
    gpio_pin_set_dt(&rgb_red,   1);
    gpio_pin_set_dt(&rgb_green, 0);
    gpio_pin_set_dt(&rgb_blue,  0);
}

void rgb_set_green(void)
{
    gpio_pin_set_dt(&rgb_red,   0);
    gpio_pin_set_dt(&rgb_green, 1);
    gpio_pin_set_dt(&rgb_blue,  0);
}

void rgb_set_yellow(void)
{
    gpio_pin_set_dt(&rgb_red,   1);
    gpio_pin_set_dt(&rgb_green, 1);
    gpio_pin_set_dt(&rgb_blue,  0);
}

void rgb_set_off(void)
{
    gpio_pin_set_dt(&rgb_red,   0);
    gpio_pin_set_dt(&rgb_green, 0);
    gpio_pin_set_dt(&rgb_blue,  0);
}
/*Alternativa para menos sentencias 
void rgb_set_color(int red, int green, int blue)
{
    gpio_pin_set_dt(&rgb_red, red);
    gpio_pin_set_dt(&rgb_green, green);
    gpio_pin_set_dt(&rgb_blue, blue);
}
    void rgb_set_yellow(void)
{
    rgb_set_color(1, 1, 0);
}
*/