#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//ESTRUCTURAS
typedef struct {
  char titulo[101];
  char album[101];
  List *artistas;   // lista de strings
  char genero[51];
  float tempo;
} Cancion;

// Funcion que permite comparar strings
int comparar_strings(void *a, void *b) {
  return strcmp((char *)a, (char *)b) == 0;
}

int CancionesACargar = 0; // Variable global

/**
 * Carga canciones desde un archivo CSV
 */

void leer_canciones(){
  
  printf("\nMaximo de canciones disponibles --> 114000\n");
  printf("Cuantas canciones cargaras hoy ??: ");
  scanf("%d", &CancionesACargar);

  if(CancionesACargar <= 0 || CancionesACargar > 114000){
    printf("\nLa cantidad no existe. intenta nuevamente :/\n\n");
    return;
  }

  FILE *archivo = fopen("data/song_dataset_.csv", "r");
  if (archivo == NULL) {
    perror("Error al abrir el archivo");
    return;
  }

  char **campos;
  campos = leer_linea_csv(archivo, ','); // encabezados 

  int k = 0;
  while ((campos = leer_linea_csv(archivo, ',')) != NULL && k < CancionesACargar){
    // Mostrar canciones
    printf("\nTitulo cancion: %s\n", campos[4]);

    char *artistas_duplicados = strdup(campos[2]);
    List* artistas = split_string(campos[2], ";");
    free(artistas_duplicados);

    printf("Artistas:\n");
    char *artista = list_first(artistas);
    while(artista != NULL){
      printf(" %s\n", artista);
      artista = list_next(artistas);
    }

    printf("Album: %s\n", campos[3]);
    printf("Genero: %s\n", campos[20]);
    printf("Tempo: %.2f\n", atof(campos[18]));
    printf(" -------------------------------\n");

    k++;
  }
  printf("\nYA PUEDES DISFRUTAR DE TODAS TUS CANCIONES !!\n\n");

  fclose(archivo);
}

// Mapa global para los generos
Map *mapaGeneros = NULL;

// Funcion que carga las canciones por genero en un map 
Map* cargar_generos() {

  if(CancionesACargar == 0){
    printf("Primero carga algunas canciones.\n");
    return NULL;
  }

  FILE *archivo = fopen("data/song_dataset_.csv", "r");
  if (archivo == NULL) {
    perror("No se pudo abrir el archivo CSV");
    return NULL;
  }

  Map *map = map_create(comparar_strings); // clave: nombre del género, valor: List* canciones
  char **campos = leer_linea_csv(archivo, ','); // saltar encabezado

  int k = 0;
  while ((campos = leer_linea_csv(archivo, ',')) != NULL && k < CancionesACargar){
    Cancion *c = malloc(sizeof(Cancion));
    strcpy(c->titulo, campos[4]);
    strcpy(c->album, campos[3]);

    char *artistas_duplicados = strdup(campos[2]);
    c->artistas = split_string(campos[2], ";");
    free(artistas_duplicados);

    strcpy(c->genero, campos[20]);
    c->tempo = atof(campos[18]);

    // Buscar lista del genero o crearla
    List *lista = NULL;
    MapPair *par = map_search(map, c->genero);
    if (par != NULL) {
      lista = par->value;
    } else {
      lista = list_create();
      map_insert(map, strdup(c->genero), lista);
    }

    list_pushBack(lista, c);
    k++;
  }

  fclose(archivo);
  return map;
}

// Funcion de busqueda por genero
void buscar_por_genero() {
  mapaGeneros = cargar_generos();
  if (mapaGeneros == NULL) return;

  char genero[51];
  printf("Ingrese el genero en cuestion: ");
  scanf(" %[^\n]", genero);

  MapPair *par = map_search(mapaGeneros, genero);
  if (par == NULL) {
    printf("\nNo se encontraron canciones del genero :/'%s'\n\n", genero);
    return;
  }

  List *canciones = par->value;
  printf("\nCanciones del genero '%s':\n", genero);
  for (Cancion *c = list_first(canciones); c != NULL; c = list_next(canciones)) {
    printf("| Titulo: %s | Album: %s | Tempo: %.2f\n",
            c->titulo, c->album, c->tempo);
  }
  printf("\nYa tienes todas las canciones de este genero :)\n\n");
}

// Mapa global para artistas
Map *mapaArtistas = NULL;

// Funcion para guardar canciones por artista
Map* guardar_artistas(){

  if(CancionesACargar == 0){
    printf("Primero carga algunas canciones.\n");
    return NULL;
  }

  FILE *archivo = fopen("data/song_dataset_.csv", "r");
  if (archivo == NULL) {
    perror("No se pudo abrir el archivo CSV");
    return NULL;
  }

  Map *mapa = map_create(comparar_strings); // clave: nombre del artista, valor: List* canciones
  char **campos = leer_linea_csv(archivo, ','); // saltar encabezado

  int k = 0;
  while ((campos = leer_linea_csv(archivo, ',')) != NULL && k < CancionesACargar){
    Cancion *c = malloc(sizeof(Cancion));
    strcpy(c->titulo, campos[4]);
    strcpy(c->album, campos[3]);

    // COPIAR campos[2] antes de usar split_string
    char *artistas_duplicado = strdup(campos[2]);
    c->artistas = split_string(artistas_duplicado, ";");
    free(artistas_duplicado); // liberar copia después de usarla

    strcpy(c->genero, campos[20]);
    c->tempo = atof(campos[18]);

    for (char *artista = list_first(c->artistas); artista != NULL; artista = list_next(c->artistas)) {
      List *lista;
      MapPair *par = map_search(mapa, artista);
      if (par != NULL) {
        lista = par->value;
      } else {
        lista = list_create();
        map_insert(mapa, strdup(artista), lista);
      }
      list_pushBack(lista, c);
    }

    k++;
  }

  fclose(archivo);
  return mapa;
}

// Funcion para buscar por artista
void buscar_por_artista() {
  if (mapaArtistas == NULL) {
    mapaArtistas = guardar_artistas();
    if (mapaArtistas == NULL) {
      printf("No se pudo cargar el mapa de artistas.\n");
      return;
    }
  }

  char artista[101];
  printf("Ingrese el nombre del artista: ");
  scanf(" %[^\n]", artista);

  MapPair *par = map_search(mapaArtistas, artista);
  if (par == NULL) {
    printf("\nNo se encontraron canciones de ese artista :/ '%s'\n\n", artista);
    return;
  }

  List *canciones = par->value;
  printf("\nCanciones del artista '%s':\n", artista);
  for (Cancion *c = list_first(canciones); c != NULL; c = list_next(canciones)) {
    printf("| Titulo: %s | Album: %s | Genero: %s | Tempo: %.2f\n",
           c->titulo, c->album, c->genero, c->tempo);
  }
  printf("\nYa tienes todas las canciones de este artista :)\n\n");
}

// Mapa global para los distintos tempos
Map *mapTempo = NULL;

// Funcion que carga canciones por tempo
Map* guardar_tempo(){
  if(CancionesACargar == 0){
    printf("\nPrimero carga algunas canciones\n");
    return NULL;
  }

  FILE *archivo = fopen("data/song_dataset_.csv", "r");
  if(archivo == NULL){
    perror("NO se pudo abrir el archiuvo CSV");
    return NULL;
  }

  Map *mapa = map_create(comparar_strings); // clave "lenta", "moderna" y "rapida"

  // Crea las listas para cada tempo
  map_insert(mapa, strdup("lenta"), list_create());
  map_insert(mapa, strdup("moderada"), list_create());
  map_insert(mapa, strdup("rapida"), list_create());

  char **campos = leer_linea_csv(archivo, ','); // encabezado
  int k = 0;

  while ((campos = leer_linea_csv(archivo, ',')) != NULL && k < CancionesACargar) {
    Cancion *c = malloc(sizeof(Cancion));
    strcpy(c->titulo, campos[4]);
    strcpy(c->album, campos[3]);

    char *artistas_duplicado = strdup(campos[2]);
    c->artistas = split_string(artistas_duplicado, ";");
    free(artistas_duplicado);

    strcpy(c->genero, campos[20]);
    c->tempo = atof(campos[18]);

    // Determinamos el tempo 
    char *categoria;
    if (c->tempo < 80.0){
      categoria = "lenta";
    } else if (c->tempo >= 80.0 && c->tempo < 120.0){
      categoria = "moderada";
    } else {
      categoria = "rapida";
    }

    MapPair *par = map_search(mapa, categoria);
    List *lista = par->value;
    list_pushBack(lista, c);

    k++;
  }

  fclose(archivo);
  return mapa;
}

// Funcion para buscar canciones por tempo
void buscar_por_tempo(){
  if(mapTempo == NULL){
    mapTempo = guardar_tempo();
    if(mapTempo == NULL) return;
  }

  // Menu interactivo
  printf("\nQue tipo de tempo estas buscando hoy ??\n");
  printf("1) Lentas --> (Menor de 80)\n");
  printf("2) Moderadas --> (De 80 a 120)\n");
  printf("3) Rapidas --> (Mayor a 120)\n");
  printf("Selecciona una opcion (1-3): ");

  // opcion del usuario
  char opcionDeTempo[22];
  scanf(" %s", opcionDeTempo);

  char *claveTempo;

  // Posibles casos de la opcion 
  if (strcmp(opcionDeTempo, "1") == 0)
    claveTempo = "lenta";
  else if (strcmp(opcionDeTempo, "2") == 0)
    claveTempo = "moderada";
  else if (strcmp(opcionDeTempo, "3") == 0)
    claveTempo = "rapida";
  else {
    printf("Opcion invalida. intenta nuevamente :/\n");
    return;
  }

  MapPair *par = map_search(mapTempo, claveTempo);
  if (par == NULL || list_first(par->value) == NULL) {
    printf("No hay canciones con tempo %s.\n", claveTempo);
    return;
  }

  printf("Canciones con tempo %s:\n", claveTempo);
  List *lista = par->value;

  for (Cancion *c = list_first(lista); c != NULL; c = list_next(lista)) {
    printf("| Titulo: %s | Genero: %s | Album: %s | Tempo: %.2f\n",
           c->titulo, c->genero, c->album, c->tempo);
  }

  printf("\nYa tienes todas las canciones con ese tempo :)\n\n");
}

// Funcion que libera la lista de canciones
void liberar_lista_canciones(List *lista) {
  if (!lista) return;
  Cancion *c = list_first(lista);
  while (c) {
    list_clean(c->artistas); // libera la lista de artistas
    free(c);                 // libera la canción
    c = list_next(lista);
  }
  list_clean(lista); // libera nodos de la lista
  free(lista);
}

// Funcion que libera memoria del mapa
void liberar_map(Map *map) {
  if (!map) return;
  MapPair *par = map_first(map);
  while (par) {
    free(par->key);                       // liberar clave
    liberar_lista_canciones(par->value); // liberar lista de canciones
    par = map_next(map);
  }
  map_clean(map); // limpia nodos del mapa
  free(map);      // libera el mapa
}

// Funcion que libera la memoria utilizada
void liberar(){
  liberar_map(mapaArtistas);
  liberar_map(mapTempo);
  liberar_map(mapaGeneros);
}

// Menu principal
void mostrarMenuPrincipal() {
  puts("========================================");
  puts("     ---> Bienvenido a Spotifind <---     ");
  puts("========================================");

  puts("1) Cargar Canciones");
  puts("2) Buscar por Genero");
  puts("3) Buscar por Artista");
  puts("4) Buscar por Tempo");
  puts("5) Salir");
}

// Funcion principal
int main() {
  char validar[101];
  char opcion;

  do {
    mostrarMenuPrincipal(); // Mostrar el menu principal
    printf("\nIngrese su opcion: ");
    scanf(" %s", validar);  // Leer toda la entrada como cadena

    // Validar que solo tenga 1 caracter y que este entre '1' y '5'
    if (strlen(validar) != 1 || validar[0] < '1' || validar[0] > '5') {
      printf("Opcion no valida :( ---> intente de nuevo...\n\n");
      continue;
    }

    opcion = validar[0];

    switch (opcion) {
      case '1':
        leer_canciones();
        break;
      case '2':
        buscar_por_genero();
        break;
      case '3':
        buscar_por_artista();
        break;
      case '4':
        buscar_por_tempo();
        break;
      case '5':
        printf("\nGracias por usar Spotifind (^-^)\n");
        printf("Vuelve pronto!\n\n");
        liberar();
        break;
    }

  } while (opcion != '5');

  return 0;
}