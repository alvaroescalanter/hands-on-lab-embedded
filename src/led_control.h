#ifndef LED_CONTROL_H // Evitar inclusión múltiple
#define LED_CONTROL_H
// Declaración de funciones para el control de LEDs
void led_init(void);
void led_set_blue(void);
void led_set_red(void);
void led_set_green(void);
void led_set_yellow(void);
void led_set_off(void);
// Función para actualizar el estado del LED según el modo del sistema
void led_update_mode(void);


#endif
