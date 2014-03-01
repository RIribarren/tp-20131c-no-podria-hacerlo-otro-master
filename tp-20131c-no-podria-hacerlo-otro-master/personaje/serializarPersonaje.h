/*
 * serializar.h
 *
 *  Created on: 08/05/2013
 *      Author: utnso
 */

#ifndef SERIALIZAR_H_
#define SERIALIZAR_H_


#include <stdint.h>
#include <stdlib.h>
#include "defines.h"

typedef struct {
	int8_t id;
	int16_t size_data;
	char *data;
} __attribute__ ((packed)) t_paquete;

#define SIZE_CABECERA (sizeof(int8_t) + sizeof(int16_t)) //la cabecera se compone de un id y del tamaño del mensaje que se va a enviar


typedef struct{
	char* ip_Plan;
	int32_t puerto_Plan;
	char* ip_Nivel;
	int32_t puerto_Nivel;
} __attribute__ ((packed)) t_info_nivel_plan;

typedef struct{
	int8_t x;
	int8_t y;
} __attribute__ ((packed)) t_destino;

typedef struct{
	int8_t posX;
	int8_t posY;
	char* recurso;
} __attribute__ ((packed)) t_datosParaPedirInstancia;

t_paquete *crear_Paquete(int8_t id, char *data, int16_t size_data);

int enviar_Paquete(int socket,t_paquete *paquete);

t_paquete *recibir_Paquete(int socket);

char *serializar_Paquete(t_paquete *paquete,size_t *longitud_Stream);

t_info_nivel_plan *deserializar_Info(char *data, size_t longitud_Info);

t_paquete *deserializar_Cabecera(char* data);

char *serializar_Destino(t_destino *destino, size_t longitud_destino);

t_destino *deserializar_Destino(char *data, size_t longitud_Destino);

char *serializar_Datos_Pedir_Instancia(t_datosParaPedirInstancia *datosParaPedirInstancia,size_t longitud_datos);

#endif /* SERIALIZAR_H_ */
