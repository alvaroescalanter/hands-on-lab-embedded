#include <zephyr/sys/printk.h>
#include <zephyr/drivers/gpio.h>
#include "buttonB2_thread.h"


#define BUTTON_NODE DT_ALIAS(sw1)
static const struct gpio_dt_spec buttonB2 = GPIO_DT_SPEC_GET(BUTTON_NODE, gpios);

/* GPIO callback structure for button B2 */
static struct gpio_callback buttonB2_cb;

/* Button press tracking flags */
static bool long_press_detected = false;
static bool button_pressed = false; // Prevents processing the same press multiple times

/* Semaphore: Signals a short button press */
K_SEM_DEFINE(buttonB2_sem, 0, 1);
/* Semaphore: Signals a long button press (>= 1 second) */
K_SEM_DEFINE(buttonB2_sem_long, 0, 1);
/* Semaphore: Triggers button debounce processing */
K_SEM_DEFINE(debounceB2_sem, 0, 1);

/*Prototipo callback botón B2 */
static void buttonB2_isr(const struct device *port, struct gpio_callback *cb, gpio_port_pins_t pins);
/*Prototipo Callback Timer */
static void debounceB2_timer_callback(struct k_timer *timer);
static void pulseLongB2_timer_callback(struct k_timer *timer);

/* Definicion del timer */
K_TIMER_DEFINE(debounceB2_timer, debounceB2_timer_callback, NULL);
/* Definicion del timer */
K_TIMER_DEFINE(pulseLongB2_timer,pulseLongB2_timer_callback, NULL);


 /* Define buttonB2 thread  */
  K_THREAD_DEFINE(buttonB2_thread,       /* Thread identifier */
                  1024,                  /* Stack size */
                  buttonB2_thread_func,  /* Entry function */
                  NULL, NULL, NULL,      /* Parameters */
                  5,                     /* Priority */
                  0,                     /* Options */
                  0);                    /* Starts immediately */

void buttonB2_thread_func(void *p1, void *p2, void *p3){

    /*Comprueba que el gpio esta listo. Configura el pin como entrada */
     if (!gpio_is_ready_dt(&buttonB2)) return;
     if (gpio_pin_configure_dt(&buttonB2, GPIO_INPUT) < 0) return;

     
     /*Inicializa la estructura de la callback: funcion a ejecutar y que pin atender*/
     gpio_init_callback(&buttonB2_cb, buttonB2_isr, BIT(buttonB2.pin)); 
     /*Registra la callback en el controlador GPIO, zephyr la llama cuando se produce la interrución*/
     gpio_add_callback_dt(&buttonB2, &buttonB2_cb); 
     /*Configura y habilita la interrupción del pin, indicando cuándo queremos que se dispare*/
     gpio_pin_interrupt_configure_dt(&buttonB2, GPIO_INT_EDGE_BOTH);


    while (1) {

        /* Wait for timer signal */
        k_sem_take(&debounceB2_sem, K_FOREVER);

        /* Button pressed */
        if (gpio_pin_get_dt(&buttonB2) == 1) {

            if (!button_pressed) {
                button_pressed = true; //Pulsacion en curso 
                long_press_detected = false;

                k_timer_start(&pulseLongB2_timer, K_MSEC(950), K_NO_WAIT);
            }
        }else {
            /* Button released */
                if (button_pressed) {
                    button_pressed = false;
                    k_timer_stop(&pulseLongB2_timer);

                    if (!long_press_detected) {
                        k_sem_give(&buttonB2_sem);
                    }
                }

        }

    }
}

static void buttonB2_isr(const struct device *port, struct gpio_callback *cb, gpio_port_pins_t pins){
        ARG_UNUSED(port);
        ARG_UNUSED(cb);
        ARG_UNUSED(pins);
        k_timer_start(&debounceB2_timer, K_MSEC(50), K_NO_WAIT);

}

static void debounceB2_timer_callback(struct k_timer *timer){
     ARG_UNUSED(timer);
     k_sem_give(&debounceB2_sem);

}
static void pulseLongB2_timer_callback(struct k_timer *timer){

    ARG_UNUSED(timer);
    /* Check if B2 is still pressed */
    if (gpio_pin_get_dt(&buttonB2) == 1) {
        long_press_detected = true;
        k_sem_give(&buttonB2_sem_long);
    }
}