#include "juego.h"
#include "lista.h"
#include "tipo.h"
#include "pokemon.h"
#include "ataque.h"
#include "adversario.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_POKEMONES 50
#define TAMANIO_MAXIMO 20
typedef struct {
    char nombres[MAX_POKEMONES][TAMANIO_MAXIMO];
    int cantidad;
} nombres_pokemon_t;
typedef struct jugador_1{
	int puntaje;
	pokemon_t *pokemon1;
	pokemon_t *pokemon2;
	pokemon_t *pokemon3;
} jugador_t;
struct juego{
	int cant_jugadas;
	informacion_pokemon_t *info_pokemones;
	jugador_t jugador;
	adversario_t *adversario;
	// lista_t *pokemon;
	// JUEGO_ESTADO estado;
};

juego_t *juego_crear(void)
{
	juego_t *juego = calloc(1, sizeof(juego_t));
	if (juego == NULL) {
		return NULL;
	}
	return juego;
}

JUEGO_ESTADO juego_cargar_pokemon(juego_t *juego, char *archivo)
{
	if(juego == NULL || archivo == NULL){
		return ERROR_GENERAL;
	}

	informacion_pokemon_t *info = pokemon_cargar_archivo(archivo);
	if(info == NULL){
		return ERROR_GENERAL;
	}

	int cantidad = pokemon_cantidad(info);
	if(cantidad <= 3){
		return POKEMON_INSUFICIENTES;
	}

	juego->info_pokemones = info;
	return TODO_OK;
}

void obtener_nombre_pokemon(pokemon_t *pokemon, void *parametro) {
    nombres_pokemon_t *data = (nombres_pokemon_t *)parametro;

    const char *nombre = pokemon_nombre(pokemon);
    strcpy(data->nombres[data->cantidad], nombre);
    data->cantidad++;
}

lista_t *juego_listar_pokemon(juego_t *juego)
{
	if(juego == NULL){
		return NULL;
	}

	lista_t *lista = lista_crear();
	if(lista == NULL){
		return NULL;
	}

	nombres_pokemon_t *nombres_pokemones = calloc(1, sizeof(nombres_pokemon_t));

	int iterados = con_cada_pokemon(juego->info_pokemones, obtener_nombre_pokemon, nombres_pokemones);
	if(iterados == 0){
		return NULL;
	}
	for(int i = 0; i < iterados; i++){
		pokemon_t *pokemon = pokemon_buscar(juego->info_pokemones, nombres_pokemones->nombres[i]);
		if (pokemon != NULL) {
            lista_insertar(lista, pokemon);
        }
    }

	free(nombres_pokemones);
    return lista;
}

JUEGO_ESTADO juego_seleccionar_pokemon(juego_t *juego, JUGADOR jugador,
				       const char *nombre1, const char *nombre2,
				       const char *nombre3)
{
	if(juego == NULL || nombre1 == NULL || nombre2 == NULL || nombre3 == NULL){
		return ERROR_GENERAL;
	}

	pokemon_t *pokemon1 = pokemon_buscar(juego->info_pokemones, nombre1);
	if(pokemon1 == NULL){
		return POKEMON_INEXISTENTE;
	}

	pokemon_t *pokemon2 = pokemon_buscar(juego->info_pokemones, nombre2);
	if(pokemon2 == NULL){
		return POKEMON_INEXISTENTE;
	}

	pokemon_t *pokemon3 = pokemon_buscar(juego->info_pokemones, nombre3);
	if(pokemon3 == NULL){
		return POKEMON_INEXISTENTE;
	}

	if(jugador == JUGADOR1){
		juego->jugador.pokemon1 = pokemon1;
        juego->jugador.pokemon2 = pokemon2;
        juego->jugador.pokemon3 = pokemon3;
	}
	return TODO_OK;
}

RESULTADO_ATAQUE efectividad_ataque(const struct ataque *ataque, pokemon_t *pokemon){
	enum TIPO tipo_pokemon = pokemon_tipo(pokemon);
	switch (ataque->tipo){
		case NORMAL:
			return ATAQUE_REGULAR;
			break;
		case FUEGO:
			if(tipo_pokemon == PLANTA){
				return ATAQUE_EFECTIVO;
			}
			if(tipo_pokemon == AGUA){
				return ATAQUE_INEFECTIVO;
			}
			break;
		case PLANTA:
			if(tipo_pokemon == ROCA){
				return ATAQUE_EFECTIVO;
			}
			if(tipo_pokemon == FUEGO){
				return ATAQUE_INEFECTIVO;
			}
			break;
		case ROCA:
			if(tipo_pokemon == ELECTRICO){
				return ATAQUE_EFECTIVO;
			}
			if(tipo_pokemon == PLANTA){
				return ATAQUE_INEFECTIVO;
			}
			break;
		case ELECTRICO:
			if(tipo_pokemon == AGUA){
				return ATAQUE_EFECTIVO;
			}
			if(tipo_pokemon == ROCA){
				return ATAQUE_INEFECTIVO;
			}
			break;
		case AGUA:
			if(tipo_pokemon == FUEGO){
				return ATAQUE_EFECTIVO;
			}
			if(tipo_pokemon == ELECTRICO){
				return ATAQUE_INEFECTIVO;
			}
			break;
	}
	return ATAQUE_ERROR;
}

resultado_jugada_t juego_jugar_turno(juego_t *juego, jugada_t jugada_jugador1,
				     jugada_t jugada_jugador2)
{
	resultado_jugada_t resultado;
	if(juego == NULL || jugada_jugador1.pokemon[0] == '\0' || jugada_jugador1.ataque[0] == '\0'|| jugada_jugador2.pokemon[0] == '\0'|| jugada_jugador2.ataque[0] == '\0'){
		resultado.jugador1 = ATAQUE_ERROR;
		resultado.jugador2 = ATAQUE_ERROR;
		return resultado;
	}

	pokemon_t *pokemon1 = pokemon_buscar(juego->info_pokemones, jugada_jugador1.pokemon);
	const struct ataque *ataque1 = pokemon_buscar_ataque(pokemon1, jugada_jugador1.ataque);
	
	pokemon_t *pokemon2 = pokemon_buscar(juego->info_pokemones, jugada_jugador2.pokemon);
	const struct ataque *ataque2 = pokemon_buscar_ataque(pokemon2, jugada_jugador2.ataque);

	if(pokemon1 == NULL || pokemon2 == NULL || ataque1 == NULL || ataque2 == NULL){
		resultado.jugador1 = ATAQUE_ERROR;
		resultado.jugador2 = ATAQUE_ERROR;			
		return resultado;
	}

	//validar que un ataque pueda ser usado una sola vez
	//
	//

	RESULTADO_ATAQUE resulta_ataque_jugada1 = efectividad_ataque(ataque1, pokemon2);
	RESULTADO_ATAQUE resulta_ataque_jugada2 = efectividad_ataque(ataque2, pokemon1);

	switch (resulta_ataque_jugada1){
		case ATAQUE_EFECTIVO:
			juego->jugador.puntaje += ataque1->poder * 3;
			break;
		case ATAQUE_INEFECTIVO:
			juego->jugador.puntaje += ataque1->poder / 2;
			break;
		default:
			juego->jugador.puntaje += ataque1->poder;
			break;
	}

	//puntos del adversario
	// switch (resulta_ataque_jugada2){
	// 	case ATAQUE_EFECTIVO:
	// 		ataque2->poder * 3;
	// 		break;
	// 	case ATAQUE_INEFECTIVO:
	// 		ataque2->poder / 2;
	// 		break;
	// 	default:
	// 		ataque2->poder;
	// 		break;
	// }

	resultado.jugador1 = resulta_ataque_jugada1;
	resultado.jugador2 = resulta_ataque_jugada2;
	return resultado;
}

int juego_obtener_puntaje(juego_t *juego, JUGADOR jugador)
{
	if(juego == NULL){
		return 0;
	}

	if(jugador == JUGADOR1){
		return juego->jugador.puntaje;
	}

	if(jugador == JUGADOR2){
		//adversario puntaje
	}
	return 0;
}

bool juego_finalizado(juego_t *juego)
{
	if(juego == NULL){
		return false;
	}
	return true;
}

void juego_destruir(juego_t *juego)
{
	if(juego == NULL){
		return;
	}

	pokemon_destruir_todo(juego->info_pokemones);
	free(juego);
}
