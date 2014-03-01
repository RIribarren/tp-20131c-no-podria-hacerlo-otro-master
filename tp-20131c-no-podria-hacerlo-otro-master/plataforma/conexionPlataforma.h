/*
 * conexionPlataforma.h
 *
 *  Created on: 11/05/2013
 *      Author: utnso
 */

#ifndef CONEXIONPLATAFORMA_H_
#define CONEXIONPLATAFORMA_H_

#include "bibliotecas.h"

int aceptar_cliente(int socket_servidor, int longitud_cliente);

int abrir_conexion_servidor(int nroPuerto, int backlog);

#endif /* CONEXIONPLATAFORMA_H_ */
