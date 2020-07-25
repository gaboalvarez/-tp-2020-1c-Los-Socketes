#ifndef CACHE_BUDDYSYSTEM_H_
#define CACHE_BUDDYSYSTEM_H_

#include "genericCache.h"
#include "basicCache.h"

typedef struct Bloque {
	int posicion;
	int tamanio;
	int tamanioOcupado;
	int estaDividido;
	char* fechaCreacion;
	char* fechaUltimoUso;
	long ID;
	long IDCorrelativo;
	TipoCola cola;
	t_list* suscriptoresRecibidos;
	t_list* suscriptoresEnviados;
	struct Bloque* izq;
	struct Bloque* der;
}Bloque, *Cache;

Cache cache;

void inicializarBuddySystem();
void liberarBuddySystem();
void imprimirCache();

#endif
