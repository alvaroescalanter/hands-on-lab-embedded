#ifndef BUTTON_THREAD_H
#define BUTTON_THREAD_H

#include <zephyr/kernel.h>

/* Shared semaphore: Button thread -> Main */
extern struct k_sem buttonB2_sem;

/* Button B2 thread function */
void buttonB2_thread_func(void *p1, void *p2, void *p3);

#endif /* BUTTON_THREAD_H */