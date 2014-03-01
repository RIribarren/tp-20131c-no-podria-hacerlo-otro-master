/*
 * mensajesNivel.c
 *
 *  Created on: 22/05/2013
 *      Author: utnso
 */


#include "mensajesNivel.h"


void enviar_Datos_Nivel(t_nivel *nivel, int socketOrquestador)
{

        t_info_datos_nivel *datosNivel = malloc(sizeof(t_info_datos_nivel));


        //se obtiene la ip, puerto y nombre del nivel
        datosNivel = obtener_Datos_Nivel(nivel);

        //variables para la serializacion
        char *datosSerializados = malloc(strlen(datosNivel->ip_Nivel) + 1 + strlen(datosNivel->nombre_Nivel) + 1 + sizeof(int32_t));
        size_t longitudDatos = strlen(datosNivel->ip_Nivel) + 1 + strlen(datosNivel->nombre_Nivel) + 1 + sizeof(int32_t);

        //serializando
        datosSerializados = serializar_Datos_Nivel(datosNivel, longitudDatos);



        //creando y enviando el paquete
        t_paquete *paquete = crear_Paquete(DATOS_DEL_NIVEL, datosSerializados, longitudDatos);
        enviar_Paquete(socketOrquestador, paquete);

        free(paquete->data);
        free(paquete);
        free(datosNivel->ip_Nivel);
        free(datosNivel->nombre_Nivel);
        free(datosNivel);
        free(datosSerializados);
}

t_info_datos_nivel *obtener_Datos_Nivel(t_nivel *nivel)
{
        t_info_datos_nivel *datosNivel = malloc(sizeof(t_info_datos_nivel));

        datosNivel->ip_Nivel = malloc(strlen(nivel->niv_ip) + 1);
        strcpy(datosNivel->ip_Nivel, nivel->niv_ip);

        datosNivel->nombre_Nivel = malloc(strlen(nivel->niv_nombre) + 1);
        strcpy(datosNivel->nombre_Nivel, nivel->niv_nombre);

        datosNivel->puerto_Nivel = nivel->niv_puerto;

        return datosNivel;
}


t_destino *buscar_Destino(char* recurso, t_list *listaDeCajas)
{
	t_destino *destino = malloc(sizeof(t_destino));
	t_datosCaja *datosCaja = malloc(sizeof(t_datosCaja));

	bool es_Misma_Caja(t_datosCaja *p)
	{
		return strcmp(p->caja_letra, recurso) == 0;
	}


	datosCaja = list_find(listaDeCajas, (void *) es_Misma_Caja );

	destino->x = datosCaja->caja_posx;
	destino->y = datosCaja->caja_posy;


	return destino;
}


void enviar_Destino(int socketPersonaje, t_destino* destino)
{

	t_paquete *paquete = malloc(sizeof(t_paquete));
	size_t longitud_Destino = sizeof(t_destino);
	char* destino_Serializado = malloc(sizeof(t_destino));

	destino_Serializado = serializar_Destino(destino, longitud_Destino);

	paquete = crear_Paquete(DESTINO, destino_Serializado, longitud_Destino);

	int nroBytesEnviados = enviar_Paquete(socketPersonaje, paquete);

	free(paquete);

	if(nroBytesEnviados < 0)
	{
		perror("Error al enviar el destino al personaje\n");
		exit(1);
	}//else

	//TODO	printf("Le mande el destino al pj \n");
}

int verificar_Posicion(t_datosParaPedirInstancia *datos, t_list *listaDeCajas)
{
	char *recurso = malloc(strlen(datos->recurso) + 1);
	strcpy(recurso,datos->recurso);

	//busca en la lista de las cajas, la struct con el mismo recurso, para poder ver la posX y la posY

	bool es_Misma_Caja(t_datosCaja *ptr)
		{
			return strcmp(ptr->caja_letra, recurso) == 0;
		}

	t_datosCaja *datosCaja = list_remove_by_condition(listaDeCajas, (void *) es_Misma_Caja);

	//se fija si las posX y posY son iguales, tambien se fija si hay suficientes instancias
	if((datosCaja->caja_posx == datos->posX) && (datosCaja->caja_posy == datos->posY) && (datosCaja->caja_instancias > 0))
	{
		datosCaja->caja_instancias--; //TODO
		char letra = datosCaja->caja_letra[0];


		restarRecurso(ListaItems, letra); //parte grafica

		nivel_gui_dibujar(ListaItems);
		list_add(listaDeCajas, datosCaja);


		bool es_Mismo_Recurso(t_recursosDisponibles *ptr)
				{
					return strcmp(ptr->recurso_letra, recurso) == 0;
				}

	//edito la lista de recursos disponibles
	t_recursosDisponibles *recursoDisponible = list_remove_by_condition(listaDeRecursosDisponibles, (void *) es_Mismo_Recurso);

	recursoDisponible->recurso_instancias--;

	list_add(listaDeRecursosDisponibles, recursoDisponible);

		return YES;

	}else
	{
				list_add(listaDeCajas, datosCaja);
				return NO;
	}
}

void enviar_Respuesta_Otorgado(int socketPersonaje, int respuesta)
{
	t_paquete *paquete = malloc(sizeof(t_paquete));
	char* stringRespuesta;

	if(respuesta == YES)
		stringRespuesta = "YES";
	else if(respuesta == NO)
		stringRespuesta = "NO";
	else
	{
		perror("Error en poner una respuesta valida en la funcion enviar_Respuesta_Otorgado\n");
		exit(1);
	}

	size_t longitud_respuesta = strlen(stringRespuesta) + 1;

		paquete = crear_Paquete(PEDIR_INSTANCIA, stringRespuesta, longitud_respuesta);

		int nroBytesEnviados = enviar_Paquete(socketPersonaje, paquete);

		if(nroBytesEnviados < 0)
		{
			perror("Error en enviar una respuesta de instancia al personaje\n");
			exit(1);
		}

		free(paquete);
}
