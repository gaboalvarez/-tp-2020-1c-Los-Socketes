#ifndef CONEXIONBROKER_H_
#define CONEXIONBROKER_H_

#include <shared/structs.h>
#include <shared/messages.h>
#include <shared/serialize.h>

pthread_t thread;

void iniciar_servidor(t_config* config, int socketBroker);

#endif
