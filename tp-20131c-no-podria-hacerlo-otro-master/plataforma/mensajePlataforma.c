/*
 * mensajePlataforma.c
 *
 *  Created on: 11/05/2013
 *      Author: utnso
 */

#include "mensajePlataforma.h"


int leer_socket(int socket, char *buffer, int longitud, int flag)
{
	int nroBytes;

	//Recv
	//recv(socket que voy a leer, paquete, tamaño, flag)
		//Seteo el buffer a vacio


		nroBytes = recv(socket, buffer, longitud, flag);

		if(nroBytes == -1){
			perror("Error al recibir el mensaje\n");
			exit(1);
		}
		else
			printf("Mensaje recibido: %s\n",buffer);

	return 0;

}

/*
* Escribe dato en el socket cliente. Devuelve numero de bytes escritos,
* o -1 si hay error.
*/
int escribir_socket(int socket, char *buffer, int longitud, int flag)
{
	int nroBytes;

		nroBytes = send(socket, buffer, longitud, flag);


			if(nroBytes == -1){
				perror("Error escribiendo al socket\n");
				exit(1);}


	return 0;
}

