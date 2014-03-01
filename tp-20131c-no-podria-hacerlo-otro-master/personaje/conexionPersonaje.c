/*
 * conexionPersonaje.c
 *
 *  Created on: 10/05/2013
 *      Author: utnso
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "serializarPersonaje.h"
#include "conexionPersonaje.h"


int conectar(char *ip, int nroPuerto)
{
	int socketPersonaje;
	struct sockaddr_in infoSocketOrquestador;

	infoSocketOrquestador.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &(infoSocketOrquestador.sin_addr)); //"convierte" la cadena ip en una struct para que pueda conecatarse
	infoSocketOrquestador.sin_port = htons(nroPuerto); //convierte el int del puerto a binario
	memset(&(infoSocketOrquestador.sin_zero), '\0', 8);


	//Se crea el socket

	socketPersonaje = socket(AF_INET,SOCK_STREAM,0);

	if (socketPersonaje == -1)
		 	 {
		 		 perror ("Error al crear el socket\n");
		 		 exit(1);
		 	 }


	if (connect(socketPersonaje, (struct sockaddr *)&infoSocketOrquestador,sizeof(infoSocketOrquestador)) == -1)
				{
					perror("Error al conectar al servidor\n");
					exit(1);
				}



	return socketPersonaje;
}

void hacer_Handshake(int socket)
{
	t_paquete *paquete = crear_Paquete(HANDSHAKE, "Soy un personaje", strlen("Soy un personaje") + 1 );

		 		enviar_Paquete(socket, paquete);

		 		destroy_paquete(paquete);
}
