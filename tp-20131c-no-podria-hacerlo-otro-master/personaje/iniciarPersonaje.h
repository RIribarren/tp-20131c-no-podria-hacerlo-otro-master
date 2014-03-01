/*
 * personaje.h
 *
 *  Created on: 05/05/2013
 *      Author: utnso
 */

#ifndef PERSONAJE_H_
#define PERSONAJE_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "serializarPersonaje.h"
#include <commons/config.h>
#include <commons/log.h>


t_log* logger;


//Estructura del personaje
typedef struct{
	int32_t pj_vidas;
	char *pj_nombre;
	int32_t pj_cant_niveles;
	char *ip_Orquestador;
	int32_t puerto_orquestador;
	char *pj_simbolo;
	char **pj_planDeNiveles;
	int32_t *pj_cantidadDeRecursos;
	char *ip_local;
} __attribute__ ((packed)) t_personaje;


//se pasa como parametro la ip completa, devuelve el puerto
int32_t obtener_Puerto(char* direccion);

//se pasa como parametro la ip completa, devuelve la IP
char* obtener_IP(char* direccion);

t_personaje *iniciar_Personaje(t_config *config);

char* agregar_Nivel_Al_String(char* nivel);

char* agregar_Recurso_Al_String(char* recurso);

char **obtener_Recursos_Nivel(char *nivel, t_config *config);

void destroy_paquete(t_paquete* paquete);

void destroy_info(t_info_nivel_plan *info);

void destroy_personaje(t_personaje *personaje);

#endif /* PERSONAJE_H_ */
