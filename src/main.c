#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "adc_thread.h"
#include "buttonB2_thread.h"
#include "rgb_thread.h"

/* FSM states */
typedef enum {
    ST_NORMAL,
    ST_BLUE,
    ST_OFF
} estado_t;

/* Initial state */
static estado_t estadoActual = ST_NORMAL;

/*Variable para la colas de mensajes */
static adc_msg_t adcMessage_p = {0};
static rgb_msg_t rgbMessage_p = {0};

/*Para pasar de enum a string*/
static const char *rgb_color_names[] = {
    "BLUE",
    "RED",
    "YELLOW",
    "GREEN",
    "OFF"
};

/*Prototipo Callback Timer */
static void measurement_timer_callback(struct k_timer *timer);


/* Definicion del timer */
K_TIMER_DEFINE(measurement_timer, measurement_timer_callback, NULL);


int main(void)
{
    printk("Embedded Platforms and Communications for IoT\n");
    printk("Hands-on Exercise - Brightness Control - Alejandro y Álvaro\n\n");


    /*Variable porcentaje luminosidad. Rango: 0 (nada de luz) - 100 (mucha luz)*/
    int ldr_percentage;
    rgb_msg_t current_color = RGB_NONE;

    /* Initial state: NORMAL */
    k_timer_start(&measurement_timer, K_SECONDS(2), K_SECONDS(2));

    while (1) {

        switch (estadoActual) {

                case ST_NORMAL:

                    /* Espera un mensaje de la cola */
                    if(!k_msgq_get(&adc_queue,&adcMessage_p,K_NO_WAIT)){

                        /* Coge el valor raw y lo pasa a % con el full scale range*/
                        ldr_percentage = 100 - (adcMessage_p.adc_raw * 100) / 4095;
                        printk("------------MODO NORMAL-------------\n");

                        /*Acciones RGB en funcion del valor leido*/
                        if (ldr_percentage < 33) {
                            rgbMessage_p = RGB_RED;
                        }
                        else if (ldr_percentage <= 66) {
                            rgbMessage_p = RGB_YELLOW;
                        }
                        else {
                            rgbMessage_p = RGB_GREEN;
                        }

                         /*Meter mensaje en la cola RGB solo si hay un cambio*/
                        if (rgbMessage_p != current_color) {
                            k_msgq_put(&rgb_msgq, &rgbMessage_p, K_FOREVER);
                            current_color = rgbMessage_p;
                        }

                        printk("Brightness: %d%% | RGB Color: %s\n",ldr_percentage, rgb_color_names[current_color]);
                        printk("Short press B2 to enter BLUE mode.\n");
                        printk("Long press B2 to enter OFF mode.\n\n");
                    }

                    /* Semafoto de cambio de Modo*/
                    if(!k_sem_take(&buttonB2_sem, K_NO_WAIT)){
                        k_timer_stop(&measurement_timer);
                        rgbMessage_p = RGB_BLUE;
                        k_msgq_put(&rgb_msgq, &rgbMessage_p, K_FOREVER);
                        current_color = RGB_BLUE;
                        printk("------------BLUE MODE------------\n");
                        printk("RGB Color: BLUE\n");
                        printk("ADC sampling paused.\n\n");
                        printk("Short press B2 to enter NORMAL mode.\n");
                        printk("Long press B2 to enter OFF mode.\n\n");
                        estadoActual = ST_BLUE;
                    }

                    //IF pulsacion larga -> MODO OFF
                    if(!k_sem_take(&buttonB2_sem_long, K_NO_WAIT)){
                        k_timer_stop(&measurement_timer);
                        printk("------------MODE OFF------------\n");
                        printk("RGB Color: NONE\n");
                        printk("ADC sampling paused.\n\n");
                        printk("Short press B2 to enter NORMAL mode.\n");
                        printk("Long press B2 to enter OFF mode.\n\n");
                        rgbMessage_p = RGB_OFF;
                        if (k_msgq_put(&rgb_msgq, &rgbMessage_p, K_FOREVER) == 0) {
                        current_color = RGB_OFF;
                        }
                        estadoActual = ST_OFF;
                    }

                    break;

                case ST_BLUE:
                    // Blue mode
                    /* Semafoto de cambio de Modo*/
                    if (!k_sem_take(&buttonB2_sem, K_NO_WAIT)) {
                        rgbMessage_p = RGB_OFF;
                        k_msgq_put(&rgb_msgq, &rgbMessage_p, K_FOREVER);
                        current_color = RGB_OFF;
                        
                        k_timer_start(&measurement_timer, K_SECONDS(2), K_SECONDS(2));
                        estadoActual = ST_NORMAL;
                    }

                    //If pulsacion larga -> MODO OFF
                    if(!k_sem_take(&buttonB2_sem_long, K_NO_WAIT)){
                        printk("------------MODE OFF------------\n");
                        printk("RGB Color: NONE\n\n");
                        printk("Short press B2 to enter NORMAL mode.\n\n");
                        rgbMessage_p = RGB_OFF;
                        if (k_msgq_put(&rgb_msgq, &rgbMessage_p, K_FOREVER) == 0) {
                            current_color = RGB_OFF;
                        }
                        estadoActual = ST_OFF;
                    }

                    break;

                    case ST_OFF:
                    //MODE OFF
                    /* Semafoto de cambio de Modo*/
                    if (!k_sem_take(&buttonB2_sem, K_FOREVER)) {
                        k_timer_start(&measurement_timer, K_SECONDS(2), K_SECONDS(2));
                        estadoActual = ST_NORMAL;
                    }

                    break;

                default:
                    break;
            
        }

        k_msleep(10);
    }
    
    return 0;
}

static void measurement_timer_callback(struct k_timer *timer){

    ARG_UNUSED(timer);
    //Mandar el semaforo al hilo del ADC
    k_sem_give(&adc_trigger_sem);
}
