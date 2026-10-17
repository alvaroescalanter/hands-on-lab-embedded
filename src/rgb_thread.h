#ifndef RGB_THREAD_H // Evitar inclusión múltiple
#define RGB_THREAD_H

#include <zephyr/kernel.h>

/*Enum para los casos que tenemos*/
typedef enum {
    RGB_BLUE,
    RGB_RED,
    RGB_YELLOW,
    RGB_GREEN,
    RGB_OFF,
    RGB_NONE
} rgb_msg_t;

/* Shared RGB message queue */
extern struct k_msgq rgb_msgq;

/*Declaración de funciones para el control del LED RGB*/
void rgb_init(void);
void rgb_set_blue(void);
void rgb_set_red(void);//
void rgb_set_green(void);
void rgb_set_yellow(void);
void rgb_set_off(void);

/*Funvcion del hilo RGB*/
void rgb_thread_func(void *p1, void *p2, void *p3);

#endif