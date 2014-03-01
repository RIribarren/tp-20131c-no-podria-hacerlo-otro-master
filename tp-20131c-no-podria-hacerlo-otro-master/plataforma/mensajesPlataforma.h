/*
 *
 * mensajePlataforma.h
 *
 *  Created on: 09/05/2013
 *      Author: clavelli
 */

#ifndef MENSAJESSERVIDOR_H_
#define MENSAJESSERVIDOR_H_

#include "bibliotecas.h"
#include "definesPlataforma.h"
#include "orquestador.h"
#include "serializarPlataforma.h"

t_niveles *buscar_Nivel(t_list *listaDeNiveles, char* nombreNivel);
t_niveles *buscar_Nivel_De_Lista_Niveles(t_list *listaDeNiveles, char* nombreNivel);


void agregar_Personaje_Lista(int socketPersonaje, char* simbolo, t_listas* structDelNivel);

t_listas *listas_create(char* nombre);

int obtener_Nivel (char* nivel);

void enviar_Datos_Nivel (t_niveles *nodoDelNivel, int socket_personaje);

t_info_datos_nivel *inicializar_Listas_Nivel(t_paquete *paquete);

t_niveles *agregar_Nivel_A_Lista(t_info_datos_nivel *info_datos_nivel, int puerto);

void agregar_Personaje_A_Lista_Niveles(int socketPersonaje, t_datos_para_planificador* nivel);

void avisar_Movimiento_Permitido (int socket_personaje);

void compactar_Descriptores(int* nfds, struct pollfd* fds);

t_cola *buscar_Pj_Que_Solicita_Recurso(t_list *listaBloqueados, char* recurso);

t_list* filtrar_Solo_Bloqueados(t_list *listaDeNiveles,char* nivel);

t_listas *agregar_Lista_Recursos(t_cola *nodoPj,t_listas *nodoDeListas,char* recursoActual,t_lista_recursos *structRecursos);

void avisar_Pj_Desbloqueado(char* simbolo);

int *list_index_by_condition(t_list *self, bool(*condition)(void*));

t_listas *buscar_Mismo_Nivel(char* nombreNivel);

t_listas *encontrar_Mismo_Nivel(char* nombreNivel);

void agregar_Pj_A_Online(char *simboloPj);

void sacar_Pj_Online(char *simboloPj);

void cambiar_Estado_Bloqueado(int socket_personaje, char* simboloPj,char* nombreNivel);

void destroy_paquete(t_paquete *paquete);

void mostrar_Lista(t_listas *nodoDelNivel, char* nombreNivel);

void avisar_Victima_Seleccionada(t_list *listaPersonajesOrquestador,t_datos_pj *nodoPrimerPj, char* nombreNivel);

void enviar_Struct_a_Nivel(t_lista_recursos *structRecursos,int socketNivel);

#endif /* MENSAJESSERVIDOR_H_ */
