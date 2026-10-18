#include <zephyr/drivers/adc.h>
#include <zephyr/sys/printk.h>
#include <errno.h>
#include "adc_thread.h"

/* ADC1 - Channel 5 - PB1 (A0), defined in Devicetree */
#define ADC_NODE DT_ALIAS(analog_in_pin)
static const struct adc_dt_spec adc_ch = ADC_DT_SPEC_GET(ADC_NODE);

/* ADC measurement message queue */
K_MSGQ_DEFINE(adc_queue, sizeof(adc_msg_t), 10, 4);

/* ADC measurement message */
static adc_msg_t adcMessage = {0};

/* Semaphore: Triggers an ADC measurement every 2 seconds */
K_SEM_DEFINE(adc_trigger_sem, 0, 1);

 /* Define ADC thread at compile-time */
  K_THREAD_DEFINE(adc_thread,       /* Thread identifier */
                  1024,             /* Stack size */
                  adc_thread_func,  /* Entry function */
                  NULL, NULL, NULL, /* Parameters */
                  5,                /* Priority */
                  0,                /* Options */
                  0);               /* Starts immediately */


/* ADC thread function */
void adc_thread_func(void *p1, void *p2, void *p3)
{
        int ret;

        /* Check if ADC device is ready */
        if (!adc_is_ready_dt(&adc_ch)) {
            printk("Error: ADC device not ready\n");
            return;
        }

        /* Configure ADC channel */
        ret = adc_channel_setup_dt(&adc_ch);
        if (ret < 0) {
            printk("Error configuring ADC channel: %d\n", ret);
            return;
        }

        /* Define where the ADC sample will be stored */
        struct adc_sequence sequence = {
            .buffer = &adcMessage.adc_raw,
            .buffer_size = sizeof(adcMessage.adc_raw),
        };

        /* Initialize ADC sequence */
        ret = adc_sequence_init_dt(&adc_ch, &sequence);
        if (ret < 0) {
            printk("Error initializing ADC sequence: %d\n", ret);
            return;
        }

    while (1) {
        
        /* Wait for timer signal */
        k_sem_take(&adc_trigger_sem, K_FOREVER);

        /* Read ADC */
        ret = adc_read_dt(&adc_ch, &sequence);

        if (ret < 0) {
            printk("ADC read failed: %d\n", ret);
        } else {

            /* Send ADC measurement to main */
            ret = k_msgq_put(&adc_queue, &adcMessage, K_FOREVER);

            if (ret < 0) {
                printk("Error sending ADC measurement: %d\n", ret);
            }
        }
    }
}
