#include "fsm.h"

// Estado actual del sistema (solo visible dentro de este archivo)
static enum system_mode current_mode = MODE_NORMAL;

void fsm_set_mode(enum system_mode mode) // Cambiar el modo del sistema
{
    current_mode = mode; // Actualizar el estado actual del sistema
}

enum system_mode fsm_get_mode(void) // Obtener el modo actual del sistema
{
    return current_mode; // Retornar el estado actual del sistema
}
