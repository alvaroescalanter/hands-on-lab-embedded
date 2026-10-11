#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
// INICIO DE CODIGO PARA PRENDER UN LED EN EL PROTOBOARD
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <errno.h>
#define LED_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led =
GPIO_DT_SPEC_GET(LED_NODE, gpios);
int main(void)
{
if (!gpio_is_ready_dt(&led)) {
return-ENODEV;
}
int ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
if (ret < 0) {
return ret;
}
gpio_pin_set_dt(&led, 1);
return 0;
}