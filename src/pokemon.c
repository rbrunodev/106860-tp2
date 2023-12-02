#include <stdlib.h>
#include "pokemon.h"
#include "tipo.h"
#include <stdio.h>
#include "ataque.h"
#include <string.h>

struct pokemon {
	char nombre[20];
	enum TIPO tipo;
	struct ataque ataques[4];
};

struct info_pokemon {
	pokemon_t *pokemones;
	size_t cantidad;
};

informacion_pokemon_t *pokemon_cargar_archivo(const char *path)
{
	if (path == NULL) {
		return NULL;
	}
	informacion_pokemon_t *info = calloc(1, sizeof(informacion_pokemon_t)); // calloc inicializa en 0
	if (!info) {
		return NULL;
	}

	FILE *archivo = fopen(path, "r");

	if (!archivo) {
		free(info);
		return NULL;
	}

	pokemon_t *pokemon = malloc(sizeof(pokemon_t));
	while (!feof(archivo)) {
		char pokemon_type[20];
		int linea_pokemon = fscanf(archivo, "%[^;];%[^\n]\n", pokemon->nombre,
				    pokemon_type);
		if (linea_pokemon != 2) {
			fclose(archivo);
			free(pokemon);
			free(info);
			continue;
		}
		//optimizar
		if (strcmp(pokemon_type, "F") == 0) {
			pokemon->tipo = FUEGO;
		} else if (strcmp(pokemon_type, "A") == 0) {
			pokemon->tipo = AGUA;
		} else if (strcmp(pokemon_type, "N") == 0) {
			pokemon->tipo = NORMAL;
		} else if (strcmp(pokemon_type, "P") == 0) {
			pokemon->tipo = PLANTA;
		} else if (strcmp(pokemon_type, "E") == 0) {
			pokemon->tipo = ELECTRICO;
		} else if (strcmp(pokemon_type, "R") == 0) {
			pokemon->tipo = ROCA;
		} else {
			printf("Tipo de Pokémon inválido.\n");
			fclose(archivo);
			free(pokemon);
			free(info);
			return NULL;
		}
		for (int i = 0; i < 3; i++) {
			char ataque_type[20];
			int ataques_leidos = fscanf(archivo, "%[^;];%[^;];%u\n",
						    pokemon->ataques[i].nombre,
						    ataque_type,
						    &pokemon->ataques[i].poder);
			if (ataques_leidos != 3) {
				printf("Error al leer los ataques del Pokémon.\n");
				fclose(archivo);
				free(pokemon);
				free(info);
				return NULL;
			}
			//optimizar
			if (strcmp(ataque_type, "F") == 0) {
				pokemon->ataques[i].tipo = FUEGO;
			} else if (strcmp(ataque_type, "A") == 0) {
				pokemon->ataques[i].tipo = AGUA;
			} else if (strcmp(ataque_type, "N") == 0) {
				pokemon->ataques[i].tipo = NORMAL;
			} else if (strcmp(ataque_type, "P") == 0) {
				pokemon->ataques[i].tipo = PLANTA;
			} else if (strcmp(ataque_type, "E") == 0) {
				pokemon->ataques[i].tipo = ELECTRICO;
			} else if (strcmp(ataque_type, "R") == 0) {
				pokemon->ataques[i].tipo = ROCA;
			} else {
				printf("Tipo de Ataque inválido.\n");
				free(pokemon);
				fclose(archivo);
				return info;
			}
		}
		info->cantidad++;
		info->pokemones = realloc(info->pokemones,
					  sizeof(pokemon_t) * info->cantidad);
		if (!info->pokemones) {
			printf("Error al reasignar memoria para los Pokémon.\n");
			free(info);
			free(pokemon);
			fclose(archivo);
			return NULL;
		}
		info->pokemones[info->cantidad - 1] = *pokemon;
	}

	free(pokemon);
	// ordenar_pokemones(info);
	fclose(archivo);
	return info;
}

pokemon_t *pokemon_buscar(informacion_pokemon_t *ip, const char *nombre)
{
	if (!ip || !nombre)
		return NULL;

	for (int i = 0; i < ip->cantidad; i++) {
		if (strcmp(ip->pokemones[i].nombre, nombre) == 0) {
			return &ip->pokemones[i];
		}
	}

	return NULL;
}

int pokemon_cantidad(informacion_pokemon_t *ip)
{
	return ip ? (int)ip->cantidad : 0;
}

const char *pokemon_nombre(pokemon_t *pokemon)
{
	if (pokemon)
		return pokemon->nombre;

	return NULL;
}

enum TIPO pokemon_tipo(pokemon_t *pokemon)
{
	return FUEGO;
}

const struct ataque *pokemon_buscar_ataque(pokemon_t *pokemon,
					   const char *nombre)
{
	for (int i = 0; i < 3; i++) {
		if (strcmp(pokemon->ataques[i].nombre, nombre) == 0) {
			printf("Ataque encontrado: %s\n",
			       pokemon->ataques[i].nombre);
			return &pokemon->ataques[i];
		}
	}

	return NULL;
}


int con_cada_pokemon(informacion_pokemon_t *ip, void (*f)(pokemon_t *, void *),
		     void *aux)
{
	int cantidad_itinerados = 0;

	if (!ip || !f)
		return cantidad_itinerados;

	for (int i = 0; i < ip->cantidad; i++) {
		f(&ip->pokemones[i], aux);
		cantidad_itinerados++;
	}

	return cantidad_itinerados;
}

int con_cada_ataque(pokemon_t *pokemon,
		    void (*f)(const struct ataque *, void *), void *aux)
{
	return 0;
}

void pokemon_destruir_todo(informacion_pokemon_t *ip)
{
}
