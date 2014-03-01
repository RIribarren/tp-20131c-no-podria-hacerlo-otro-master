/*
 * serializarPersonaje.c
 *
 *  Created on: 10/05/2013
 *      Author: utnso
 */

#include "serializarPersonaje.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <commons/log.h>
#include "iniciarPersonaje.h"

static int nuevo_Send(int socket, char *stream, size_t longitud_Stream, int flag);

static int nuevo_Recv(int socket, char *stream, size_t longitud_Stream, int flag);


t_paquete *crear_Paquete(int8_t id, char *data, int16_t size_data)
{
	t_paquete *paquete = malloc(sizeof(t_paquete));

	paquete->id = id;
	paquete->size_data = size_data;
	paquete->data = malloc(size_data);


	//TODO verificar si el size_data > 0??

	memcpy(paquete->data,data,size_data);

	return paquete;
}

int enviar_Paquete(int socket, t_paquete *paquete)
{
	size_t longitud_Stream = 0;
	char *stream = NULL;
	int nroBytesEnviados = 0;

	stream = serializar_Paquete(paquete, &longitud_Stream); //Aca sale retornado el paquete serializado
															//Y LA LONGITUD DEL MISMO (longitud_stream)

	//nuevo_Send(int32_t socket, t_paquete *paquete, int32_t longitud_Stream, int flag);
	nroBytesEnviados = nuevo_Send(socket, stream, longitud_Stream, 0);

	if(nroBytesEnviados < 0){
		printf("Fallo en el send del paquete\n");
		exit(1);
	}
	free(stream);

	return 0;
}

char *serializar_Paquete(t_paquete *paquete, size_t *longitud_Stream)
{
	char *stream = NULL;
	int offset = 0, tmp_size = 0;
	*longitud_Stream = 0;

	*longitud_Stream = (SIZE_CABECERA + (paquete->size_data) * sizeof(char)); //es la suma entre el tamaño de la
																			//cabecera + el cuerpo (de tamaño variable)

	stream = malloc(*longitud_Stream);

	memset(stream, '0', *longitud_Stream);

	//pasando el id
	tmp_size = sizeof(uint8_t);
	memcpy(stream, &paquete->id, tmp_size);

	offset += tmp_size;

	//pasando el tamaño
	tmp_size = sizeof(uint16_t);
	memcpy(stream + offset, &paquete->size_data, tmp_size);

	offset += tmp_size;

	//pasando la data del cuerpo
	tmp_size = paquete->size_data;
	memcpy(stream + offset, paquete->data, tmp_size); // TODO mandar el puntero?

	offset += tmp_size;

	*longitud_Stream = offset; //aca sale la variable modificada!. Tiene el tamaño completo de tod0 el stream serializado

	return stream;
}


t_paquete *deserializar_Cabecera(char* data)
{
	t_paquete *paquete = malloc(sizeof(t_paquete));
	int offset = 0, tmp_size = 0;

	 memset(paquete, '0', sizeof(t_paquete));

	//sacando el id
	tmp_size = sizeof(uint8_t);
	memcpy(&paquete->id,data, tmp_size);

	offset += tmp_size;

	//sacando el lenght
	tmp_size = sizeof(uint16_t);
	memcpy(&paquete->size_data,data+offset, tmp_size);

	return paquete;
}

void deserializar_Cuerpo(t_paquete *paquete, char *stream)
{
	int size_data = paquete->size_data;

	paquete->data = malloc(size_data);

	memcpy(paquete->data, stream, paquete->size_data);
}



static int nuevo_Send(int socket, char *stream, size_t longitud_Stream, int flag)
{
	int nroBytesEnviados = 0;
	int offset = 0;

	size_t tmp_size_stream = longitud_Stream;

	while(offset < tmp_size_stream)
	{
		nroBytesEnviados = send(socket, stream + offset, tmp_size_stream, flag | MSG_NOSIGNAL);

		if(nroBytesEnviados < 0){
			perror("Error en enviar el stream\n");
			if(errno == EINTR)
				continue;
			else
				exit(1);
			}
		else if(nroBytesEnviados == 0){
			perror("Error en enviar el stream, se corto la conexion\n");
			exit(1);
		}else
		{

		offset += nroBytesEnviados;
		tmp_size_stream -= nroBytesEnviados;
	}
	}


	return nroBytesEnviados;

}

t_paquete *recibir_Paquete(int socket){
	char *stream = malloc(SIZE_CABECERA);
	uint32_t nroBytesRecibidos = 0;

	memset(stream, '0', SIZE_CABECERA);


	//Recibiendo la cabecera
	nroBytesRecibidos = nuevo_Recv(socket, stream, SIZE_CABECERA, 0);

	if(nroBytesRecibidos <= 0){
		perror("Error en el recv de la cabecera");
		free(stream);
		return NULL;
	}
	else //recibi toda la cabecera, ahora la deserializo
	{
		t_paquete *paquete = deserializar_Cabecera(stream);
		free(stream);

		//ahora recibo el cuerpo

		stream = malloc(paquete->size_data);

		nroBytesRecibidos = nuevo_Recv(socket, stream, paquete->size_data, 0);


		if(nroBytesRecibidos <= 0){
			perror("Problema en el recv del cuerpo");
			free(stream);
			free(paquete);
			return NULL;
		}
		else
		{	//recibi tod0 el cuerpo, lo deserializo

		deserializar_Cuerpo(paquete, stream);
		free(stream);

		return paquete;

		}

	}
}

static int nuevo_Recv(int socket, char *stream, size_t longitud_Stream, int flag)
{
	int nroBytesRecibidos = 0;
	int offset = 0;
	size_t tmp_longitud_Stream = longitud_Stream;



	while( offset < tmp_longitud_Stream ) {
		nroBytesRecibidos = recv(socket, stream + offset, tmp_longitud_Stream, flag | MSG_NOSIGNAL);
		if ( nroBytesRecibidos	<= 0) {
			if(errno == EINTR)
				continue;
			else{
				return nroBytesRecibidos;
			perror("Error en el recv\n");
			exit(1);}
		}
		else{

			offset += nroBytesRecibidos;
			tmp_longitud_Stream -= nroBytesRecibidos;
		}
	}


	return nroBytesRecibidos;
}


t_info_nivel_plan *deserializar_Info(char* data, size_t longitud_info)
{
        t_info_nivel_plan *info = malloc(longitud_info);
        int offset = 0, tmp_size = 0;

        //TODO hacer el memset(paquete, '0', sizeof(t_paquete))??

    //saco la ip del planif.
        //averiguo la cantidad de bytes que tiene el string ip_plan
        for(tmp_size = 1; (data)[tmp_size - 1] != '\0'; tmp_size ++);

        info->ip_Plan = malloc(tmp_size);

        memcpy(info->ip_Plan,data, tmp_size);

        offset += tmp_size;

    //saco el puerto del planif.
        tmp_size = sizeof(int32_t);
        memcpy(&info->puerto_Plan,data+offset, tmp_size);

        offset += tmp_size;

    //saco la ip del nivel
        //averiguo la cantidad de bytes que tiene el string ip_nivel
        for(tmp_size = 1; (data + offset)[tmp_size - 1] != '\0'; tmp_size ++);

        info->ip_Nivel = malloc(tmp_size);

        memcpy(info->ip_Nivel,data+offset, tmp_size);

        offset += tmp_size;

    //saco el puerto del nivel
        tmp_size = sizeof(int32_t);
        memcpy(&info->puerto_Nivel,data+offset, tmp_size);

        return info;
}

char *serializar_Destino(t_destino *destino, size_t longitud_destino)
{
	char *stream = NULL;
	int offset = 0, tmp_size = 0;

	stream = malloc(longitud_destino);

	//pasamos al stream el posX
	tmp_size = sizeof(int8_t);
	memcpy(stream, &destino->x, tmp_size);

	offset += tmp_size;

	//pasamos el posY

	tmp_size = sizeof(int8_t);
	memcpy(stream + offset, &destino->y, tmp_size);

	return stream;
}

t_destino *deserializar_Destino(char *data, size_t longitud_Destino)
{
	int offset = 0, tmp_size = 0;
	t_destino *destino = malloc(longitud_Destino);

	//paso la variable X
	tmp_size = sizeof(int8_t);
	memcpy(&destino->x, data, tmp_size);

	offset += tmp_size;

	//paso la variable Y
	tmp_size = sizeof(int8_t);
	memcpy(&destino->y, data + offset, tmp_size);

	return destino;
}

char *serializar_Datos_Pedir_Instancia(t_datosParaPedirInstancia *datosParaPedirInstancia, size_t longitud_datos)
{
	char *stream = malloc(longitud_datos);
	int offset = 0, tmp_size = 0;

	//paso la variable x
	tmp_size = sizeof(int8_t);
	memcpy(stream, &datosParaPedirInstancia->posX, tmp_size);

	offset += tmp_size;

	//paso la variable y
	tmp_size = sizeof(int8_t);
	memcpy(stream + offset, &datosParaPedirInstancia->posY, tmp_size);

	offset += tmp_size;

	//paso el recurso
	tmp_size = strlen(datosParaPedirInstancia->recurso) + 1;
	memcpy(stream + offset, datosParaPedirInstancia->recurso, tmp_size);

	return stream;
}
