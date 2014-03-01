/*
 * mensajePlataforma.c
 *
 *  Created on: 11/05/2013
 *      Author: utnso
 */

#include "mensajesPlataforma.h"


t_niveles *buscar_Nivel(t_list *listaDeNiveles, char* nombreNivel)
{
	t_niveles *nodoDelNivel = malloc(sizeof(t_niveles));

    bool es_El_Mismo_Nivel(t_niveles *ptr)
     {
     return strcmp(ptr->nombre_Nivel, nombreNivel) == 0;
     }

    nodoDelNivel = list_find(listaDeNiveles, (void*)es_El_Mismo_Nivel);
    //printf("%d\n",structDelNivel->puerto_Nivel);
   // printf("%s\n",structDelNivel->ip_Nivel);
   // printf("%s\n",structDelNivel->ip_Planif);
    //printf("%d\n",structDelNivel->puerto_Planif);
    return nodoDelNivel;
}

//TODO VER SI ESTA BIEN
t_niveles *buscar_Nivel_De_Lista_Niveles(t_list *listaDeNiveles, char* nombreNivel)
{
	t_niveles *nodoDelNivel = malloc(sizeof(t_niveles));

    bool es_El_Mismo_Nivel(t_niveles *ptr)
     {
     return strcmp(ptr->nombre_Nivel, nombreNivel) == 0;
     }

    nodoDelNivel = list_remove_by_condition(listaDeNiveles, (void*)es_El_Mismo_Nivel);
    //printf("%d\n",structDelNivel->puerto_Nivel);
   // printf("%s\n",structDelNivel->ip_Nivel);
   // printf("%s\n",structDelNivel->ip_Planif);
    //printf("%d\n",structDelNivel->puerto_Planif);
    return nodoDelNivel;
}

t_listas *buscar_Mismo_Nivel(char* nombreNivel)
{
	t_listas *structDelNivel = malloc(sizeof(t_listas));

    bool es_El_Mismo_Nivel(t_listas *ptr)
     {
     return strcmp(ptr->nombreNivel, nombreNivel) == 0;
     }

    structDelNivel = list_remove_by_condition(listaDeListas, (void*)es_El_Mismo_Nivel);

  //  printf("TAMANIO LISTADELISTOS DEL NIVEL %s: %d\n",nombreNivel,list_size(structDelNivel->listaListos));
    return structDelNivel;
}

t_listas *encontrar_Mismo_Nivel(char* nombreNivel)
{
	t_listas *structDelNivel = malloc(sizeof(t_listas));

    bool es_El_Mismo_Nivel(t_listas *ptr)
     {
     return strcmp(ptr->nombreNivel, nombreNivel) == 0;
     }

    structDelNivel = list_find(listaDeListas, (void*)es_El_Mismo_Nivel);

    return structDelNivel;
}


void agregar_Personaje_Lista(int socketPersonaje, char* simbolo, t_listas* structDelNivel)
{
	t_cola *personaje = malloc(sizeof(t_cola));

	personaje->socketPj = socketPersonaje;
	personaje->recursoPj = '\0';
	personaje->simboloPj = simbolo;

	list_add(structDelNivel->listaListos, personaje);
}



t_listas *listas_create(char* nombre)
{

     t_listas *new = malloc(sizeof(t_listas));

     new->nombreNivel = malloc(strlen(nombre)+ 1);
     strcpy(new->nombreNivel, nombre);

     t_list *listaListos = list_create();
     //colaListos=NULL;
     new->listaListos = listaListos;
     t_list *listaBloqueados = list_create();
     //colaBloqueados=NULL;
     new->listaBloqueados = listaBloqueados;
     t_list *listaAnormales = list_create();
     //colaAnormales=NULL;
     new->listaAnormales = listaAnormales;
     //colaDesconexiones
     t_list* listaDesconexiones = list_create();
     new->listaDesconexiones = listaDesconexiones;

    return new;
}

int obtener_Nivel (char* nivel)
{
char* nivel_string = (char *)malloc(20);
memset(nivel_string, '\0', 10);
int nivel_transformado;
int j = 0, i;

for(i=5; nivel[i] != '\0'; i++)
{
 nivel_string[j] = nivel[i];
 j++;
}

nivel_transformado = atoi(nivel_string);

return nivel_transformado;
}

void enviar_Datos_Nivel (t_niveles *nodoDelNivel, int socket_personaje)
{
	t_paquete *paquete = malloc(sizeof(t_paquete));
	t_info_nivel_plan *info = malloc(sizeof(t_info_nivel_plan));

	info->ip_Nivel = malloc(strlen(nodoDelNivel->ip_Nivel) + 1);
	strcpy(info->ip_Nivel,nodoDelNivel->ip_Nivel);

	info->ip_Plan = malloc(strlen(nodoDelNivel->ip_Planif) + 1);
	strcpy(info->ip_Plan, nodoDelNivel->ip_Planif);

	info->puerto_Nivel = nodoDelNivel->puerto_Nivel;
	info->puerto_Plan = nodoDelNivel->puerto_Planif;
/*
	printf("IP del Nivel: %s\n",info->ip_Nivel);
	printf("Puerto del nivel: %d\n",info->puerto_Nivel);
	printf("IP del Planificador: %s\n",info->ip_Plan);
	printf("Puerto del Planificador: %d\n",info->puerto_Plan);
*/

	char *info_serializada = malloc(sizeof(t_info_nivel_plan));
	size_t longitud_info = (strlen(info->ip_Nivel) + 1) + sizeof(int32_t) + sizeof(int32_t) + (strlen(info->ip_Plan) + 1);

	info_serializada = serializar_Info(info, longitud_info);

	paquete = crear_Paquete(IPyPUERTO, info_serializada , longitud_info);

	enviar_Paquete(socket_personaje, paquete);

	free(info->ip_Nivel);
	free(info->ip_Plan);

}

//void compactar_Descriptores(int* nfds, struct pollfd* fds)
//{
//	int j;
//	int i;
//	 for (i = 0; i < *nfds; i++)
//	      {
//	        if (fds[i].fd == -1)
//	        {
//	          for(j = i; j < *nfds; j++)
//	          {
//	            fds[j].fd = fds[j+1].fd;
//	          }
//	          nfds--;
//	        }
//	      }

//	 fds = (struct pollfd *)realloc(fds,*nfds*sizeof(struct pollfd));
//}


t_info_datos_nivel *inicializar_Listas_Nivel(t_paquete *paquete)
{
	 int longitud_info_datos_nivel = paquete->size_data;
	 t_info_datos_nivel *info_datos_nivel = malloc(longitud_info_datos_nivel);

	 info_datos_nivel = deserializar_Info_Datos_Nivel(paquete->data, longitud_info_datos_nivel);

	 t_listas *unStructDeColas = listas_create(info_datos_nivel->nombre_Nivel);
	 list_add(listaDeListas, unStructDeColas);

	 return info_datos_nivel;
}


t_niveles *agregar_Nivel_A_Lista(t_info_datos_nivel *info_datos_nivel, int puerto)
{
	t_niveles *structNivel = malloc(sizeof(t_niveles));

 structNivel->nombre_Nivel = malloc(strlen(info_datos_nivel->nombre_Nivel) + 1);
 strcpy(structNivel->nombre_Nivel,info_datos_nivel->nombre_Nivel);

 structNivel->ip_Nivel = malloc(strlen(info_datos_nivel->ip_Nivel) + 1);
 strcpy(structNivel->ip_Nivel,info_datos_nivel->ip_Nivel);

 structNivel->ip_Planif = malloc(strlen(ipOrquestador) + 1);
 strcpy(structNivel->ip_Planif, ipOrquestador);


 structNivel->puerto_Nivel = info_datos_nivel->puerto_Nivel;
 structNivel->puerto_Planif = puerto;
 structNivel->listaDePjsJugando = list_create();


 list_add(listaDeNiveles,structNivel);
/*
 printf("Se agrego a la lista de niveles, el: ");
 printf("%s\n",structNivel->nombre_Nivel);
 printf("con IP: %s\n",structNivel->ip_Nivel);
 printf("de puerto: %d\n",structNivel->puerto_Nivel);
 printf("Su planificador es: %s\n",structNivel->ip_Planif);
 printf("y su puerto es: %d\n",structNivel->puerto_Planif);
*/
 return structNivel;
}

void avisar_Movimiento_Permitido (int socket_personaje)
{
	t_paquete *paquete = NULL;

	paquete = crear_Paquete(MOVIMIENTO_PERMITIDO,"Movimiento permitido",strlen("Movimiento permitido") + 1);
	enviar_Paquete(socket_personaje,paquete);
	free(paquete);
}

/*
int *list_index_by_condition(t_list *self, bool(*condition)(void*)) {
	int index = 0;

	t_link_element* element = list_find_element(self, condition, &index);
	if (element != NULL) {
		return index;
	}

	return -1;
}

int *buscar_Pos_Nivel(t_list *listaDePjsJugando, char* nombreNivel)
{
	int index;

    bool es_El_Mismo_Nivel(t_listas *ptr)
     {
     return strcmp(ptr->nombreNivel, nombreNivel) == 0;
     }

    index = list_index_by_condition(listaDePjsJugando, (void*)es_El_Mismo_Nivel);
    printf("Index del nodo es:%d\n", index);
    return index;
}
*/
t_list* filtrar_Solo_Bloqueados(t_list *listaDeNiveles,char* nivel)
{
	 bool es_El_Mismo_Nivel(t_listas *ptr)
	     {
	     return strcmp(ptr->nombreNivel, nivel) == 0;
	     }
	t_niveles *nodoDelNivel = malloc(sizeof(t_niveles));
	nodoDelNivel =  list_find(listaDeNiveles, (void*)es_El_Mismo_Nivel);

	//printf("CANTIDAD DE PJS JUGANDO: %d\n", list_size(nodoDelNivel->listaDePjsJugando));

	bool esta_Bloqueado(t_datos_pj *ptr)
		     {
		     return ptr->bloqueado == TRUE;
		     }

	t_list *listaDePjsBloqueados= list_filter(nodoDelNivel->listaDePjsJugando,(void*) esta_Bloqueado);


	//prueba
/*	int n = list_size(listaDePjsBloqueados);
	int e = 0;
	t_datos_pj *prueba = malloc(sizeof(t_datos_pj));

	while(e < n)
	{
		prueba = list_get(listaDePjsBloqueados, e);
	//	printf("SIMBOLO FILTRADO: %s\n", prueba->simboloPj);
		e++;
	}
*/



	return listaDePjsBloqueados;
}


void avisar_Victima_Seleccionada(t_list *listaPersonajesOrquestador,t_datos_pj *nodoPrimerPj, char* nombreNivel)
{
	t_personajeOrquestador *nodoVictima = malloc(sizeof(t_personajeOrquestador));
	pthread_mutex_lock( &mutex1 );
	t_listas *nodoNivel = buscar_Mismo_Nivel(nombreNivel);

	bool es_El_Mismo_Simbolo(t_personajeOrquestador *ptr)
	{
	return strcmp(ptr->simboloPersonaje, nodoPrimerPj->simboloPj) == 0;
	}


	nodoVictima = list_find(listaPersonajesOrquestador,(void*)es_El_Mismo_Simbolo);

	if(nodoVictima != NULL)
	{
	log_warning(logger, "Nodo victima del Interbloqueo: %s", nodoVictima->simboloPersonaje);

	t_paquete *paquete = crear_Paquete(RESPUESTA_ORQUESTADOR,"Morite",strlen("Morite")+1);

	bool es_El_Mismo_Simbolo_Bloqueado(t_cola *ptr)
		{
			return strcmp(ptr->simboloPj, nodoPrimerPj->simboloPj) == 0;
		}


	t_cola *nodoNoBloqueado = list_remove_by_condition(nodoNivel->listaBloqueados, (void *) es_El_Mismo_Simbolo_Bloqueado);

	list_add(nodoNivel->listaDesconexiones, nodoNoBloqueado);
	list_add(nodoNivel->listaAnormales, nodoNoBloqueado);

	list_add(listaDeListas, nodoNivel);
	pthread_mutex_unlock( &mutex1 );
	enviar_Paquete(nodoVictima->socketPersonaje,paquete);

	free(paquete->data);
	free(paquete);


	}
}


t_cola *buscar_Pj_Que_Solicita_Recurso(t_list *listaBloqueados, char* recurso)
{
	t_cola *nodoPj = malloc(sizeof(t_cola));

	bool es_El_Mismo_Recurso(t_cola *ptr)
		 {
		  return strcmp(ptr->recursoPj, recurso) == 0;
		  }
	nodoPj = list_remove_by_condition(listaBloqueados,(void*) es_El_Mismo_Recurso);

	return nodoPj;
}

t_listas *agregar_Lista_Recursos(t_cola *nodoPj,t_listas *nodoDeListas,char* recurso,t_lista_recursos *structRecursos)
{
	//si el nodoPj es NULL, significa que no encontro un Pj que necesite ese recurso
	//entonces lo agrego a la lista de recursos libres
	char *recursoActual = malloc(strlen(recurso) + 1);
	strcpy(recursoActual,recurso);

	if(nodoPj == NULL)
	{
		//char *recurso = malloc(1);
		//recurso = recursoActual;
		list_add(structRecursos->listaRecursosLibres,recursoActual);
	}
	//sino, agrego a la lista de recursos asignados, el recurso que le di a un Pj (simbolo),
	//y lo agrego a la listaDeListos.
	else
	{
		t_recursos_asignados *nodoRecurso = malloc(sizeof(t_recursos_asignados));

		nodoRecurso->simboloPj = malloc(strlen(nodoPj->simboloPj) + 1);
		strcpy(nodoRecurso->simboloPj,nodoPj->simboloPj);

		nodoRecurso->recursoAsignado = malloc(strlen(recursoActual) + 1);
		strcpy(nodoRecurso->recursoAsignado,recursoActual);

		list_add(structRecursos->listaRecursosAsignados,nodoRecurso);
		//paso a listos al pj, y le aviso
		list_add(nodoDeListas->listaListos, nodoPj);
		avisar_Pj_Desbloqueado(nodoRecurso->simboloPj);
	}
	return nodoDeListas;
}

void avisar_Pj_Desbloqueado(char* simbolo)
{
	t_personajeOrquestador *nodoPjOrquestador = malloc(sizeof(t_personajeOrquestador));

	bool es_El_Mismo_Simbolo(t_personajeOrquestador *ptr)
			 {
			  return strcmp(ptr->simboloPersonaje, simbolo) == 0;
			  }

	nodoPjOrquestador = list_find(listaPersonajesOrquestador,(void*)es_El_Mismo_Simbolo);

	t_paquete *paquete = malloc(sizeof(t_paquete));
	paquete = crear_Paquete(RESPUESTA_ORQUESTADOR,"Estas desbloqueado",strlen("Estas desbloqueado") + 1);
	enviar_Paquete(nodoPjOrquestador->socketPersonaje,paquete);
	free(paquete);
}

void enviar_Struct_a_Nivel(t_lista_recursos *structRecursos,int socketNivel)
{
	int cantRecursosLibres = list_size(structRecursos->listaRecursosLibres);
	int cantRecursosAsignados = list_size(structRecursos->listaRecursosAsignados);
/*
	printf("CANTIDAD RECURSOS LIBRES: %d\n", cantRecursosLibres);
	printf("CANTIDAD RECURSOS ASIGNADOS %d\n", cantRecursosAsignados);
*/
	size_t longitud_datos_libres = cantRecursosLibres * 2;
	size_t longitud_datos_asignados = cantRecursosAsignados * 4;


	t_paquete *paquete = malloc(sizeof(t_paquete));

	if(cantRecursosLibres > 0)
	{
	char* datos_libres_serializados = serializar_Recursos_Libre(structRecursos->listaRecursosLibres, longitud_datos_libres, cantRecursosLibres);
	paquete = crear_Paquete(RECURSOS_LIBRES,datos_libres_serializados,longitud_datos_libres);
	enviar_Paquete(socketNivel,paquete);

	}else
	{
		paquete = crear_Paquete(RECURSOS_LIBRES_CERO, "Hola", strlen("hola")+1);
		enviar_Paquete(socketNivel, paquete);

	}

	free(paquete);

	if(cantRecursosAsignados > 0)
	{
	char* datos_asignados_serializados = serializar_Recursos_Asignados(structRecursos->listaRecursosAsignados, longitud_datos_asignados, cantRecursosAsignados);
	paquete = crear_Paquete(RECURSOS_ASIGNADOS,datos_asignados_serializados,longitud_datos_asignados);
	enviar_Paquete(socketNivel,paquete);
	}else
	{
		paquete = crear_Paquete(RECURSOS_ASIGNADOS_CERO, "Hola", strlen("hola")+1);
		enviar_Paquete(socketNivel, paquete);
	}

	free(paquete);

}

void agregar_Pj_A_Online(char *simboloPj)
{
	int cantidadElementos = list_size(pjsOnline);
	int pjVisto = 0;
	char* simbolo;
	int encontrado = FALSE;

	if(primeraVez)
	{
		list_add(pjsOnline, simboloPj);
		primeraVez = FALSE;
		encontrado = TRUE;
	}else
	{
		while((pjVisto < cantidadElementos) && (encontrado == FALSE))
		{
		simbolo = list_get(pjsOnline,pjVisto);

			if(strcmp(simbolo,simboloPj) == 0)
			{
				encontrado = TRUE;
			}
			else
			{
				pjVisto++;
			}
		}


	}

	if(encontrado == FALSE)
			{
			list_add(pjsOnline, simboloPj);
			}

}

void sacar_Pj_Online(char *simboloPj)
{
	int cantidadElementos = list_size(pjsOnline);
	int pjVisto = 0;
	char* simbolo;
	char* simboloSacado;
	int encontrado = FALSE;

		while((pjVisto < cantidadElementos) && (encontrado == FALSE))
		{
			simbolo = list_get(pjsOnline,pjVisto);

			if(strcmp(simbolo,simboloPj) == 0)
			{
				encontrado = TRUE;
				simboloSacado = list_remove(pjsOnline, pjVisto);
	//			printf("SAQUE AL SIMBOLO: %s", simboloSacado);
		//		printf("TAMAÑO DE LISTA DE PJS ONLINE: %d", list_size(pjsOnline));
			}
			else
				pjVisto++;
		}

		if(encontrado == FALSE)
			printf("Error en sacar el pj de la lista de pjs Online\n");
}

int esta_En_Lista_Orquestador(char *simboloPj, char* nombreNivel)
{
	t_personajeOrquestador *nodoVictima = malloc(sizeof(t_personajeOrquestador));
	t_listas *nodoNivel = encontrar_Mismo_Nivel(nombreNivel);

		bool es_El_Mismo_Simbolo(t_personajeOrquestador *ptr)
		{
		return strcmp(ptr->simboloPersonaje, simboloPj) == 0;
		}


		nodoVictima = list_find(listaPersonajesOrquestador,(void*)es_El_Mismo_Simbolo);

		if(nodoVictima == NULL)
			return FALSE;
		else
			return TRUE;
}

void cambiar_Estado_Bloqueado(int socket_personaje, char* simboloPj,char* nombreNivel)
{
	t_datos_pj *nodoPj = malloc(sizeof(t_datos_pj));

	nodoPj->simboloPj = malloc(strlen(simboloPj) + 1);
	strcpy(nodoPj->simboloPj, simboloPj);

	nodoPj->socketPj = socket_personaje;
	nodoPj->bloqueado = FALSE;

//	printf("PJ A CAMBIAR: %s\n", nodoPj->simboloPj);

	int cont =0;
	int es_el_mismo=FALSE;
	int index;
	t_niveles *nivel = buscar_Nivel_De_Lista_Niveles(listaDeNiveles, nombreNivel);
	t_datos_pj *nodoPjEnJuego = malloc(sizeof(t_datos_pj));

	while(es_el_mismo==FALSE)
	{
		nodoPjEnJuego = list_get(nivel->listaDePjsJugando,cont);
		if(strcmp(nodoPjEnJuego->simboloPj, simboloPj) == 0)
		{
			es_el_mismo=TRUE;
			index=cont;
		}
		else{cont++;}
	}



	t_datos_pj *aux = list_replace(nivel->listaDePjsJugando,index,nodoPj);
	free(aux->simboloPj);
	free(aux);

/*
	int s = 0;

	while(s < list_size(nivel->listaDePjsJugando))
	{
		t_datos_pj *prueba = malloc(sizeof(t_datos_pj));

		printf("SIMBOLO: %s\n", prueba->simboloPj);
		printf("SOCKET: %d\n", prueba->socketPj);
		printf("BLOQUEADO: %d\n", prueba->bloqueado);

		s++;


	}


*/


	list_add(listaDeNiveles, nivel);
}

void destroy_paquete(t_paquete *paquete)
{
	free(paquete->data);
	free(paquete);

}

void mostrar_Lista(t_listas *nodoDelNivel, char* nombreNivel)
{
	int contador = 0;
	int cantListos = list_size(nodoDelNivel->listaListos);
	int cantBloqueados = list_size(nodoDelNivel->listaBloqueados);
	int cantAnormales = list_size(nodoDelNivel->listaAnormales);

	char* buffer = malloc(strlen("Listos del ") + 1 + strlen(nombreNivel) + 1 + strlen(":") + 1 + sizeof(char) + 1 * cantListos +(strlen("<-") + 1) * (cantListos - 1));

	t_cola* nodoPj = malloc(sizeof(t_cola));


	strcpy(buffer,"Listos del ");
	strcat(buffer, nombreNivel);
	strcat(buffer, ":");

	while(contador < cantListos)
	{
		nodoPj = list_get(nodoDelNivel->listaListos, contador);

		if(contador == 0)
			strcat(buffer, nodoPj->simboloPj);
		else
		{
			strcat(buffer, "<-");
			strcat(buffer, nodoPj->simboloPj);

		}



		contador++;

	}

	log_info(logger, "%s", buffer);

	free(buffer);


		contador = 0;


		char* buffer2 = malloc(strlen("Bloqueados del ") + 1 + strlen(nombreNivel) + 1 + strlen(":") + 1 + sizeof(char) + 1 * cantBloqueados +(strlen("<-") + 1) * (cantBloqueados - 1));



		strcpy(buffer2,"Bloqueados del ");
		strcat(buffer2, nombreNivel);
		strcat(buffer2, ":");

		while(contador < cantBloqueados)
		{
			nodoPj = list_get(nodoDelNivel->listaBloqueados, contador);

			if(contador == 0)
				strcat(buffer2, nodoPj->simboloPj);
			else
			{
				strcat(buffer2, "<-");
				strcat(buffer2, nodoPj->simboloPj);

			}



			contador++;

		}

		log_info(logger, "%s", buffer2);

		free(buffer2);


		contador = 0;


		char* buffer3 = malloc(strlen("Anormales del ") + 1 + strlen(nombreNivel) + 1 + strlen(":") + 1 + sizeof(char) + 1 * cantAnormales +(strlen("<-") + 1) * (cantAnormales - 1));


			strcpy(buffer3,"Anormales del ");
			strcat(buffer3, nombreNivel);
			strcat(buffer3, ":");

			while(contador < cantAnormales)
			{
				nodoPj = list_get(nodoDelNivel->listaAnormales, contador);

				if(contador == 0)
					strcat(buffer3, nodoPj->simboloPj);
				else
				{
					strcat(buffer3, "<- ");
					strcat(buffer3, nodoPj->simboloPj);

				}



				contador++;

			}


			log_info(logger, "%s", buffer3);

			free(buffer3);

}

