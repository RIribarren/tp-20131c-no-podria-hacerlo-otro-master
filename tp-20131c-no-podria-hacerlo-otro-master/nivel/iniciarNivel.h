/*
 * Nivel.h
 *
 *  Created on: 16/05/2013
 *      Author: utnso
 */

#ifndef INICIARNIVEL_H_
#define INICIARNIVEL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/log.h>
#include "tad_items.h"
#include <stdlib.h>
#include <curses.h>
#include <pthread.h>

ITEM_NIVEL* ListaItems;

t_log* logger;

t_list *listaDePersonajes;
t_list *listaDeRecursosDisponibles;

int temporizador;
int hayInterbloqueo;

typedef struct{
        int8_t x;
        int8_t y;
} __attribute__ ((packed)) t_destino;

typedef struct{
	int socketPersonaje;
	t_list *listaDeRecursosGanados;
	char* recursoQueBusca;
	char* simboloPersonaje;
	int bloqueado;
} __attribute__ ((packed)) t_personaje;


//Estructura del nivel

typedef struct{
        char *caja_nombre;
        char *caja_letra;
        int8_t caja_instancias;
        int8_t caja_posx;
        int8_t caja_posy;
} __attribute__ ((packed)) t_datosCaja;

typedef struct{
	char* recurso_letra;
	int8_t recurso_instancias;
} t_recursosDisponibles;



typedef struct{
        char *niv_nombre;
        char *niv_ip;
        int32_t niv_puerto;

        int32_t cantidadDeCajas;

        int8_t niv_recovery;
        int32_t niv_tiempoDeadlock;

        char *ip_Orquestador;
        int32_t puerto_orquestador;

        t_list *listaDeCajas;

} __attribute__ ((packed)) t_nivel;

typedef struct {
	char* simboloPj;
	char* recursoAsignado;
}t_recursos_asignados;

//se pasa como parametro la ip completa, devuelve el puerto
int32_t obtener_Puerto(char* direccion);

//se pasa como parametro la ip completa, devuelve la IP
char* obtener_IP(char* direccion);

t_nivel *iniciar_Nivel(t_config *config);

void agregar_Personaje(int socketPersonaje);

t_datosCaja *conseguir_Info_Caja(char*caja);

char *agregar_Indice_Al_String(int indice);

void agregar_Recurso(int socketPersonaje, char* recurso);

void agregar_Simbolo(int socketPersonaje,char* simboloPersonaje);

void dibujar_Nueva_Posicion(int socketPersonaje,t_destino *nuevaPos);

void inicializar_Posicion(char* simboloPersonaje);

void agregar_Recurso_Buscado(int socketPersonaje,char* recurso);

void agregar_Lista_Recurso_Disponible(t_datosCaja *datosCaja);

void quitar_Recurso_Buscado(int socketPersonaje, char *recurso);

int cantidad_Recursos_Ganados(int socketPersonaje);

char* simbolo_Personaje(int socketPersonaje);

t_personaje *buscar_Eliminar_Personaje(int socketPersonaje);

char* eliminar_Recurso(t_list *listaRecursosGanados);

void agregar_Recursos_Liberados_A_Disponibles(t_personaje *personaje, int cantRecursosGanados);

void actualizar_Recursos(t_list *listaRecursosLibres, t_list *listaDeCajas);

void actualizar_lista_personajes(t_list *listaRecursosAsignados);

void asignar_Recurso_Personaje_Asignado(t_recursos_asignados *recursoAsignado);

#endif /* INICIARNIVEL_H_ */
