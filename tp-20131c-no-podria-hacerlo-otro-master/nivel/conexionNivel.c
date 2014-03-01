/*
 * conexionNivel.c
 *
 *  Created on: 18/05/2013
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
#include <sys/ioctl.h>
#include "serializarNivel.h"
#include "conexionNivel.h"
#include "defines.h"

int conectar(char *ip, int nroPuerto)
{
        int socketNivel;
        struct sockaddr_in infoSocketOrquestador;

        infoSocketOrquestador.sin_family = AF_INET;
        inet_pton(AF_INET, ip, &(infoSocketOrquestador.sin_addr)); //"convierte" la cadena ip en una struct para que pueda conecatarse
        infoSocketOrquestador.sin_port = htons(nroPuerto); //convierte el int del puerto a binario
        memset(&(infoSocketOrquestador.sin_zero), '\0', 8);


        //Se crea el socket

        socketNivel = socket(AF_INET,SOCK_STREAM,0);

        if (socketNivel == -1)
                         {
                                 perror ("Error al crear el socket\n");
                                 exit(1);
                         }


        if (connect(socketNivel, (struct sockaddr *)&infoSocketOrquestador,sizeof(infoSocketOrquestador)) == -1)
                                {
                                        perror("Error al conectar al servidor\n");
                                        exit(1);
                                }


        return socketNivel;
}


void hacer_Handshake(int socket)
{
                    //TODO   printf("Haciendo el Handshake...\n");
        t_paquete *paquete = crear_Paquete(HANDSHAKE, "Soy un nivel", strlen("Soy un nivel") + 1 );

                                        enviar_Paquete(socket, paquete);

                                        free(paquete->data);
                                        free(paquete);
        }

int abrir_conexion_servidor(int nroPuerto, int backlog)
{
        int socket_servidor;
        struct sockaddr_in infoSocketServidor;
        int optval = 1;
        int actividad;

        //Abriendo el socket

        socket_servidor = socket(AF_INET, SOCK_STREAM, 0);

                                if (socket_servidor == -1) //Verificacion de error
                                {
                                                perror("Error al crear el socket\n");
                                                exit(1);
                                }

        setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

        actividad = ioctl(socket_servidor, FIONBIO, (char *)&optval);
          if (actividad < 0)
          {
            perror("ioctl() failed");
            close(socket_servidor);
            exit(-1);
          }


        infoSocketServidor.sin_family = AF_INET;
        infoSocketServidor.sin_port = htons(nroPuerto);
        infoSocketServidor.sin_addr.s_addr = INADDR_ANY;
        memset(&(infoSocketServidor.sin_zero), '\0', 8);


        //Bind
        if (bind(socket_servidor, (struct sockaddr *) &infoSocketServidor, sizeof(struct sockaddr)) == -1)
        {
                perror("Error al hacer el bind\n");
                exit(1);
        }


        //Listen de clientes

        if (listen (socket_servidor, backlog) == -1) //backlog = cantidad maxima de clientes
                                        {
                                                perror("Error al hacer el listen\n");
                                                exit(1);
                                        }
        else
             //TODO   printf("Esperando por clientes \n");

        return socket_servidor;
}

int aceptar_cliente(int socket_servidor, int longitud_cliente)
{

	struct sockaddr_in infoSocketCliente;
	int32_t socket_cliente;


	socket_cliente = accept(socket_servidor, (struct sockaddr *) &infoSocketCliente,(socklen_t *) &longitud_cliente);
							/*if (socket_cliente == -1)
							{
								perror("Error al aceptar el cliente\n");
								exit(1);

							}*/



	return socket_cliente;
}
