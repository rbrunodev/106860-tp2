#include "lista.h"
#include "juego.h"
#include "adversario.h"
#include "pokemon.h"
#include "ataque.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

struct adversario {
	int puntaje;
	pokemon_t *pokemon1;
	pokemon_t *pokemon2;
	pokemon_t *pokemon3;
	lista_t *lista_pokemones_cargados;
	lista_t *pokemones_del_jugador;
	lista_t *pokemones_jugados;
	struct ataque *vector_ataques[9];
};

adversario_t *adversario_crear(lista_t *pokemon)
{
	adversario_t *adversario = calloc(1, sizeof(adversario_t));
	if (adversario == NULL) {
		return NULL;
	}
	adversario->lista_pokemones_cargados = pokemon;
	return adversario;
}

bool adversario_seleccionar_pokemon(adversario_t *adversario, char **nombre1,
				    char **nombre2, char **nombre3)
{
	if (adversario == NULL) {
		return false;
	}

	srand((unsigned int)time(NULL));

	size_t lista_tamano =
		lista_tamanio(adversario->lista_pokemones_cargados);
	if (lista_tamano < 3) {
		return false;
	}

	size_t pos1, pos2, pos3;
	pos1 = (size_t)rand() % lista_tamano;
	do {
		pos2 = (size_t)rand() % lista_tamano;
	} while (pos2 == pos1);

	do {
		pos3 = (size_t)rand() % lista_tamano;
	} while (pos3 == pos1 || pos3 == pos2);

	adversario->pokemon1 = lista_elemento_en_posicion(
		adversario->lista_pokemones_cargados, pos1);
	adversario->pokemon2 = lista_elemento_en_posicion(
		adversario->lista_pokemones_cargados, pos2);
	pokemon_t *pokemon3 = lista_elemento_en_posicion(
		adversario->lista_pokemones_cargados, pos3);

	if (adversario->pokemon1 == NULL || adversario->pokemon2 == NULL) {
		return false;
	}

	const char *tmp_nombre1 = pokemon_nombre(adversario->pokemon1);
	const char *tmp_nombre2 = pokemon_nombre(adversario->pokemon2);
	const char *tmp_nombre3 = pokemon_nombre(pokemon3);

	*nombre1 = malloc(sizeof(char) * (strlen(tmp_nombre1) + 1));
	*nombre2 = malloc(sizeof(char) * (strlen(tmp_nombre2) + 1));
	*nombre3 = malloc(sizeof(char) * (strlen(tmp_nombre3) + 1));

	if (!*nombre1 || !*nombre2 || !*nombre3) {
		return false;
	}

	strcpy(*nombre1, tmp_nombre1);
	strcpy(*nombre2, tmp_nombre2);
	strcpy(*nombre3, tmp_nombre3);

	//pokemon es para el otro jugador
	// pokemon_t *pokemon3 = lista_elemento_en_posicion(adversario->lista_pokemones_cargados, pos3);
	return true;
}

bool adversario_pokemon_seleccionado(adversario_t *adversario, char *nombre1,
				     char *nombre2, char *nombre3)
{
	if (adversario == NULL) {
		return false;
	}

	// adversario->pokemon3 = pokemon_buscar(adversario->lista_pokemones_cargados, nombre3);
	return adversario->pokemon3 != NULL;
}

jugada_t adversario_proxima_jugada(adversario_t *adversario)
{
	jugada_t j = { .ataque = "", .pokemon = "" };
	if (adversario == NULL) {
		return j;
	}

	//el adversario tiene 3 pokemones cargados y debe elegir uno para la jugada
	return j;
}

void adversario_informar_jugada(adversario_t *a, jugada_t j)
{
}

void adversario_destruir(adversario_t *adversario)
{
}
