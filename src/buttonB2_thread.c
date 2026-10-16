#include <zephyr/sys/printk.h>
#include <zephyr/drivers/gpio.h>
#include "buttonB2_thread.h"


/*Definicion del botón B2*/
#define BUTTON_NODE DT_ALIAS(sw1)
static const struct gpio_dt_spec buttonB2 = GPIO_DT_SPEC_GET(BUTTON_NODE, gpios);

/*Estructura para que Zephyr */
static struct gpio_callback buttonB2_cb;

/*Semaforos*/
K_SEM_DEFINE(buttonB2_sem,0,1);
K_SEM_DEFINE(debounceB2_sem,0,1);

/*Prototipo callback botón B2 */
static void buttonB2_isr(const struct device *port, struct gpio_callback *cb, gpio_port_pins_t pins);
/*Prototipo Callback Timer */
static void debounceB2_timer_callback(struct k_timer *timer);

/* Definicion del timer */
K_TIMER_DEFINE(debounceB2_timer, debounceB2_timer_callback, NULL);


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
     gpio_pin_interrupt_configure_dt(&buttonB2, GPIO_INT_EDGE_TO_ACTIVE);


    while (1) {

        /* Wait for timer signal */
        k_sem_take(&debounceB2_sem, K_FOREVER);
        //Mandar el semaforo al hilo main para avisar del cambio de modo
        if (gpio_pin_get_dt(&buttonB2) == 1) {
            k_sem_give(&buttonB2_sem);
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