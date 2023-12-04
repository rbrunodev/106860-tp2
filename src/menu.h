#ifndef MENU_H_
#define MENU_H_

#include <stdbool.h>

typedef struct menu menu_t;

typedef enum {
	MENU_ERROR,
	OPCION_INEXISTENTE,
	MENU_OK,
	MENU_SALIR
} MENU_RESULTADO;

menu_t *menu_crear(void);

menu_t *menu_agregar_opcion(menu_t *menu, char *opcion, char *comando,
			    bool (*funcion)(void *));

MENU_RESULTADO menu_mostrar(menu_t *menu, char *comando, void *extra);

void menu_destruir(menu_t *menu);

#endif // MENU_H_
