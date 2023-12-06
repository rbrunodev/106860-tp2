#include "src/pokemon.h"
#include "src/ataque.h"
#include "src/juego.h"
#include "src/lista.h"
#include "src/adversario.h"
#include "src/menu.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_RUTA_LONGITUD 100

struct estado_juego {
	int rondas;
	juego_t *juego;
	menu_t *menu;
	adversario_t *adversario;
	bool salir;
};


bool solicitar_archivo(void *estado)
{
	struct estado_juego *estado_juego = estado;
	char ruta_archivo[MAX_RUTA_LONGITUD];

	printf("Por favor, ingrese la ruta del archivo: ");

	if (!fgets(ruta_archivo, MAX_RUTA_LONGITUD, stdin)) {
		printf("Error al leer la ruta del archivo.\n");
		return false;
	}

	ruta_archivo[strcspn(ruta_archivo, "\n")] = 0;
	printf("Gracias. La ruta del archivo ingresada: '%s'\n", ruta_archivo);

	JUEGO_ESTADO juego_estado =
		juego_cargar_pokemon(estado_juego->juego, ruta_archivo);
	switch (juego_estado) {
	case POKEMON_INSUFICIENTES:
		printf("No hay pokemon suficientes como para jugar. Intente nuevamente.\n\n");
		break;
	case TODO_OK:
		printf("Archivo cargado correctamente.\n\n");
		break;
	case ERROR_GENERAL:
		printf("Hubo un error al cargar el archivo.\n\n");
		estado_juego->salir = true;
		break;
	default:
		printf("Error desconocido.\n\n");
		estado_juego->salir = true;
		break;
	}

	return true;
}

const char *mostrar_tipo_ataque(enum TIPO tipo)
{
	switch (tipo) {
	case NORMAL:
		return "Normal";
	case FUEGO:
		return "Fuego";
	case AGUA:
		return "Agua";
	case ELECTRICO:
		return "Electrico";
	case ROCA:
		return "Roca";
	case PLANTA:
		return "Planta";
	default:
		return "Desconocido";
	}
}

void imprimir_ataque(const struct ataque *ataque, void *extra)
{
	printf(" - %s\n", ataque->nombre);
	printf("   Poder: %d\n", ataque->poder);
	printf("   Tipo: %s\n", mostrar_tipo_ataque(ataque->tipo));
}

bool solicitar_seleccionar_pokemones(void *estado)
{
	struct estado_juego *estado_juego = estado;

	lista_t *lista = juego_listar_pokemon(estado_juego->juego);
	if (lista_tamanio(lista) == 0) {
		printf("No hay pokemones disponibles para seleccionar.\n");
		printf("Por favor, cargue un archivo de pokemones. (Opcion 'C')\n\n");
		return true;
	}

	printf("Pokemones disponibles:\n");
	for (int i = 0; i < lista_tamanio(lista); ++i) {
		pokemon_t *pokemon = lista_elemento_en_posicion(lista, i);
		if (pokemon != NULL) {
			printf("%d. %s de TIPO: %s\n", i + 1,
			       pokemon_nombre(pokemon),
			       mostrar_tipo_ataque(pokemon_tipo(pokemon)));
			con_cada_ataque(pokemon, imprimir_ataque, NULL);
		}
	}

	char *eleccionJugador1, *eleccionJugador2, *eleccionJugador3;
	for (int i = 0; i < 3; i++) {
		printf("Por favor, ingrese el nombre del pokemon %d: ", i + 1);
		char nombre_pokemon[20];
		if (!fgets(nombre_pokemon, 20, stdin)) {
			printf("Error al leer el nombre del pokemon.\n");
			return false;
		}
		nombre_pokemon[strcspn(nombre_pokemon, "\n")] = 0;
		printf("Gracias. El nombre del pokemon ingresado: '%s'\n\n",
		       nombre_pokemon);

		if (i == 0)
			eleccionJugador1 = strdup(nombre_pokemon);
		else if (i == 1)
			eleccionJugador2 = strdup(nombre_pokemon);
		else
			eleccionJugador3 = strdup(nombre_pokemon);
	}

	adversario_t *adversario =
		adversario_crear(juego_listar_pokemon(estado_juego->juego));
	if (!adversario) {
		return false;
	}
	estado_juego->adversario = adversario;

	char *eleccionAdversario1, *eleccionAdversario2, *eleccionAdversario3;
	adversario_seleccionar_pokemon(adversario, &eleccionAdversario1,
				       &eleccionAdversario2,
				       &eleccionAdversario3);

	JUEGO_ESTADO juego_estado = juego_seleccionar_pokemon(
		estado_juego->juego, JUGADOR1, eleccionJugador1,
		eleccionJugador2, eleccionAdversario3);

	juego_seleccionar_pokemon(estado_juego->juego, JUGADOR2,
				  eleccionAdversario1, eleccionAdversario2,
				  eleccionJugador3);

	switch (juego_estado) {
	case POKEMON_INEXISTENTE:
		printf("Un pokemon ingresado no existe. Intente seleccionar nuevamente.\n\n");
		break;
	case POKEMON_REPETIDO:
		printf("No puede seleccionar pokemones repetidos.\n\n");
		break;
	case TODO_OK:
		printf("Pokemones seleccionados correctamente.\n");
		printf("Usted selecciono: '%s', '%s', '%s'\n\n",
		       eleccionJugador1, eleccionJugador2, eleccionJugador3);
		break;
	case ERROR_GENERAL:
		printf("Hubo un error seleccionar los pokemones.\n");
		estado_juego->salir = true;
		break;
	default:
		printf("Error desconocido.\n");
		estado_juego->salir = true;
		break;
	}

	adversario_pokemon_seleccionado(adversario, eleccionJugador1,
					eleccionJugador2, eleccionJugador3);

	free(eleccionJugador1);
	free(eleccionJugador2);
	free(eleccionJugador3);
	return true;
}

int comparador(void *p1, void *nombre)
{
	pokemon_t *pokemon = p1;
	return strcmp(pokemon_nombre(pokemon), (char *)nombre);
}

jugada_t solicitar_jugada(juego_t *juego)
{
	jugada_t jugada;

	printf("Por favor, ingrese nombre y ataque del pokemon con el que va a jugar. \n");
	printf("NOMBRE: ");

	char nombre_pokemon[20];
	if (!fgets(nombre_pokemon, 20, stdin)) {
		printf("Error al leer el nombre del pokemon.\n");
		return jugada;
	}
	nombre_pokemon[strcspn(nombre_pokemon, "\n")] = 0;

	lista_t *lista = juego_listar_pokemon(juego);

	pokemon_t *pokemon =
		lista_buscar_elemento(lista, comparador, nombre_pokemon);
	if (!pokemon) {
		printf("El pokemon ingresado no existe. Vuelva a intentarlo\n");
		return jugada;
	}
	printf("ATAQUE: ");

	char nombre_ataque[20];
	if (!fgets(nombre_ataque, 20, stdin)) {
		printf("Error al leer el nombre del ataque.\n");
		return jugada;
	}

	nombre_ataque[strcspn(nombre_ataque, "\n")] = 0;
	const struct ataque *ataque =
		pokemon_buscar_ataque(pokemon, nombre_ataque);
	if (!ataque) {
		printf("El ataque ingresado no existe para ese pokemon. Vuelva a intentarlo\n");
		return jugada;
	}

	printf("Su ronda es: %s ; %s\n", nombre_pokemon, nombre_ataque);

	strcpy(jugada.pokemon, nombre_pokemon);
	strcpy(jugada.ataque, nombre_ataque);
	return jugada;
}

bool solicitar_jugar(void *estado)
{
	printf("Comenzando el juego...\n");
	struct estado_juego *estado_juego = estado;

	while (!juego_finalizado(estado_juego->juego)) {
		printf("\nRonda: %d\n", estado_juego->rondas + 1);
		jugada_t jugada_jugador_1 =
			solicitar_jugada(estado_juego->juego);
		if (jugada_jugador_1.pokemon[0] == '\0' ||
		    jugada_jugador_1.ataque[0] == '\0') {
			printf("Hubo un error al leer la jugada. Vuelva a intentarlo\n");
			continue;
		}
		jugada_t jugada_juagador_2 =
			adversario_proxima_jugada(estado_juego->adversario);
		if (jugada_juagador_2.pokemon[0] == '\0' ||
		    jugada_juagador_2.ataque[0] == '\0') {
			printf("Hubo un error al leer la jugada. Vuelva a intentarlo\n");
			continue;
		}
		resultado_jugada_t resultado_ronda =
			juego_jugar_turno(estado_juego->juego, jugada_jugador_1,
					  jugada_juagador_2);

		if (resultado_ronda.jugador1 == ATAQUE_ERROR ||
		    resultado_ronda.jugador2 == ATAQUE_ERROR) {
			printf("Hubo un error al jugar la ronda. Vuelva a intentarlo\n");
			continue;
		}
		estado_juego->rondas++;

		printf("RONDA: %d CONCLUIDA", estado_juego->rondas);
		printf("JUGADOR: %d PUNTOS === ADVERSARIO: %d PUNTOS \n",
		       juego_obtener_puntaje(estado_juego->juego, JUGADOR1),
		       juego_obtener_puntaje(estado_juego->juego, JUGADOR2));
	}

	int puntaje_jugador_1 =
		juego_obtener_puntaje(estado_juego->juego, JUGADOR1);
	int puntaje_jugador_2 =
		juego_obtener_puntaje(estado_juego->juego, JUGADOR2);

	if (puntaje_jugador_1 > puntaje_jugador_2) {
		printf("HAS GANADO! Sumaste %d puntos!\n", puntaje_jugador_1);

	} else if (puntaje_jugador_1 < puntaje_jugador_2) {
		printf("GAME OVER! El ganador fue jugador 2 con %d puntos\n",
		       puntaje_jugador_2);
	} else {
		printf("Empate\n");
	}

	printf("Juego terminado!\n");
	estado_juego->salir = true;
	return true;
}

bool salir(void *estado)
{
	return true;
}

bool mostrar_pokemones(void *estado)
{
	return true;
}

bool mostrar_ayuda(void *estado)
{
	return true;
}

int main(int argc, char *argv[])
{
	juego_t *juego = juego_crear();
	struct estado_juego estado = { .juego = juego, .salir = false };

	menu_t *menu = menu_crear();

	menu_agregar_opcion(menu, "Cargar archivo de pokemones", "C",
			    solicitar_archivo);
	menu_agregar_opcion(menu, "Seleccionar pokemones", "S",
			    solicitar_seleccionar_pokemones);
	menu_agregar_opcion(menu, "Jugar", "J", solicitar_jugar);
	menu_agregar_opcion(menu, "Salir", "Q", salir);
	menu_agregar_opcion(menu, "Mostrar pokemones", "M", mostrar_pokemones);
	menu_agregar_opcion(menu, "Ayuda", "H", mostrar_ayuda);

	printf("Bienvenido a Pokemon Go!\n");
	printf("Ingrese comandos para jugar, ingrese 'H' para ver la ayuda\n");

	while (!juego_finalizado(estado.juego) || !estado.salir) {
		printf("TP2> ");

		char linea[100];
		char *leido = fgets(linea, 100, stdin);
		if (!leido) {
			continue;
		}

		linea[strlen(linea) - 1] = '\0';
		MENU_RESULTADO menu_resultado =
			menu_mostrar(menu, linea, &estado);
		if (menu_resultado == MENU_ERROR) {
			printf("Hubo un problema al ejecutar.\n\n");
		}
		if (menu_resultado == OPCION_INEXISTENTE) {
			printf("La opcion ingresada no existe.\n\n");
		}
		if (menu_resultado == MENU_SALIR) {
			printf("Saliendo...\n\n");
			break;
		}
		printf("Ingrese otro comando para seguir jugando..\n");
	}

	printf("\n Gracias por jugar!\n");

	adversario_destruir(estado.adversario);
	juego_destruir(estado.juego);
	menu_destruir(estado.menu);
	return 0;
}
