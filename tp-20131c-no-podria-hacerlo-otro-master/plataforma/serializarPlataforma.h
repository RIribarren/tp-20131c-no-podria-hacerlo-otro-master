/*
 * serializarPlataforma.h
 *
 *  Created on: 09/05/2013
 *      Author: clavelli
 */

#ifndef SERIALIZARPLATAFORMA_H_
#define SERIALIZARPLATAFORMA_H_


#include "bibliotecas.h"
#include "orquestador.h"

typedef struct {
	int8_t id;
	int16_t size_data;
	char *data;
} __attribute__ ((packed)) t_paquete;

#define SIZE_CABECERA (sizeof(int8_t) + sizeof(int16_t)) //la cabecera se compone de un id y del tamaño del mensaje que se va a enviar


typedef struct{
	int socketPj;
	char* simboloPj;
	char* recursoPj;
} t_cola;

typedef struct{
	char* ip_Plan;
	int32_t puerto_Plan;
	char* ip_Nivel;
	int32_t puerto_Nivel;
} __attribute__ ((packed)) t_info_nivel_plan;

typedef struct{
	char* nombre_Nivel;
	char* ip_Nivel;
	int32_t puerto_Nivel;
} __attribute__ ((packed)) t_info_datos_nivel;

typedef struct{
	char* nombre_Nivel;
	char* ip_Nivel;
	int32_t puerto_Nivel;
	char* ip_Planif;
	int32_t puerto_Planif;
}__attribute__ ((packed)) t_info_datos_nivel_con_planif;

typedef struct {
	char* nombreNivel;
	char* simboloPersonaje;
	t_list *listaRecursosLibres;
}t_datosParaRecursosLiberados;


t_paquete *crear_Paquete(int8_t id, char *data, int16_t size_data);

int enviar_Paquete(int socket,t_paquete *paquete);

t_paquete *recibir_Paquete(int socket);

char *serializar_Paquete(t_paquete *paquete,size_t *longitud_Stream);

char *serializar_Info(t_info_nivel_plan *info, size_t longitud_info);

t_info_nivel_plan *deserializar_Info(char *data, size_t longitud_info);

t_info_datos_nivel *deserializar_Info_Datos_Nivel(char* data, size_t longitud_info_nivel);

t_paquete *deserializar_Cabecera(char* data);

t_datosParaRecursosLiberados *deserializar_Info_Recursos_Liberados(char* datos_serializados,size_t longitud_datos);

char* serializar_Recursos_Asignados(t_list* listaRecursosAsignados, size_t longitud_datos_asignados, int cantRecursosAsignados);

char* serializar_Recursos_Libre(t_list* listaRecursosLibres, size_t longitud_datos_libres, int cantRecursosLibres);

t_list *deserializar_Personajes_Bloqueados(char *datos_serializados, size_t longitud_datos);

#endif /* SERIALIZARPLATAFORMA_H_ */
