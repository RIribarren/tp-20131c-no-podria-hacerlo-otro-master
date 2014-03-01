/*
 * mensajesPersonaje.h
 *
 *  Created on: 06/05/2013
 *      Author: utnso
 */

#ifndef MENSAJESPERSONAJE_H_
#define MENSAJESPERSONAJE_H_

#include "iniciarPersonaje.h"
#include "serializarPersonaje.h"


t_info_nivel_plan *pedir_Info_NivPla(int socketOrquestador, char* nivel);

t_destino *realizar_Movimiento(int socketPlanificador, t_destino *posActual, t_destino *destino);

t_destino *pedir_Destino(int socketNivel, char* recurso);

int verificar_Misma_Posicion(t_destino *posActual, t_destino *destino);

int pedir_Instancia(int socketNivel, char* recurso, t_destino *posActual);

//void avisar_Bloqueo(int socketPlanificador);

void estoy_Listo(int socketPlanificador);

void turno_Terminado(int socketPlanificador, char* estado);

void enviar_Recurso(int socketPlanificador, char* recurso);

void enviar_Simbolo(int socketNivel,char* simbolo);

void enviar_Nueva_Posicion(int socketNivel, t_destino *posActual);

void avisar_Desconexion(int socketr);

void avisar_Juego_Terminado(int socketPlanificador);

void enviar_Espero_Respuesta(int socketOrquestador, char* simboloPersonaje);

void avisar_Desconexion_Interbloqueo(int socketNivel);

#endif /* MENSAJESPERSONAJE_H_ */
