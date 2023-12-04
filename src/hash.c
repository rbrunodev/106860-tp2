#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hash.h"

#define FACTOR_CARGA_MAXIMO 0.7
typedef struct nodo {
	char *clave;
	void *valor;
	struct nodo *siguiente;
} nodo_t;

struct hash {
	size_t capacidad;
	size_t cantidad;
	nodo_t **tabla;
};

hash_t *hash_crear(size_t capacidad)
{
	if (capacidad < 3)
		capacidad = 3;

	hash_t *hash = malloc(sizeof(hash_t));
	if (!hash)
		return NULL;

	hash->tabla = calloc(capacidad, sizeof(nodo_t *));
	if (!hash->tabla) {
		free(hash);
		return NULL;
	}

	hash->capacidad = capacidad;
	hash->cantidad = 0;
	return hash;
}

size_t hash_func(const char *clave, size_t capacidad)
{
	size_t hash = 0;
	while (*clave) {
		hash += (size_t)(*clave);
		clave++;
	}
	return hash % capacidad;
}

void hash_redimensionar(hash_t *hash, size_t nueva_capacidad)
{
	if (!hash)
		return;

	nodo_t **tabla_nueva = calloc(nueva_capacidad, sizeof(nodo_t *));
	if (!tabla_nueva)
		return;

	size_t nueva_cantidad = hash->cantidad;

	for (size_t i = 0; i < hash->capacidad; i++) {
		nodo_t *nodo = hash->tabla[i];
		while (nodo) {
			nodo_t *nodo_a_mover = nodo;
			nodo = nodo->siguiente;

			size_t indice =
				hash_func(nodo_a_mover->clave, nueva_capacidad);
			nodo_a_mover->siguiente = tabla_nueva[indice];
			tabla_nueva[indice] = nodo_a_mover;
		}
	}

	free(hash->tabla);
	hash->tabla = tabla_nueva;
	hash->capacidad = nueva_capacidad;
	hash->cantidad = nueva_cantidad;
}

bool agregar_capacidad(hash_t *hash)
{
	if (!hash)
		return false;

	size_t factor_carga = hash->cantidad / hash->capacidad;

	return factor_carga >= FACTOR_CARGA_MAXIMO;
}

nodo_t *crear_nodo(const char *clave, void *elemento)
{
	nodo_t *nodo = malloc(sizeof(nodo_t));
	if (!nodo)
		return NULL;

	char *cadena = (char *)calloc(strlen(clave) + 1, sizeof(char));
	if (cadena) {
		strcpy(cadena, clave);
	} else {
		free(nodo);
		return NULL;
	}
	nodo->clave = cadena;
	nodo->valor = elemento;
	nodo->siguiente = NULL;
	return nodo;
}

hash_t *hash_insertar(hash_t *hash, const char *clave, void *elemento,
		      void **anterior)
{
	if (!hash || !clave)
		return NULL;

	if (agregar_capacidad(hash)) {
		hash_redimensionar(hash, hash->capacidad * 2);
	}

	size_t indice = hash_func(clave, hash->capacidad);
	nodo_t *nodo = hash->tabla[indice];
	nodo_t *nodo_previo = NULL;

	while (nodo) {
		if (strcmp(nodo->clave, clave) == 0) {
			if (anterior) {
				*anterior = nodo->valor;
			}
			nodo->valor = elemento;
			return hash;
		}
		nodo_previo = nodo;
		nodo = nodo->siguiente;
	}

	nodo_t *nuevo_nodo = crear_nodo(clave, elemento);
	if (!nuevo_nodo)
		return NULL;

	if (anterior)
		*anterior = NULL;

	if (nodo_previo) {
		nodo_previo->siguiente = nuevo_nodo;
	} else {
		hash->tabla[indice] = nuevo_nodo;
	}

	hash->cantidad++;
	return hash;
}

void *hash_quitar(hash_t *hash, const char *clave)
{
	if (!hash || !clave)
		return NULL;

	size_t indice = hash_func(clave, hash->capacidad);
	nodo_t *nodo_actual = hash->tabla[indice];
	nodo_t *nodo_anterior = NULL;

	while (nodo_actual) {
		if (strcmp(nodo_actual->clave, clave) == 0) {
			if (nodo_anterior) {
				nodo_anterior->siguiente =
					nodo_actual->siguiente;
			} else {
				hash->tabla[indice] = nodo_actual->siguiente;
			}

			void *valor = nodo_actual->valor;
			free(nodo_actual->clave);
			free(nodo_actual);
			hash->cantidad--;
			return valor;
		}

		nodo_anterior = nodo_actual;
		nodo_actual = nodo_actual->siguiente;
	}

	return NULL;
}

void *hash_obtener(hash_t *hash, const char *clave)
{
	if (!hash || !clave)
		return NULL;

	size_t indice = hash_func(clave, hash->capacidad);
	nodo_t *nodo = hash->tabla[indice];

	while (nodo) {
		if (strcmp(nodo->clave, clave) == 0) {
			return nodo->valor;
		}
		nodo = nodo->siguiente;
	}

	return NULL;
}

bool hash_contiene(hash_t *hash, const char *clave)
{
	if (!hash || !clave)
		return false;

	size_t indice = hash_func(clave, hash->capacidad);
	nodo_t *nodo = hash->tabla[indice];

	while (nodo) {
		if (strcmp(nodo->clave, clave) == 0) {
			return true;
		}
		nodo = nodo->siguiente;
	}

	return false;
}

size_t hash_cantidad(hash_t *hash)
{
	if (!hash)
		return 0;
	return hash->cantidad;
}

void hash_destruir(hash_t *hash)
{
	hash_destruir_todo(hash, NULL);
}

void hash_destruir_todo(hash_t *hash, void (*destructor)(void *))
{
	if (!hash)
		return;

	if (hash_cantidad(hash) == 0) {
		free(hash->tabla);
		free(hash);
		return;
	}

	for (size_t i = 0; i < hash->capacidad; i++) {
		nodo_t *nodo = hash->tabla[i];
		while (nodo) {
			nodo_t *nodo_a_eliminar = nodo;
			nodo = nodo->siguiente;
			if (destructor)
				destructor(nodo_a_eliminar->valor);
			free(nodo_a_eliminar->clave);
			free(nodo_a_eliminar);
		}
	}

	free(hash->tabla);
	free(hash);
}

size_t hash_con_cada_clave(hash_t *hash,
			   bool (*f)(const char *clave, void *valor, void *aux),
			   void *aux)
{
	size_t n = 0;
	if (!hash || !f)
		return n;

	for (size_t i = 0; i < hash->capacidad; ++i) {
		nodo_t *nodo_actual = hash->tabla[i];
		while (nodo_actual) {
			n++;
			if (!f(nodo_actual->clave, nodo_actual->valor, aux)) {
				return n;
			}
			nodo_actual = nodo_actual->siguiente;
		}
	}
	return n;
}

