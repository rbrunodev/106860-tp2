<div align="right">
<img width="32px" src="img/algo2.svg">
</div>

# TP2

## Repositorio de Renata Bruno  - 106860 - rbruno@fi.uba.ar

- Para compilar:

```bash
gcc main.c src/pokemon.c src/juego.c src/adversario.c src/lista.c src/menu.c src/hash.c -o mi_programa
```

- Para ejecutar:

```bash
./mi_programa
```

- Para ejecutar con valgrind:
```bash
compilo: gcc -o pruebas_alumno pruebas_alumno.c
ejecuto: ./pruebas_alumno 

valgrind ./pruebas_alumno 
```

---

## Respuestas a las preguntas teóricas
Para la logica del juego implemente el TDA de lista, ya que me parecio que era la estructura mas adecuada para el juego, ya que se necesita una estructura que permita agregar y eliminar elementos de forma eficiente, y que permita recorrerla de forma secuencial. Ademas implemente el TDA de pokemon para guardar y manejar la informacion de los pokemones de la manera que en ese TDA se utiliza. Las operaciones de ambos TDA me permitian realizar lo que necesita para darle el correcto funcionamiento al juego. Incorporar la lista me permitio iterar sobre cada elemento de ella y poder implementar las funciones del TDA del pokemon, para por ejemplo devolverlo un pokemon en base a su nombre, o devolver un ataque en base a su nombre.

Para la logica del adversario tambienn implemente el TDA de lista y utilice el de hash, practicamente de la misma forma, porque las operaciones alli implementadas me permitian realizar lo que necesitaba para el correcto funcionamiento del juego. 

Fuera del juego y el adversario, para el menu implemente el tda de hash, ya que maneje cada opcion del menu como una clave y cada funcion como un valor. Esto me permitio poder acceder a cada funcion de forma eficiente, y poder agregar y eliminar opciones del menu de forma eficiente, y llamar a cada funcion que el usuario eligio de forma rapida.

**Complejidades**

Analisis de la complejidad de las funciones implementadas en juego.c
- `juego_crear`: O(1)

```c
s1 juego_t *juego = calloc(1, sizeof(juego_t));
```
s1 es O(1) ya que solo se realiza una asignacion de memoria dinamica.

```c
if(!juego)
    return NULL;
```
Esta linea es O(1) ya que solo se realiza una comparacion.
Por lo tanto la complejidad de la funcion es O(1).

- `juego_cargar_pokemon`: O(n)

```c
if (!juego || !archivo) {
    return ERROR_GENERAL;
}
```
Esta linea es O(1) ya que solo se realiza una comparacion.

```c
informacion_pokemon_t *info = pokemon_cargar_archivo(archivo);
```
Esta linea depende de la complejidad de la funcion pokemon_cargar_archivo. Donde la complejidad es O(n) ya que se realiza un ciclo while que itera hasta que se termine de leer el archivo, por ende el archivo puede tener n lineas, y en cada iteracion se realiza una asignacion de memoria dinamica, una comparacion y una asignacion de valor.

Las verificaciones siguientes y las asignaciones de valor son O(1).

```c
int cantidad = pokemon_cantidad(info);
if (cantidad <= 3) {
    pokemon_destruir_todo(info);
    return POKEMON_INSUFICIENTES;
}
```
En este caso, la funcion pokemon_cantidad es O(1) ya que solo se realiza una comparacion y pokemon_destruir_todo tambien es O(1).

Por lo tanto, todas las complejidades son O(1) excepto la de pokemon_cargar_archivo que es O(n), entonces la complejidad de la funcion es O(n).

- `juego_listar_pokemon`: O(n^2)

```c
if (!juego) {
		return NULL;
	}

lista_t *lista = lista_crear();
if (!lista) {
    return NULL;
}

nombres_pokemon_t *nombres_pokemones =
    calloc(1, sizeof(nombres_pokemon_t));
```
Estas lineas son O(1) ya que solo se realizan asignaciones de memoria dinamica y verificaciones.


```c
int iterados = con_cada_pokemon(juego->info_pokemones,
					obtener_nombre_pokemon,
					nombres_pokemones);
```

Esta linea depende de la complejidad de la funcion con_cada_pokemon, la cual es O(n) ya que itera en base a la cantidad de pokemones que hay cargados, es decir a n pokemones.

```c
if (iterados == 0) {
    free(lista);
    free(nombres_pokemones);
    return NULL;
}
```

Estas lineas son O(1).

```c
for (int i = 0; i < iterados; i++) {
		pokemon_t *pokemon = pokemon_buscar(
			juego->info_pokemones, nombres_pokemones->nombres[i]);
		if (pokemon != NULL) {
			juego->lista_pokemones = lista_insertar(lista, pokemon);
		}
	}
```

Aca tenemos un ciclo for que itera en base a la cantidad de pokemones (n). Por cada iteracion:
 - Llama a pokemon_buscar, donde la complejidad es O(n), ya que tambien itera sobre la cantidad de pokemones (n).  
 - Llama a lista_insertar, donde la complejidad es O(1).
Por lo tanto este ciclo for es O(n).

En conclusion, la complejidad total es dominada por la combinación de con_cada_pokemon y el bucle for con pokemon_buscar dentro de él:
O(n) * O(n) = O(n^2)

- `juego_seleccionar_pokemon`: O(n)

Las verificaiones y asignaciones de valor realizadas en esta funcion son todas O(1). Por lo que la funcion depende de pokemon_buscar, la cual es O(n) ya que itera sobre la cantidad de pokemones (n), esta busqueda se realiza 3 veces, la complejidad es O(3n) = O(n).

- `juego_jugar_turno`: O(n)

Las funciones de las cuales puede variar la complejidad son:
- pokemon_buscar: O(n)
- pokemon_buscar_ataque: O(1)
- ataque_utilizado: O(n)
- efectividad_ataque: O(1)
Ya que las verificaciones (if) y las asignaciones que se hacen dentro de esta funcion son O(1). Por lo tanto la complejidad total es O(n) + O(n) + O(1) + O(1) + O(1)= O(2n) + O(3) = O(n).

- `juego_obtener_puntaje`: O(1)

Todas las operaciones de esta funcion se realizan en tiempo constante, no dependen de ningun factor externo, por lo tanto la complejidad es O(1).

- `juego_finalizado`: O(1)
Todas las operaciones de esta funcion se realizan en tiempo constante, no dependen de ningun factor externo, por lo tanto la complejidad es O(1).

- `juego_destruir`: O(n)

La complejidad de esta funcion depende de pokemon_destruit_todo y lista_destruir. Analizando estas funciones:
- pokemon_destruir_todo: O(1) 
- lista_destruir: O(n), ya que la funcion recorre cada nodo en la lista enlazada y libera la memoria de cada nodo, por lo tanto depende de la cantidad de nodos que tenga la lista.
- free(juego): O(1)

Por lo tanto la complejidad total es O(1) + O(n) + O(1) = O(n).