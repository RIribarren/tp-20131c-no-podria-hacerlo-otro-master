/*
 * mensajesPersonaje.c
 *
 *  Created on: 10/05/2013
 *      Author: utnso
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "mensajesPersonaje.h"
#include "defines.h"




t_info_nivel_plan *pedir_Info_NivPla(int socketOrquestador, char* nivel)
{

	//no hace falta serializar aca porque nivel es de tipo char*
	t_paquete *paquete = crear_Paquete(PEDIR_INFO_NIVEL_PLAN, nivel, strlen(nivel) + 1);

	enviar_Paquete(socketOrquestador, paquete);

	destroy_paquete(paquete);

	//recibo la info
	paquete = recibir_Paquete(socketOrquestador);

	t_info_nivel_plan *info = deserializar_Info(paquete->data, paquete->size_data);

	destroy_paquete(paquete);

	return info;
}

t_destino *realizar_Movimiento(int socketPlanificador, t_destino *posActual, t_destino *destino)
{

	int seMovio = NO;

	//espera la confirmacion
	t_paquete *paquete = recibir_Paquete(socketPlanificador);

	if(paquete->id == MOVIMIENTO_PERMITIDO)
	{
		//se tiene que mover el personaje

		//caso en el que el personaje esta en una pos menor a la X y menor a la Y
		if(seMovio == NO)
		{
			if(posActual->y 		< 		destino->y)
			{
				//se mueve en la columna
				posActual->y++;
				seMovio = YES;
			}
			else
			{	if(posActual->x 		< 		destino->x)
					{

			//una vez terminado de moverse por la columna, se mueve en fila
						posActual->x++;
						seMovio = YES;
					}
			}

		}


		//caso en el que el pj esta en un lugar menor a las X, y mayor a las Y

		if(seMovio == NO)
		{
			if(posActual->y 	> 	destino->y){

				posActual->y--;
				seMovio = YES;
			}
			else
			{	if((posActual->x	<	destino->x))
			{
				posActual->x++;
				seMovio = YES;
			}
			}
		}

		//caso en el que el pj esta en un lugar mayor a las X y menor a las Y


			if(seMovio == NO)
			{

				if(posActual->y		<		destino->y){

					posActual->y++;
					seMovio = YES;
				}

				else
				{ if(posActual->x 	>	destino->x)
				{

					posActual->x--;
					seMovio = YES;
				}
				}

			}

		//caso en el que tanto la pos x como la pos y estan en un lugar mayor al del destino
		if(seMovio == NO)
		{

			if(posActual->y		>		destino->y){

				posActual->y--;
				seMovio = YES;
			}

			else
			{ if(posActual->x		>		destino->x)
			{

				posActual->x--;
				seMovio = YES;
			}
			}

		}

	}

	destroy_paquete(paquete);

	return posActual;
}

t_destino *pedir_Destino(int socketNivel, char* recurso)
{
	int nroBytesEnviados;
	size_t longitud_Destino;

	//no serializo porque es un char*

	t_paquete* paquete = crear_Paquete(DESTINO, recurso, strlen(recurso) + 1);

	nroBytesEnviados = enviar_Paquete(socketNivel,paquete);


	if(nroBytesEnviados < 0)
	{
		perror("Error al enviar el recurso al Nivel\n");
		exit(1);
	}

	destroy_paquete(paquete);
	//espera el paquete con el struct del destino

	paquete = recibir_Paquete(socketNivel);

	t_destino *destino = malloc(sizeof(t_destino));

	if(paquete == NULL)
	{
			log_error(logger, "El nivel se cerro");
			exit(1);
	}
	else if(paquete->id == DESTINO)
	{
		longitud_Destino = paquete->size_data;


		destino = deserializar_Destino(paquete->data, longitud_Destino);
		destroy_paquete(paquete);
	}




	return destino;
}

int verificar_Misma_Posicion(t_destino *posActual, t_destino *destino)
{
	if(posActual->x == destino->x && posActual->y == destino->y)
		return YES;
	else
		return NO;
}

int pedir_Instancia(int socketNivel, char *recurso, t_destino *posActual)
{
	int nroBytesEnviados = 0;
	t_datosParaPedirInstancia *datosParaPedirInstancia = malloc(sizeof(t_datosParaPedirInstancia));

	datosParaPedirInstancia->posX = posActual->x;
	datosParaPedirInstancia->posY = posActual->y;

	datosParaPedirInstancia->recurso = malloc(strlen(recurso) + 1);
	strcpy(datosParaPedirInstancia->recurso,recurso);

	size_t longitud_datos = sizeof(int8_t) * 2 + strlen(recurso) + 1; //porque tengo una variable char*

	char* datos_Serializados = serializar_Datos_Pedir_Instancia(datosParaPedirInstancia, longitud_datos);

	t_paquete* paquete = crear_Paquete(PEDIR_INSTANCIA, datos_Serializados, longitud_datos);

	nroBytesEnviados = enviar_Paquete(socketNivel, paquete);

	destroy_paquete(paquete);

	if(nroBytesEnviados < 0)
	{
		perror("Error al enviar el pedido de instancia del recurso al Nivel\n");
		exit(-1);
	}

	//esperando respuesta de tipo char*, o sea YES o NO.

	paquete = recibir_Paquete(socketNivel);

	if(paquete == NULL)
	{
		log_error(logger, "Se cerro el nivel");
		exit(1);
	}


	if(paquete->id == PEDIR_INSTANCIA)
	{
		if(strcmp(paquete->data, "YES") == 0)
		{
			destroy_paquete(paquete);
			return YES;

		}
		else
		{
			destroy_paquete(paquete);
			return NO;

		}
	}
	else
	{
		log_error(logger, "Se cerro el nivel");
		exit(1);
	}

}



void turno_Terminado(int socketPlanificador, char* estado)
{
	int nroBytesEnviados = 0;

	//no hace falta serializar, porque es un char*

	t_paquete* paquete = crear_Paquete(TURNO_CONCLUIDO, estado, strlen(estado) + 1);

	nroBytesEnviados = enviar_Paquete(socketPlanificador, paquete);

	destroy_paquete(paquete);

	if(nroBytesEnviados < 0)
	{
		perror("Error al enviar el mensaje del estado, al planificador\n");
		exit(1);
	}
}

void enviar_Recurso(int socketPlanificador, char* recurso)
{

	int nroBytesEnviados = 0;

	//no hace falta serializar, porque recurso es un char*

	t_paquete *paquete = crear_Paquete(DAR_RECURSO, recurso, strlen(recurso) + 1);

	nroBytesEnviados = enviar_Paquete(socketPlanificador, paquete);

	destroy_paquete(paquete);

	if(nroBytesEnviados < 0)
	{
		perror("Error al enviar el recurso al planificador");
		exit(1);
	}
}

void estoy_Listo(int socketPlanificador)
{

	int nroBytesEnviados = 0;

	//no hace falta serializar, es un char*
	t_paquete *paquete = crear_Paquete(ME_QUIERO_MOVER, "Me quiero mover", strlen("Me quiero mover") + 1);

	nroBytesEnviados = enviar_Paquete(socketPlanificador, paquete);

	if(nroBytesEnviados < 0)
	{
		perror("Error al enviar el Estoy Listo al planificador\n");
		exit(1);
	}

	destroy_paquete(paquete);
}

void avisar_Desconexion(int socketr)
{

	t_paquete *paquete = crear_Paquete(DESCONEXION, "Termine", strlen("Termine") + 1);

	int nroBytesEnviados = enviar_Paquete(socketr, paquete);

	if(nroBytesEnviados < 0)
	{
		perror("Error al enviar el aviso de desconexion al planificador \n");
		exit(1);
	}

	destroy_paquete(paquete);

}

void avisar_Desconexion_Interbloqueo(int socketNivel)
{
	t_paquete *paquete = crear_Paquete(DESCONEXION, "Interbloqueo", strlen("Interloqueo") + 1);

	int nroBytesEnviados = enviar_Paquete(socketNivel, paquete);

	if(nroBytesEnviados < 0)
	{
		perror("Error al enviar el aviso de desconexion al planificador \n");
		exit(1);
	}

		destroy_paquete(paquete);
}

void avisar_Juego_Terminado(int socketPlanificador)
{

	 t_paquete* paquete = crear_Paquete(DESCONEXION, "Juego terminado", strlen("Juego terminado") + 1);

	int nroBytesEnviados = enviar_Paquete(socketPlanificador, paquete);

	if(nroBytesEnviados < 0)
	{
		perror("Error al enviar el aviso de juego terminado al planificador\n");
		exit(1);
	}

	destroy_paquete(paquete);

}

void enviar_Simbolo(int socketNP, char* simbolo)
{
	t_paquete *paquete = crear_Paquete(SIMBOLO, simbolo, strlen(simbolo) + 1);

	int nroBytesEnviados = enviar_Paquete(socketNP, paquete);

	if(nroBytesEnviados < 0)
	{
		perror("Error al enviar el simbolo al nivel\n");
		exit(1);
	}

	destroy_paquete(paquete);
}

void enviar_Nueva_Posicion(int socketNivel, t_destino *posActual)
{

	int nroBytesEnviados = 0;
	t_destino *nuevaPos = malloc(sizeof(t_destino));

	nuevaPos->x = posActual->x;
	nuevaPos->y = posActual->y;


	size_t longitud_datos = sizeof(int8_t) * 2;

	char* datos_Serializados = serializar_Destino(nuevaPos, longitud_datos);

	t_paquete *paquete = crear_Paquete(NUEVA_POSICION, datos_Serializados, longitud_datos);

	nroBytesEnviados = enviar_Paquete(socketNivel, paquete);

	if(nroBytesEnviados < 0)
		{
			perror("Error al enviar la nueva posicion al nivel\n");
			exit(1);
		}

	destroy_paquete(paquete);
}

void enviar_Espero_Respuesta(int socketOrquestador, char* simboloPersonaje)
{
	t_paquete *paquete = malloc(sizeof(t_paquete));

	paquete = crear_Paquete(RESPUESTA_ORQUESTADOR, simboloPersonaje, strlen(simboloPersonaje) + 1);

	int nroBytesEnviados = enviar_Paquete(socketOrquestador, paquete);

	destroy_paquete(paquete);

	if(nroBytesEnviados < 0)
			{
				perror("Error al enviar \"Espero respuesta\" al orquestador\n");
				exit(1);
			}
}
