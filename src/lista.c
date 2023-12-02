#include "lista.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct nodo {
	void *elemento;
	struct nodo *siguiente;
} nodo_t;

typedef struct lista {
	nodo_t *nodo_inicio;
	nodo_t *nodo_fin;
} lista;

struct lista_iterador {
	nodo_t *nodo_actual;
	lista_t *lista;
};

lista_t *lista_crear(void)
{
	lista_t *lista = malloc(sizeof(lista_t));

	if (!lista)
		return NULL;

	lista->nodo_inicio = NULL;
	lista->nodo_fin = NULL;
	return lista;
}

nodo_t *nodo_crear(void *elemento)
{
	nodo_t *nodo = malloc(sizeof(nodo_t));
	if (!nodo)
		return NULL;

	nodo->elemento = elemento;
	nodo->siguiente = NULL;

	return nodo;
}

lista_t *lista_insertar(lista_t *lista, void *elemento)
{
	if (!lista)
		return NULL;

	nodo_t *nuevo_nodo = nodo_crear(elemento);

	if (!nuevo_nodo)
		return NULL;

	if (!lista->nodo_inicio) {
		lista->nodo_inicio = nuevo_nodo;
		lista->nodo_fin = nuevo_nodo;
		return lista;
	}

	lista->nodo_fin->siguiente = nuevo_nodo;
	lista->nodo_fin = nuevo_nodo;
	return lista;
}

size_t lista_tamanio(lista_t *lista)
{
	if (!lista)
		return 0;

	if (!lista->nodo_inicio)
		return 0;

	size_t contador = 0;
	nodo_t *nodo_aux = lista->nodo_inicio;

	while (nodo_aux) {
		contador++;
		nodo_aux = nodo_aux->siguiente;
	}

	return contador;
}

lista_t *lista_insertar_en_posicion(lista_t *lista, void *elemento,
				    size_t posicion)
{
	if (!lista)
		return NULL;

	nodo_t *nuevo_nodo = nodo_crear(elemento);
	if (!nuevo_nodo)
		return NULL;

	if (posicion == 0 || !lista->nodo_inicio) {
		nuevo_nodo->siguiente = lista->nodo_inicio;
		lista->nodo_inicio = nuevo_nodo;
		if (!lista->nodo_fin) {
			lista->nodo_fin = nuevo_nodo;
		}
		return lista;
	}

	nodo_t *nodo_ant = lista->nodo_inicio;
	size_t i = 0;

	while (i < posicion - 1 && nodo_ant->siguiente) {
		nodo_ant = nodo_ant->siguiente;
		i++;
	}

	nuevo_nodo->siguiente = nodo_ant->siguiente;
	nodo_ant->siguiente = nuevo_nodo;

	if (!nuevo_nodo->siguiente)
		lista->nodo_fin = nuevo_nodo;

	return lista;
}

void *lista_quitar(lista_t *lista)
{
	if (!lista)
		return NULL;

	if (!lista->nodo_inicio)
		return NULL;

	nodo_t *ultimo_nodo = lista->nodo_fin;
	void *elemento = ultimo_nodo->elemento;

	if (lista->nodo_inicio == lista->nodo_fin) {
		lista->nodo_inicio = NULL;
		lista->nodo_fin = NULL;
		free(ultimo_nodo);
		return elemento;
	}

	nodo_t *nodo_inicial = lista->nodo_inicio;

	while (nodo_inicial->siguiente != ultimo_nodo) {
		nodo_inicial = nodo_inicial->siguiente;
	}
	nodo_inicial->siguiente = NULL;
	lista->nodo_fin = nodo_inicial;
	free(ultimo_nodo);

	return elemento;
}

void *lista_quitar_de_posicion(lista_t *lista, size_t posicion)
{
	if (!lista)
		return NULL;

	if (!lista->nodo_inicio)
		return NULL;

	void *elemento = NULL;

	nodo_t *nodo_inicial = lista->nodo_inicio;
	nodo_t *nodo_anterior = NULL;

	for (size_t i = 0; i < posicion; i++) {
		if (!nodo_inicial->siguiente)
			return lista_quitar(lista);
		nodo_anterior = nodo_inicial;
		nodo_inicial = nodo_inicial->siguiente;
	}

	elemento = nodo_inicial->elemento;

	if (!nodo_anterior) {
		lista->nodo_inicio = nodo_inicial->siguiente;
		if (!lista->nodo_inicio)
			lista->nodo_fin = NULL;
		free(nodo_inicial);
		return elemento;
	}
	if (!nodo_inicial->siguiente) {
		lista->nodo_fin = nodo_anterior;
		nodo_anterior->siguiente = NULL;
		free(nodo_inicial);
		return elemento;
	}

	nodo_anterior->siguiente = nodo_inicial->siguiente;
	free(nodo_inicial);
	return elemento;
}

void *lista_elemento_en_posicion(lista_t *lista, size_t posicion)
{
	if (!lista)
		return NULL;

	if (!lista->nodo_inicio)
		return NULL;

	nodo_t *nodo_pedido = lista->nodo_inicio;

	for (size_t i = 0; i < posicion; i++) {
		if (!nodo_pedido->siguiente)
			return NULL;
		nodo_pedido = nodo_pedido->siguiente;
	}

	return nodo_pedido->elemento;
}

void *lista_buscar_elemento(lista_t *lista, int (*comparador)(void *, void *),
			    void *contexto)
{
	if (!lista || !comparador) {
		return NULL;
	}

	nodo_t *nodo_actual = lista->nodo_inicio;

	while (nodo_actual) {
		if (comparador(nodo_actual->elemento, contexto) == 0) {
			return nodo_actual->elemento;
		}
		nodo_actual = nodo_actual->siguiente;
	}

	return NULL;
}

void *lista_primero(lista_t *lista)
{
	if (!lista)
		return NULL;

	if (!lista->nodo_inicio)
		return NULL;

	return lista->nodo_inicio->elemento;
}

void *lista_ultimo(lista_t *lista)
{
	if (!lista)
		return NULL;

	if (!lista->nodo_fin)
		return NULL;

	return lista->nodo_fin->elemento;
}

bool lista_vacia(lista_t *lista)
{
	if (!lista)
		return true;
	return lista->nodo_inicio == NULL;
}

void lista_destruir(lista_t *lista)
{
	if (lista == NULL) {
		return;
	}

	if (lista_vacia(lista)) {
		free(lista);
		return;
	}

	nodo_t *nodo_actual = lista->nodo_inicio;

	while (nodo_actual) {
		nodo_t *nodo_a_eliminar = nodo_actual;
		nodo_actual = nodo_actual->siguiente;
		free(nodo_a_eliminar);
	}

	free(lista);
}

void lista_destruir_todo(lista_t *lista, void (*funcion)(void *))
{
	if (!lista) {
		return;
	}

	if (lista_vacia(lista)) {
		free(lista);
		return;
	}

	nodo_t *nodo_actual = lista->nodo_inicio;
	while (nodo_actual) {
		nodo_t *nodo_a_eliminar = nodo_actual;
		nodo_actual = nodo_actual->siguiente;

		if (funcion) {
			funcion(nodo_a_eliminar->elemento);
		}

		free(nodo_a_eliminar);
	}

	free(lista);
}

lista_iterador_t *lista_iterador_crear(lista_t *lista)
{
	if (!lista) {
		return NULL;
	}

	lista_iterador_t *iterador = malloc(sizeof(lista_iterador_t));
	if (!iterador) {
		return NULL;
	}

	iterador->lista = lista;
	iterador->nodo_actual = lista->nodo_inicio;

	return iterador;
}

bool lista_iterador_tiene_siguiente(lista_iterador_t *iterador)
{
	if (!iterador) {
		return false;
	}
	return iterador->nodo_actual != NULL;
}

bool lista_iterador_avanzar(lista_iterador_t *iterador)
{
	if (!iterador || !iterador->nodo_actual) {
		return false;
	}
	iterador->nodo_actual = iterador->nodo_actual->siguiente;

	return iterador->nodo_actual != NULL;
}

void *lista_iterador_elemento_actual(lista_iterador_t *iterador)
{
	if (!iterador || !iterador->nodo_actual) {
		return NULL;
	}
	return iterador->nodo_actual->elemento;
}

void lista_iterador_destruir(lista_iterador_t *iterador)
{
	free(iterador);
}

size_t lista_con_cada_elemento(lista_t *lista, bool (*funcion)(void *, void *),
			       void *contexto)
{
	if (!lista || !funcion) {
		return 0;
	}

	size_t elementos_iterados = 0;
	nodo_t *nodo_actual = lista->nodo_inicio;

	while (nodo_actual) {
		elementos_iterados++;
		if (!funcion(nodo_actual->elemento, contexto)) {
			break;
		}
		nodo_actual = nodo_actual->siguiente;
	}

	return elementos_iterados;
}

