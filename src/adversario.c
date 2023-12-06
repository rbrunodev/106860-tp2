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
	bool jugada_usada[9];
	jugada_t jugadas_posibles[9];
	int num_ataques;
	pokemon_t *pokemon1;
	pokemon_t *pokemon2;
	pokemon_t *pokemon3;
	lista_t *lista_pokemones_cargados;
	const struct ataque *posibles_ataques[3];
};

int comparar_pokemones(void *p1, void *nombre)
{
	pokemon_t *pokemon = p1;
	return strcmp(pokemon_nombre(pokemon), (char *)nombre);
}

adversario_t *adversario_crear(lista_t *pokemon)
{
	adversario_t *adversario = calloc(1, sizeof(adversario_t));
	if (!adversario) {
		return NULL;
	}
	adversario->lista_pokemones_cargados = pokemon;
	for (int i = 0; i < 9; i++) {
		adversario->jugada_usada[i] = false;
		strcpy(adversario->jugadas_posibles[i].pokemon, "\0");
		strcpy(adversario->jugadas_posibles[i].ataque, "\0");
	}
	return adversario;
}

void asignar_ataque(const struct ataque *ataque, void *aux)
{
	adversario_t *data = aux;
	if (data->num_ataques == 3) {
		data->posibles_ataques[0] = NULL;
		data->posibles_ataques[1] = NULL;
		data->posibles_ataques[2] = NULL;
		data->num_ataques = 0;
	}
	data->posibles_ataques[data->num_ataques] = ataque;
	data->num_ataques++;
}

bool adversario_seleccionar_pokemon(adversario_t *adversario, char **nombre1,
				    char **nombre2, char **nombre3)
{
	if (!adversario) {
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

	pokemon_t *pokemon1 = lista_elemento_en_posicion(
		adversario->lista_pokemones_cargados, pos1);

	adversario->pokemon1 = pokemon1;

	pokemon_t *pokemon2 = lista_elemento_en_posicion(
		adversario->lista_pokemones_cargados, pos2);
	adversario->pokemon2 = pokemon2;

	pokemon_t *pokemon3 = lista_elemento_en_posicion(
		adversario->lista_pokemones_cargados, pos3);

	if (!adversario->pokemon1 || !adversario->pokemon2) {
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

	return true;
}

int comparar_pokemon_nombre(void *elemento, void *contexto)
{
	pokemon_t *pokemon = (pokemon_t *)elemento;
	char *nombre_buscado = (char *)contexto;

	return strcmp(pokemon_nombre(pokemon), nombre_buscado);
}

bool adversario_pokemon_seleccionado(adversario_t *adversario, char *nombre1,
				     char *nombre2, char *nombre3)
{
	if (!adversario) {
		return false;
	}

	pokemon_t *pokemon3 =
		lista_buscar_elemento(adversario->lista_pokemones_cargados,
				      comparar_pokemon_nombre, nombre3);
	if (!pokemon3) {
		return false;
	}

	adversario->pokemon3 = pokemon3;
	return adversario->pokemon3 != NULL;
}

jugada_t adversario_proxima_jugada(adversario_t *adversario)
{
	jugada_t j = { .ataque = "", .pokemon = "" };
	if (!adversario) {
		return j;
	}

	if (!adversario->pokemon1 || !adversario->pokemon2 ||
	    !adversario->pokemon3) {
		return j;
	}

	if (strcmp(adversario->jugadas_posibles[0].pokemon, "\0") == 0) {
		int n = 0;
		for (size_t i = 0; i < 3; i++) {
			pokemon_t *pokemon;
			if (i == 0) {
				pokemon = adversario->pokemon1;
			}
			if (i == 1) {
				pokemon = adversario->pokemon2;
			}
			if (i == 2) {
				pokemon = adversario->pokemon3;
			}
			for (int j = 0; j < 3; j++) {
				strcpy(adversario->jugadas_posibles[n].pokemon,
				       pokemon_nombre(pokemon));
				con_cada_ataque(pokemon, asignar_ataque,
						adversario);
				strcpy(adversario->jugadas_posibles[n].ataque,
				       adversario->posibles_ataques[j]->nombre);
				n++;
			}
		}
	}

	int random = rand() % 9;

	while (adversario->jugada_usada[random]) {
		random = rand() % 9;
	}

	adversario->jugada_usada[random] = true;

	return adversario->jugadas_posibles[random];
	return j;
}

void adversario_informar_jugada(adversario_t *a, jugada_t j)
{
}

void adversario_destruir(adversario_t *adversario)
{
	if (!adversario) {
		return;
	}

	if (adversario->lista_pokemones_cargados) {
		lista_destruir(adversario->lista_pokemones_cargados);
	}

	free(adversario);
}
