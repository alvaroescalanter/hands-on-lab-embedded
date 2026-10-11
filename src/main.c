#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
// INICIO DE CODIGO PARA PRENDER LOS LEDS RGB
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <errno.h>
// Obtener los nodos de los LEDs desde el Device Tree
#define LED_RED_NODE DT_ALIAS(led_red)
#define LED_GREEN_NODE DT_ALIAS(led_green)
#define LED_BLUE_NODE DT_ALIAS(led_blue)
// Crear estructuras de especificación GPIO para cada LED
static const struct gpio_dt_spec led_red =
GPIO_DT_SPEC_GET(LED_RED_NODE, gpios);
static const struct gpio_dt_spec led_green =
GPIO_DT_SPEC_GET(LED_GREEN_NODE, gpios);
static const struct gpio_dt_spec led_blue =
GPIO_DT_SPEC_GET(LED_BLUE_NODE, gpios);
