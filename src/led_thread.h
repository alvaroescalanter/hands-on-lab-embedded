#ifndef LED_THREAD_H // Evitar inclusión múltiple
#define LED_THREAD_H
#include <stdint.h> // Incluir stdint.h para tipos de enteros estándar

typedef enum {// Enumeración para los modos del sistema
    MODE_NORMAL,
    MODE_BLUE,
    MODE_OFF
} system_mode_t;

// Estructura para los mensajes de la cola del hilo LED
typedef struct {
    system_mode_t mode;
    int16_t adc_value;   // Solo se usa en NORMAL
} led_msg_t;

// Declaración de funciones para el control de LEDs
void led_init(void);
void led_set_blue(void);
void led_set_red(void);//
void led_set_green(void);
void led_set_yellow(void);
void led_set_off(void);

extern struct k_msgq led_msgq;// Declaración del mensaje de cola para el hilo LED

void led_thread_func(void *p1, void *p2, void *p3);// Declaración de la función del hilo LED


#endif
