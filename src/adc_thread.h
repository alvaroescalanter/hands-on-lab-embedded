#ifndef ADC_THREAD_H
#define ADC_THREAD_H

#include <zephyr/kernel.h>

/* ADC measurement message */
typedef struct {
    int16_t adc_raw;
} adc_msg_t;

/* Shared message queue and semaphore */
extern struct k_msgq adc_queue;
extern struct k_sem adc_trigger_sem;

/* ADC thread function */
void adc_thread_func(void *p1, void *p2, void *p3);

#endif /* ADC_THREAD_H */