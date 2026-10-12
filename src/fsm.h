#ifndef FSM_H
#define FSM_H

// Estados del sistema
enum system_mode {
    MODE_NORMAL,
    MODE_BLUE,
    MODE_OFF
};

// Funciones públicas
void fsm_set_mode(enum system_mode mode); // Cambiar el modo del sistema
enum system_mode fsm_get_mode(void); // Obtener el modo actual del sistema

#endif
