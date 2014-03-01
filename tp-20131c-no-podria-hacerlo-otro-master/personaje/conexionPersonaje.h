/*
 * conexionPersonaje.h
 *
 *  Created on: 06/05/2013
 *      Author: utnso
 */

#ifndef CONEXIONPERSONAJE_H_
#define CONEXIONPERSONAJE_H_

#include <unistd.h>

int conectar(char *ip, int nroPuerto);

void hacer_Handshake(int socket);

#endif /* CONEXIONPERSONAJE_H_ */
