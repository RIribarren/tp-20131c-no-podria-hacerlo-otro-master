/*
 * conexionNivel.h
 *
 *  Created on: 18/05/2013
 *      Author: utnso
 */

#ifndef CONEXIONNIVEL_H_
#define CONEXIONNIVEL_H_


#include <unistd.h>
#include <poll.h>
#include <errno.h>

int conectar(char *ip, int nroPuerto);

void hacer_Handshake(int socket);

int abrir_conexion_servidor(int nroPuerto, int backlog);

int aceptar_cliente(int socket_servidor, int longitud_cliente);

#endif /* CONEXIONNIVEL_H_ */
