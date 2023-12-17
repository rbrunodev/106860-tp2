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

typedef struct jugador_1 {
	int puntaje;
	int num_ataques_usados;
	pokemon_t *pokemon1;
	pokemon_t *pokemon2;
	pokemon_t *pokemon3;
	const struct ataque *ataques_usados[9];
} jugador_t;
struct juego {
	bool finalizado;
	int cant_jugadas;
	int puntaje_adversario;
	jugador_t jugador;
	pokemon_t *pokemon_adversario1;
	pokemon_t *pokemon_adversario2;
	pokemon_t *pokemon_adversario3;
	lista_t *lista_pokemones;
	informacion_pokemon_t *info_pokemones;
};

juego_t *juego_crear(void)
{
	juego_t *juego = calloc(1, sizeof(juego_t));
	if (!juego) {
		return NULL;
	}
	return juego;
}

JUEGO_ESTADO juego_cargar_pokemon(juego_t *juego, char *archivo)
{
	if (!juego || !archivo) {
		return ERROR_GENERAL;
	}

	informacion_pokemon_t *info = pokemon_cargar_archivo(archivo);
	if (!info) {
		return ERROR_GENERAL;
	}

	int cantidad = pokemon_cantidad(info);
	if (cantidad <= 3) {
		pokemon_destruir_todo(info);
		return POKEMON_INSUFICIENTES;
	}

	juego->info_pokemones = info;
	return TODO_OK;
}

void obtener_nombre_pokemon(pokemon_t *pokemon, void *parametro)
{
	nombres_pokemon_t *data = (nombres_pokemon_t *)parametro;

	const char *nombre = pokemon_nombre(pokemon);
	strcpy(data->nombres[data->cantidad], nombre);
	data->cantidad++;
}

lista_t *juego_listar_pokemon(juego_t *juego)
{
	if (!juego) {
		return NULL;
	}

	lista_t *lista = lista_crear();
	if (!lista) {
		return NULL;
	}

	nombres_pokemon_t *nombres_pokemones =
		calloc(1, sizeof(nombres_pokemon_t));

	int iterados = con_cada_pokemon(juego->info_pokemones,
					obtener_nombre_pokemon,
					nombres_pokemones);
	if (iterados == 0) {
		free(lista);
		free(nombres_pokemones);
		return NULL;
	}

	for (int i = 0; i < iterados; i++) {
		pokemon_t *pokemon = pokemon_buscar(
			juego->info_pokemones, nombres_pokemones->nombres[i]);
		if (pokemon != NULL) {
			juego->lista_pokemones = lista_insertar(lista, pokemon);
		}
	}

	free(nombres_pokemones);
	return juego->lista_pokemones;
}

JUEGO_ESTADO juego_seleccionar_pokemon(juego_t *juego, JUGADOR jugador,
				       const char *nombre1, const char *nombre2,
				       const char *nombre3)
{
	if (!juego) {
		return ERROR_GENERAL;
	}

	if (strcmp(nombre1, nombre2) == 0 || strcmp(nombre1, nombre3) == 0 ||
	    strcmp(nombre2, nombre3) == 0) {
		return POKEMON_REPETIDO;
	}

	pokemon_t *pokemon1 = pokemon_buscar(juego->info_pokemones, nombre1);
	pokemon_t *pokemon2 = pokemon_buscar(juego->info_pokemones, nombre2);
	pokemon_t *pokemon3 = pokemon_buscar(juego->info_pokemones, nombre3);
	if (!pokemon1 || !pokemon2 || !pokemon3) {
		return POKEMON_INEXISTENTE;
	}

	if (jugador == JUGADOR1) {
		juego->jugador.pokemon1 = pokemon1;
		juego->jugador.pokemon2 = pokemon2;
		juego->jugador.pokemon3 = pokemon3;
		return TODO_OK;
	}

	if (jugador == JUGADOR2) {
		juego->pokemon_adversario1 = pokemon1;
		juego->pokemon_adversario2 = pokemon2;
		juego->pokemon_adversario3 = pokemon3;
		return TODO_OK;
	}

	return ERROR_GENERAL;
}

RESULTADO_ATAQUE efectividad_ataque(const struct ataque *ataque,
				    pokemon_t *pokemon)
{
	enum TIPO tipo_pokemon = pokemon_tipo(pokemon);
	switch (ataque->tipo) {
	case NORMAL:
		return ATAQUE_REGULAR;
	case FUEGO:
		if (tipo_pokemon == PLANTA) {
			return ATAQUE_EFECTIVO;
		}
		if (tipo_pokemon == AGUA) {
			return ATAQUE_INEFECTIVO;
		}
		return ATAQUE_REGULAR;
	case PLANTA:
		if (tipo_pokemon == ROCA) {
			return ATAQUE_EFECTIVO;
		}
		if (tipo_pokemon == FUEGO) {
			return ATAQUE_INEFECTIVO;
		}
		return ATAQUE_REGULAR;
	case ROCA:
		if (tipo_pokemon == ELECTRICO) {
			return ATAQUE_EFECTIVO;
		}
		if (tipo_pokemon == PLANTA) {
			return ATAQUE_INEFECTIVO;
		}
		return ATAQUE_REGULAR;
	case ELECTRICO:
		if (tipo_pokemon == AGUA) {
			return ATAQUE_EFECTIVO;
		}
		if (tipo_pokemon == ROCA) {
			return ATAQUE_INEFECTIVO;
		}
		return ATAQUE_REGULAR;
	case AGUA:
		if (tipo_pokemon == FUEGO) {
			return ATAQUE_EFECTIVO;
		}
		if (tipo_pokemon == ELECTRICO) {
			return ATAQUE_INEFECTIVO;
		}
		return ATAQUE_REGULAR;
	}
	return ATAQUE_ERROR;
}

bool ataque_utilizado(jugador_t *jugador, const struct ataque *ataque)
{
	if (!jugador || !ataque) {
		return false;
	}

	for (int i = 0; i < jugador->num_ataques_usados; ++i) {
		if (strcmp(jugador->ataques_usados[i]->nombre,
			   ataque->nombre) == 0) {
			return true;
		}
	}

	jugador->ataques_usados[jugador->num_ataques_usados] = ataque;
	jugador->num_ataques_usados++;
	return false;
}

resultado_jugada_t juego_jugar_turno(juego_t *juego, jugada_t jugada_jugador1,
				     jugada_t jugada_jugador2)
{
	resultado_jugada_t resultado;
	if (!juego || jugada_jugador1.pokemon[0] == '\0' ||
	    jugada_jugador1.ataque[0] == '\0' ||
	    jugada_jugador2.pokemon[0] == '\0' ||
	    jugada_jugador2.ataque[0] == '\0') {
		resultado.jugador1 = ATAQUE_ERROR;
		resultado.jugador2 = ATAQUE_ERROR;
		return resultado;
	}

	pokemon_t *pokemon_jugada_1 =
		pokemon_buscar(juego->info_pokemones, jugada_jugador1.pokemon);
	pokemon_t *pokemon_jugada_2 =
		pokemon_buscar(juego->info_pokemones, jugada_jugador2.pokemon);
	if (!pokemon_jugada_1 || !pokemon_jugada_2) {
		resultado.jugador1 = ATAQUE_ERROR;
		resultado.jugador2 = ATAQUE_ERROR;
		return resultado;
	}

	const struct ataque *ataque1 =
		pokemon_buscar_ataque(pokemon_jugada_1, jugada_jugador1.ataque);
	const struct ataque *ataque2 =
		pokemon_buscar_ataque(pokemon_jugada_2, jugada_jugador2.ataque);

	if (!ataque1 || !ataque2) {
		resultado.jugador1 = ATAQUE_ERROR;
		resultado.jugador2 = ATAQUE_ERROR;
		return resultado;
	}

	if (ataque_utilizado(&juego->jugador, ataque1)) {
		resultado.jugador1 = ATAQUE_ERROR;
		resultado.jugador2 = ATAQUE_ERROR;
		return resultado;
	}

	RESULTADO_ATAQUE resulta_ataque_jugada1 =
		efectividad_ataque(ataque1, pokemon_jugada_2);
	RESULTADO_ATAQUE resulta_ataque_jugada2 =
		efectividad_ataque(ataque2, pokemon_jugada_1);

	switch (resulta_ataque_jugada1) {
	case ATAQUE_EFECTIVO:
		juego->jugador.puntaje += (int)(ataque1->poder * 3);
		break;
	case ATAQUE_INEFECTIVO:
		juego->jugador.puntaje += (int)((ataque1->poder + 1) / 2);
		break;
	case ATAQUE_REGULAR:
		juego->jugador.puntaje += (int)(ataque1->poder);
		break;
	default:
		resultado.jugador1 = ATAQUE_ERROR;
		resultado.jugador2 = ATAQUE_ERROR;
		return resultado;
	}

	switch (resulta_ataque_jugada2) {
	case ATAQUE_EFECTIVO:
		juego->puntaje_adversario += (int)(ataque2->poder * 3);
		break;
	case ATAQUE_INEFECTIVO:
		juego->puntaje_adversario += (int)((ataque2->poder + 1) / 2);
		break;
	case ATAQUE_REGULAR:
		juego->puntaje_adversario += (int)(ataque2->poder);
		break;
	default:
		resultado.jugador1 = ATAQUE_ERROR;
		resultado.jugador2 = ATAQUE_ERROR;
		return resultado;
	}

	juego->cant_jugadas++;
	if (juego->cant_jugadas == 9) {
		juego->finalizado = true;
	}

	resultado.jugador1 = resulta_ataque_jugada1;
	resultado.jugador2 = resulta_ataque_jugada2;
	return resultado;
}

int juego_obtener_puntaje(juego_t *juego, JUGADOR jugador)
{
	if (!juego) {
		return 0;
	}

	if (jugador == JUGADOR1) {
		return juego->jugador.puntaje;
	}

	if (jugador == JUGADOR2) {
		return juego->puntaje_adversario;
	}

	return 0;
}

bool juego_finalizado(juego_t *juego)
{
	if (!juego) {
		return false;
	}

	return juego->finalizado || juego->cant_jugadas == 9;
}

void juego_destruir(juego_t *juego)
{
	if (!juego) {
		return;
	}

	pokemon_destruir_todo(juego->info_pokemones);
	lista_destruir(juego->lista_pokemones);
	free(juego);
}
