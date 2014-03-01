/*
 * iniciarPersonaje.c
 *
 *  Created on: 10/05/2013
 *      Author: utnso
 */

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/config.h>
#include "iniciarPersonaje.h"


//se pasa como parametro la ip completa, devuelve el puerto
int32_t obtener_Puerto(char* direccion)
{
	char *puerto = (char *) malloc(5);
	int32_t puertoNum;

	int i = 0;
	while(direccion[i] != ':')
		i += 1;

	strncpy(&puerto[0],&direccion[i+1],4);

	puerto[4] = '\0';

	puertoNum = atoi(puerto); //convierte el string del puerto a un int

	return puertoNum;
}

//se pasa como parametro la ip completa, devuelve la IP
char* obtener_IP(char* direccion)
{
	char *ip = (char *) malloc((strlen(direccion)-4)); //asigna memoria suficiente para la IP, el -4 es por el puerto
	int i = 0;
	int j= 0;
	while(direccion[i] != ':')
		strncpy(&ip[j++],&direccion[i++],1);

	ip[j] = '\0';

	return ip;
}

t_personaje *iniciar_Personaje(t_config *config)
{
		t_personaje *personaje = malloc(sizeof(t_personaje));
		char *direccionIP_Puerto;

			personaje->pj_vidas = config_get_int_value(config, "vidas");
			personaje->pj_nombre = config_get_string_value(config, "nombre");
			personaje->ip_local = config_get_string_value(config,"local");


			direccionIP_Puerto = config_get_string_value(config, "orquestador");

			personaje->ip_Orquestador = obtener_IP(direccionIP_Puerto);
			personaje->puerto_orquestador = obtener_Puerto(direccionIP_Puerto);
			personaje->pj_simbolo = config_get_string_value(config, "simbolo");



			personaje->pj_planDeNiveles = config_get_array_value(config, "planDeNiveles");

			personaje->pj_cant_niveles = config_get_int_value(config,"cantidadDeNiveles");

			int i;

			char *cantDeRecursosDelNivel;

			personaje->pj_cantidadDeRecursos = malloc(sizeof(int32_t) *personaje->pj_cant_niveles);

			for(i=0; i < personaje->pj_cant_niveles; i++)
			{


			//con cantidadDeRecursos[X], donde X es el "Nivel#", obtengo el total de recursos por cada nivel
			cantDeRecursosDelNivel = agregar_Recurso_Al_String(personaje->pj_planDeNiveles[i]);

			//agrego la cantidad de recursos al vector
			personaje->pj_cantidadDeRecursos[i] = config_get_int_value(config, cantDeRecursosDelNivel);


			}

	return personaje;
}

char* agregar_Nivel_Al_String(char* nivel)
{
	char* buffer = malloc(20);
	memset(buffer, '\0', 20);//TODO puede tener errores si la cant de niveles superan al 9, capaz con un memset se arregla
	int offset = 0, tmp_size = 0;

	tmp_size = strlen("obj[");

	memcpy(buffer,"obj[", tmp_size);

	offset += tmp_size;

	tmp_size = strlen(nivel);
	memcpy(buffer+offset, nivel, tmp_size);

	offset += tmp_size;

	tmp_size = strlen("]");
	memcpy(buffer+offset, "]", tmp_size);

	return buffer;
}

char *agregar_Recurso_Al_String(char *stringNivel)
{
	char *buffer = malloc(40);
	memset(buffer,'\0', 40);//ver si con un memset se arregla
	int offset = 0, tmp_size = 0;

	tmp_size = strlen("cantidadDeRecursos[");
	memcpy(buffer, "cantidadDeRecursos[", tmp_size);

	offset += tmp_size;

	tmp_size = strlen(stringNivel);
	memcpy(buffer + offset, stringNivel, tmp_size);

	offset += tmp_size;

	tmp_size = strlen("]");
	memcpy(buffer + offset, "]", tmp_size);

	return buffer;
}

char **obtener_Recursos_Nivel(char *nivel, t_config *config)
{
	char *nivelString;
	char **vectorDeRecursos;


	nivelString = agregar_Nivel_Al_String(nivel);

	vectorDeRecursos = config_get_array_value(config, nivelString);

	return vectorDeRecursos;
}

void destroy_paquete(t_paquete* paquete)
{
	free(paquete->data);
	free(paquete);
}

void destroy_info(t_info_nivel_plan *info)
{
	free(info->ip_Nivel);
	free(info->ip_Plan);
	free(info);
}

void destroy_personaje(t_personaje *personaje)
{
	free(personaje->ip_Orquestador);
	free(personaje->pj_nombre);
	free(personaje->pj_planDeNiveles);
	free(personaje);
}
