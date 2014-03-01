/*
 * orquestador.h
 *
 *  Created on: 17/05/2013
 *      Author: utnso
 */

#ifndef ORQUESTADOR_H_
#define ORQUESTADOR_H_

#include "bibliotecas.h"
#include "colasPlataforma.h"
#include "conexionPlataforma.h"
#include "definesPlataforma.h"
#include "serializarPlataforma.h"
#include "plataforma.h"

#define BACKLOG 10

char* ipOrquestador;
int quantum;
int tiempoDeEspera;
t_list* listaDeListas;
t_list* listaDeNiveles;
t_list* pjsOnline;
int primeraVez;
t_config *config;
pthread_mutex_t mutex1;
pthread_mutex_t mutex2;


t_list *listaPersonajesOrquestador;

typedef struct{
	int socketPersonaje;
	char* simboloPersonaje;
}t_personajeOrquestador;

typedef struct {
   char* nombreNivel;
    t_list *listaListos;
    t_list *listaBloqueados;
    t_list *listaAnormales;
    t_list *listaDesconexiones;
}t_listas;

typedef struct {
	char* nombre_Nivel;
	char* ip_Nivel;
	int32_t puerto_Nivel;
	char* ip_Planif;
	int32_t puerto_Planif;
	t_list *listaDePjsJugando;
} t_niveles;

typedef struct {
	char* nombreNivel;
	int puerto;
}t_datos_para_planificador;

typedef struct {
	char* simboloPj;
	int socketPj;
	int bloqueado;
} __attribute__ ((packed)) t_datos_pj;

typedef struct {
	t_list *listaRecursosAsignados;
	t_list *listaRecursosLibres;
}t_lista_recursos;

typedef struct {
	char* simboloPj;
	char* recursoAsignado;
}t_recursos_asignados;

typedef struct {
	char* recursoLibre;
}t_recursos_libres;


void *funcionOrquestador (void *ptr);
void *funcionPlanificador(char* nombreNivel);

#endif /* ORQUESTADOR_H_ */
