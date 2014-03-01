/*
 * interbloqueo.h
 *
 *  Created on: 12/06/2013
 *      Author: utnso
 */

#ifndef INTERBLOQUEO_H_
#define INTERBLOQUEO_H_

#include <pthread.h>
#include <commons/config.h>
#include <stdbool.h>
#include <commons/collections/list.h>
#include "iniciarNivel.h"
#include <signal.h>

#define YES 1
#define NO 0

t_list *listaPersonajesBloqueados;

int ejecutarInterbloqueo;

void *detectar_Interbloqueo(void *ptr);

t_personaje *buscar_Personaje_Ejecutable(t_list *listaDisponiblesTemporal,t_list *listaDeMarcados, t_list *listaPjsBloqueados);

int buscar_Marcado(t_list *listaDeMarcados,char* simboloPersonaje);

int verificar_Instancias(t_list *listaDisponiblesTemporal, char* recursoQueBusca);

void actualizar_Disponibles_Temporal(t_list* listaDisponiblesTemporal, t_list *listaDeRecursosGanados);

void marcar_Proceso(t_list *listaDeMarcados,char *simboloPersonaje);

void analizar_Interbloqueo(t_list *listaDeMarcados, t_list* listaPjsBloqueados);

t_list* filtrar_bloqueados(void);

#endif /* INTERBLOQUEO_H_ */
