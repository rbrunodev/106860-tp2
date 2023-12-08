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
línea con valgrind
```
---
##  Funcionamiento

Explicación de cómo funcionan las estructuras desarrolladas en el TP y el funcionamiento general del mismo.

Aclarar en esta parte todas las decisiones que se tomaron al realizar el TP, cosas que no se aclaren en el enunciado, fragmentos de código que necesiten explicación extra, etc.

Incluír **EN TODOS LOS TPS** los diagramas relevantes al problema (mayormente diagramas de memoria para explicar las estructuras, pero se pueden utilizar otros diagramas si es necesario).

### Por ejemplo:

El programa funciona abriendo el archivo pasado como parámetro y leyendolo línea por línea. Por cada línea crea un registro e intenta agregarlo al vector. La función de lectura intenta leer todo el archivo o hasta encontrar el primer error. Devuelve un vector con todos los registros creados.

<div align="center">
<img width="70%" src="img/diagrama1.svg">
</div>

En el archivo `sarasa.c` la función `funcion1` utiliza `realloc` para agrandar la zona de memoria utilizada para conquistar el mundo. El resultado de `realloc` lo guardo en una variable auxiliar para no perder el puntero original en caso de error:

```c
int *vector = realloc(vector_original, (n+1)*sizeof(int));

if(vector == NULL)
    return -1;
vector_original = vector;
```


<div align="center">
<img width="70%" src="img/diagrama2.svg">
</div>

---

## Respuestas a las preguntas teóricas
Para la logica del juego implemente el TDA de lista, ya que me parecio que era la estructura mas adecuada para el juego, ya que se necesita una estructura que permita agregar y eliminar elementos de forma eficiente, y que permita recorrerla de forma secuencial. Ademas implemente el TDA de pokemon para guardar y manejar la informacion de los pokemones de la manera que en ese TDA se utiliza. Las operaciones de ambos TDA me permitian realizar lo que necesita para darle el correcto funcionamiento al juego. Incorporar la lista me permitio iterar sobre cada elemento de ella y poder implementar las funciones del TDA del pokemon, para por ejemplo devolverlo un pokemon en base a su nombre, o devolver un ataque en base a su nombre.

Para la logica del adversario tambienn implemente el TDA de lista y utilice el de hash, practicamente de la misma forma, porque las operaciones alli implementadas me permitian realizar lo que necesitaba para el correcto funcionamiento del juego. 

Fuera del juego y el adversario, para el menu implemente el tda de hash, ya que maneje cada opcion del menu como una clave y cada funcion como un valor. Esto me permitio poder acceder a cada funcion de forma eficiente, y poder agregar y eliminar opciones del menu de forma eficiente, y llamar a cada funcion que el usuario eligio de forma rapida.