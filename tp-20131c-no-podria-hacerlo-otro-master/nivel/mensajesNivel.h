/*
 * mensajesNivel.h
 *
 *  Created on: 22/05/2013
 *      Author: utnso
 */

#ifndef MENSAJESNIVEL_H_
#define MENSAJESNIVEL_H_

#include "serializarNivel.h"
#include "iniciarNivel.h"
#include "defines.h"

void enviar_Datos_Nivel(t_nivel *nivel, int socketOrquestador);

t_destino *buscar_Destino(char* recurso, t_list *listaDeCajas);

t_info_datos_nivel *obtener_Datos_Nivel(t_nivel *nivel);

void enviar_Destino(int socketPersonaje, t_destino* destino);

int verificar_Posicion(t_datosParaPedirInstancia *datos, t_list *listaDeCajas);

void enviar_Respuesta_Otorgado(int socketPersonaje, int respuesta);

#endif /* MENSAJESNIVEL_H_ */
