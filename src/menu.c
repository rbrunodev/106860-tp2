#include "menu.h"
#include "hash.h"
#include <stdlib.h>


struct menu{
    hash_t *opciones;
};

typedef struct info_opcion{
    char *opcion;
    char *comando;
    bool (*funcion)(void*);
}info_opcion_t;

menu_t *menu_crear(void)
{
    menu_t *menu = calloc(1, sizeof(menu_t));
    if (!menu){
        return NULL;
    }
    menu->opciones = hash_crear(3);
    return menu;
}

menu_t *menu_agregar_opcion(menu_t *menu, char *opcion, char *comando ,bool (*funcion)(void *))
{
    if (!menu || !opcion || !funcion){
        return NULL;
    }

    info_opcion_t *info_opcion = malloc(sizeof(info_opcion_t));
    if (!info_opcion){
        return NULL;
    }

    info_opcion->opcion = opcion;
    info_opcion->comando = comando;
    info_opcion->funcion = funcion;

    hash_insertar(menu->opciones, comando, info_opcion, NULL);
    return menu;
}

MENU_RESULTADO menu_mostrar(menu_t *menu, char *comando, void *extra)
{
    if (!menu){
        return MENU_ERROR;
    }

    struct info_opcion *info_opcion = hash_obtener(menu->opciones, comando);
    if(!info_opcion){
        return OPCION_INEXISTENTE;
    }
   
    if(info_opcion->funcion(extra)){
        return MENU_OK;
    }

    return MENU_ERROR;
}

void menu_destruir(menu_t *menu)
{
    if (menu){
        free(menu);
    }
}   

