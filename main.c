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

struct estado_juego{
	juego_t *juego;
	menu_t *menu;
	bool salir;
};


/**
* Este main debe ser modificado para que el usuario pueda jugar el juego. Las
* instrucciones existentes son solamente a modo ilustrativo del funcionamiento
* muy alto nivel del juego.
*
* Las interacciones deben realizarse por entrada/salida estandar y estar validadas.
*
* Se aconseja en todo momento mostrar información relevante para el jugador (por
* ejemplo, mostrar puntaje actual y movimientos disponibles) para hacer que el
* juego sea facil de utilizar.
*/

jugada_t jugador_pedir_nombre_y_ataque()
{
	jugada_t jugada;
	char nombre[20] = "Pikachu";
	char ataque[20] = "Latigo";
	strcpy(jugada.pokemon, nombre);
	strcpy(jugada.ataque, ataque);
	return jugada;
}

// int main(int argc, char *argv[])
// {

// 	// informacion_pokemon_t *info = pokemon_cargar_archivo("ejemplos/corto.txt");

// 	juego_t *juego = juego_crear();

// 	// //Pide al usuario un nombre de archivo de pokemones
// 	// char *archivo = pedir_archivo();

// 	// //Carga los pokemon
// 	JUEGO_ESTADO estado= juego_cargar_pokemon(juego, "ejemplos/correcto.txt");


// 	lista_t *lista = juego_listar_pokemon(juego);

// 	// //Crea un adversario que será utilizado como jugador 2
// 	adversario_t *adversario =
// 		adversario_crear(juego_listar_pokemon(juego));

// 	// //Mostrar el listado de pokemones por consola para que el usuario sepa las opciones que tiene
// 	// mostrar_pokemon_disponibles(juego);

// 	// //Pedirle al jugador por consola que ingrese los 3 nombres de pokemon que quiere utilizar
// 	const char *eleccionJugador1, *eleccionJugador2, *eleccionJugador3;
// 	juego_seleccionar_pokemon(juego, &eleccionJugador1,
// 				    &eleccionJugador2,
// 				    &eleccionJugador3);

// 	JUEGO_ESTADO estado_2 = juego_seleccionar_pokemon(juego, 0, "Pikachu", "Charmander", "Larvitar");
// 	printf("Estado seleccionar pokemon%d\n", estado_2);
// 	// //pedirle al adversario que indique los 3 pokemon que quiere usar
// 	char *eleccionAdversario1, *eleccionAdversario2, *eleccionAdversario3;
// 	bool seleccion = adversario_seleccionar_pokemon(adversario, &eleccionAdversario1,
// 				       &eleccionAdversario2,
// 				       &eleccionAdversario3);

// 	// //Seleccionar los pokemon de los jugadores
// 	// juego_seleccionar_pokemon(juego, JUGADOR1, eleccionJugador1,
// 	// 			  eleccionJugador2, eleccionJugador3);
// 	// juego_seleccionar_pokemon(juego, JUGADOR2, eleccionAdversario1,
// 	// 			  eleccionAdversario2, eleccionAdversario3);

// 	// //informarle al adversario cuales son los pokemon del jugador
// 	adversario_pokemon_seleccionado(adversario, eleccionJugador1,
// 					eleccionJugador2, eleccionJugador3);

// 	// while (!juego_finalizado(juego)) {
// 		resultado_jugada_t resultado_ronda;

// 	// 	//Pide al jugador que ingrese por consola el pokemon y ataque para la siguiente ronda
// 		jugada_t jugada_jugador = jugador_pedir_nombre_y_ataque();

// 	// 	//Pide al adversario que informe el pokemon y ataque para la siguiente ronda
// 		// jugada_t jugada_adversario =
// 		// 	adversario_proxima_jugada(adversario);
// 		jugada_t jugada_adversario = jugador_pedir_nombre_y_ataque();

// 	// 	//jugar la ronda y después comprobar que esté todo ok, si no, volver a pedir la jugada del jugador
// 		resultado_ronda = juego_jugar_turno(juego, jugada_jugador,
// 						    jugada_adversario);

// 	// 	//Si se pudo jugar el turno, le informo al adversario la jugada realizada por el jugador
// 	// 	adversario_informar_jugada(adversario, jugada_jugador);
// 	// }

// 	// juego_destruir(juego);
// }

bool solicitar_archivo(void *estado){
	struct estado_juego *estado_juego = estado;
	char ruta_archivo[MAX_RUTA_LONGITUD];

    printf("Por favor, ingrese la ruta del archivo: ");
    
    if (fgets(ruta_archivo, MAX_RUTA_LONGITUD, stdin) == NULL) {
        printf("Error al leer la ruta del archivo.\n");
        return false;
    }

    ruta_archivo[strcspn(ruta_archivo, "\n")] = 0;
    printf("Gracias. La ruta del archivo ingresada: '%s'\n", ruta_archivo);

	JUEGO_ESTADO juego_estado = juego_cargar_pokemon(estado_juego->juego, ruta_archivo);
	switch (juego_estado){
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

const char *mostrar_tipo_ataque(enum TIPO tipo) {
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
        default:
            return "Desconocido";
    }
}

void imprimir_ataque(const struct ataque *ataque, void *extra){
	printf(" - %s\n", ataque->nombre);
	printf("   Poder: %d\n", ataque->poder);
	printf("   Tipo: %s\n", mostrar_tipo_ataque(ataque->tipo));
}

bool solicitar_seleccionar_pokemones(void *estado){
	struct estado_juego *estado_juego = estado;

	lista_t *lista = juego_listar_pokemon(estado_juego->juego);
	if(lista_tamanio(lista) == 0){
		printf("No hay pokemones disponibles para seleccionar.\n");
		printf("Por favor, cargue un archivo de pokemones. (Opcion 'C')\n\n");
		return true;
	}

	printf("Pokemones disponibles:\n");
    for (int i = 0; i < lista_tamanio(lista); ++i) {
        pokemon_t *pokemon = lista_elemento_en_posicion(lista, i);
        if (pokemon != NULL) {
            printf("%d. %s de TIPO: %s\n", i + 1, pokemon_nombre(pokemon), mostrar_tipo_ataque(pokemon_tipo(pokemon)));
			con_cada_ataque(pokemon, imprimir_ataque, NULL);
        }
    }

	char *eleccionJugador1, *eleccionJugador2, *eleccionJugador3;
	for(int i = 0; i < 3; i++){
		printf("Por favor, ingrese el nombre del pokemon %d: ", i+1);
		char nombre_pokemon[20];
		if (fgets(nombre_pokemon, 20, stdin) == NULL) {
			printf("Error al leer el nombre del pokemon.\n");
			return false;
		}
		nombre_pokemon[strcspn(nombre_pokemon, "\n")] = 0;
		printf("Gracias. El nombre del pokemon ingresado: '%s'\n\n", nombre_pokemon);


		if (i == 0) eleccionJugador1 = strdup(nombre_pokemon);
        else if (i == 1) eleccionJugador2 = strdup(nombre_pokemon);
        else eleccionJugador3 = strdup(nombre_pokemon);
	}

	JUEGO_ESTADO juego_estado = juego_seleccionar_pokemon(estado_juego->juego, JUGADOR1, 
					eleccionJugador1, eleccionJugador2, eleccionJugador3);

	switch (juego_estado){
		case POKEMON_INEXISTENTE:
			printf("Un pokemon ingresado no existe. Intente seleccionar nuevamente.\n\n");
			break;
		case POKEMON_REPETIDO:
			printf("No pude seleccionar pokemones repetidos.\n\n");
			break;
		case TODO_OK:
			printf("Pokemones seleccionados correctamente.\n");
			printf("Usted selecciono: '%s', '%s', '%s'\n\n", eleccionJugador1, eleccionJugador2, eleccionJugador3);
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
	
	free(eleccionJugador1);
	free(eleccionJugador2);
	free(eleccionJugador3);
	return true;
}

bool solicitar_jugar(void *estado){
	return true;
}

bool salir(void *estado){
	return true;
}

bool mostrar_pokemones(void *estado){
	return true;
}

bool mostrar_ayuda(void *estado){
	return true;
}

int main(int argc, char *argv[]){
	juego_t *juego = juego_crear();
	struct estado_juego estado = { .juego = juego, .salir = false };
 
	menu_t *menu = menu_crear();

	menu_agregar_opcion(menu, "Cargar archivo de pokemones", "C", solicitar_archivo);
	menu_agregar_opcion(menu, "Seleccionar pokemones", "S", solicitar_seleccionar_pokemones);
	menu_agregar_opcion(menu, "Jugar", "J", solicitar_jugar);
	menu_agregar_opcion(menu, "Salir", "Q", salir);
	menu_agregar_opcion(menu, "Mostrar pokemones", "M", mostrar_pokemones);
	menu_agregar_opcion(menu, "Ayuda", "H", mostrar_ayuda);

	printf("Bienvenido a Pokemon Go!\n");
	printf("Ingrese comandos para jugar, ingrese 'H' para ver la ayuda\n");

	while(!estado.salir){
		printf("TP2> ");

		char linea[100];
		char *leido = fgets(linea, 100, stdin);
		if(leido == NULL){
			continue;
		}

		linea[strlen(linea)-1] = '\0';
		MENU_RESULTADO menu_resultado = menu_mostrar(menu, linea, &estado);
		if(menu_resultado == MENU_ERROR){
			printf("Hubo un problema al ejecutar.\n\n");
		}
		if(menu_resultado == OPCION_INEXISTENTE){
			printf("La opcion ingresada no existe.\n\n");
		}
		if(menu_resultado == MENU_SALIR){
			printf("Saliendo...\n\n");
			break;
		}
		printf("Ingrese otro comando para seguir jugando..\n");
	}

	menu_destruir(menu);
 }
