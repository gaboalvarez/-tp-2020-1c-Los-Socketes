#include "teamInit.h"

/* pthread_mutex_t sumar;
 * pthread_mutex_init(&sumar,NULL);
 * pthread_mutex_lock(&sumar);
 * pthread_mutex_unlock(&sumar);
 */

void iniciar_entrenador(Entrenador** entrenador){
	while(1){
		printf("\nhola soy %d y me trabe",(*entrenador)->idEntrenador);
		sem_wait(&((*entrenador)->activador));
		printf("\nhola soy %d y me destrabe",(*entrenador)->idEntrenador);
	}
	pthread_exit(NULL);
}

void cargarConfig(Config* conexionConfig, t_config* config){


	conexionConfig->posiciones = config_get_array_value(config,"POSICIONES_ENTRENADORES"); // lista de strings, ultimo elemento nulo
	conexionConfig->pertenecientes = config_get_array_value(config,"POKEMON_ENTRENADORES");
	conexionConfig->objetivos = config_get_array_value(config,"OBJETIVOS_ENTRENADORES");
	conexionConfig->ip = config_get_string_value(config, "IP_TEAM");
	conexionConfig->puerto = config_get_string_value(config, "PUERTO_TEAM");
}
Pokemon* crearPokemon(char *nombre,int x, int y) {
	Pokemon *new = malloc(sizeof(Pokemon));
    new->nombre = strdup(nombre);
    new->y=y;
    new->x=x;
    return new;
}

PokemonFantasia* crearObjetivo(char *nombre) {
	PokemonFantasia *new = malloc(sizeof(PokemonFantasia));
    new->nombre = strdup(nombre);
    return new;
}


int cant_entrenadores(char** posiciones){
	int cantidad = 0;
	int i=0;
	while(posiciones[i] != NULL){
		cantidad++;
		i++;
	}
	return cantidad;
}
void asignarPosicion(Entrenador* persona,char* posicion){
	char* token = strtok(posicion, "|");
	int eje=0;
	while (token != NULL) {
		persona->posicion[eje] = atoi(token);
	    token = strtok(NULL, "|");
	    eje++;
	}
}
void asignarPertenecientes(Entrenador* persona,char* pokemons){
	char* token = strtok(pokemons, "|");
	persona->mios = list_create();
	while (token != NULL) {
		Pokemon* p;
		p = crearPokemon(token,0,0);
		list_add(persona->mios,p);
		token = strtok(NULL, "|");
	}
}
void asignarObjetivos(Entrenador* persona,char* pokemons){
	char* token = strtok(pokemons, "|");
		persona->objetivos = list_create();
		while (token != NULL) {
			PokemonFantasia* p;
			p = crearObjetivo(token);
			list_add(persona->objetivos,p);
			token = strtok(NULL, "|");
		}
}
void mostrarObjetivosActualesDe(Entrenador* entrenador){
	t_list* listaObjetivos = entrenador->objetivosActuales;
	printf("\nPokemones por encontrar: ");
	for(int i = 0 ; i<list_size(listaObjetivos);i++){
		PokemonFantasia* pokemon = list_get(listaObjetivos,i);
		printf("\n %s",pokemon->nombre);
	}
	printf("\n");
}
void mostrarEstado(Entrenador* entrenador){
	switch(entrenador->estado){
		case NUEVO:
			printf("NUEVO");
		break;
		case LISTO:
			printf("LISTO");
		break;
		case EJECUTANDO:
			printf("EJECUTANDO");
		break;
		case BLOQUEADO:
			printf("BLOQUEADO");
		break;
		case SALIR:
			printf("SALIR");
		break;
		default:
			printf("DEADLOCK");
	}
}

void mostrarEntrenador(Entrenador* entrenador){
	printf("\nEntrenador(%d)",entrenador->idEntrenador);
	printf(" posicion x=%d",entrenador->posicion[0]);
	printf(" posicion y=%d",entrenador->posicion[1]);
	printf(" estado= ");
	mostrarEstado(entrenador);
	printf("\n");
	PokemonFantasia* pokemon;
	for(int i=0;i<list_size(entrenador->mios); i++ ){

		pokemon =list_get(entrenador->mios,i);
		printf(" pokemon[%d]=",i);
		printf("%s",pokemon->nombre );
	}

	mostrarObjetivosActualesDe(entrenador);
}

char* retornarNombrePosta(Pokemon* p){
	return p->nombre;
}
char* retornarNombreFantasia(PokemonFantasia* p){
	return p->nombre;
}


Entrenador* inicializarEntrenador(int id,char* posicion, char* pokePertenecientes , char* pokeObjetivos){
	Entrenador* entrenador = (Entrenador*)malloc(sizeof(Entrenador));
	entrenador->idEntrenador = id;
	asignarPosicion(entrenador,posicion);
	asignarPertenecientes(entrenador,pokePertenecientes);
	asignarObjetivos(entrenador,pokeObjetivos);
	entrenador->estado = NUEVO;
	asignarObjetivosActuales(entrenador);
	sem_init(&(entrenador->activador),0,0);
	entrenador->movimiento[0]=0;entrenador->movimiento[1]=0;

	mostrarEntrenador(entrenador);
	return entrenador;

}
int getCantEntrenadores(Team team){
	return sizeof(team)-1;
}
void getObjetivosGlobales(Team team){
	//printf("\n ------------------- \n");
	for(int i = 0 ;i<getCantEntrenadores(team);i++){
		for(int j = 0 ; j<list_size(team[i]->objetivosActuales);j++){
			list_add(OBJETIVO_GLOBAL,list_get(team[i]->objetivosActuales,j));
		}
	}
}


Team inicializarTeam(char** posiciones, char** pokePertenecientes , char** pokeObjetivos){
	Entrenador** team = (Entrenador**)(malloc(sizeof(Entrenador)));
	OBJETIVO_GLOBAL = list_create();
	int cant = sizeof(posiciones) - 1;
	pthread_t hilo[cant];
	for(int i=0 ; i<cant ; i++){
		team[i] = inicializarEntrenador(i,posiciones[i],pokePertenecientes[i],pokeObjetivos[i]);
		pthread_create(&hilo[i],NULL,(void*)iniciar_entrenador,&(team[i]));
		pthread_detach(hilo[i]);
	}
	getObjetivosGlobales(team);

	return team;
}

void asignarObjetivosActuales(Entrenador* persona){
	persona->objetivosActuales = list_create();
	t_list* nombresObjetivos = list_map(persona->objetivos,(void*) retornarNombreFantasia);
	t_list* nombresPertenecientes = list_map(persona->mios,(void*) retornarNombrePosta);
	int indiceObjetivos=0;
	int indicePertenecientes;
	int repetido;
	int noRepetido;
	while(indiceObjetivos<list_size(nombresObjetivos)){
		indicePertenecientes=0;
		repetido=1;
		noRepetido=0;
		while((indicePertenecientes<list_size(nombresPertenecientes))&&(repetido)){
			//printf("comparo objetivo nro %d: %s con perteneciente nro %d: %s \n",indiceObjetivos,list_get(nombresObjetivos,indiceObjetivos),indicePertenecientes,list_get(nombresPertenecientes,indicePertenecientes));
			if(!strcmp(list_get(nombresObjetivos,indiceObjetivos),list_get(nombresPertenecientes,indicePertenecientes))){
				repetido=0;
				list_remove(nombresObjetivos,indiceObjetivos);
				list_remove(nombresPertenecientes,indicePertenecientes);
				indiceObjetivos--;
				//printf("saco 2 elementos \n");
			}else{
				noRepetido++;
			}
			indicePertenecientes++;
		}

		indiceObjetivos++;
	}
	if(list_size(nombresObjetivos)>0){
		for(int i=0;i<list_size(nombresObjetivos);i++){
			PokemonFantasia* p;
			p = crearObjetivo(list_get(nombresObjetivos,i));
			list_add(persona->objetivosActuales,p);
		}
	}
}
Entrenador* getEntrenador(int id, Team team){
	return team[id];
}

void liberarTeam(Team team){
	for(int i=0 ; i< getCantEntrenadores(team);i++){
		free(team[i]);
	}
	free(team);
}
void liberarMemoria(Entrenador **team){
	liberarTeam(team);
}
