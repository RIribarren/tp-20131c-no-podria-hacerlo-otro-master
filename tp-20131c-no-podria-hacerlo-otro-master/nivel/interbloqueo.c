/*
 * interbloqueo.c
 *
 *  Created on: 12/06/2013
 *      Author: utnso
 */

#include "interbloqueo.h"
#

void *detectar_Interbloqueo(void *ptr)
{

	listaPersonajesBloqueados = list_create();

	t_list* listaDeMarcados = list_create();
	t_list* listaDisponiblesTemporal = list_create();

	t_list* listaPjsBloqueados = filtrar_bloqueados();


	if(list_size(listaPjsBloqueados) > 1)
	{
		int cantidadTemporal = list_size(listaDeRecursosDisponibles);
		int cont = 0;
		while(cont < cantidadTemporal)
		{
			t_recursosDisponibles *recursoDisponible = list_get(listaDeRecursosDisponibles, cont);

			t_recursosDisponibles *recursoDisponibleNuevo = malloc(sizeof(t_recursosDisponibles));

			recursoDisponibleNuevo->recurso_letra = malloc(strlen(recursoDisponible->recurso_letra) + 1);
			strcpy(recursoDisponibleNuevo->recurso_letra,recursoDisponible->recurso_letra);

			recursoDisponibleNuevo->recurso_instancias = recursoDisponible->recurso_instancias;

			list_add(listaDisponiblesTemporal, recursoDisponibleNuevo);

			cont++;
		}
//TODO
/*
	printf("MOSTRANDO LOS RECURSOS DE LA LISTA TEMPORAL:\n");
	int n = 0;
	while(n < list_size(listaDisponiblesTemporal))
	{
		t_recursosDisponibles *recurso = list_get(listaDisponiblesTemporal, n);
		printf("RECURSO: %s\n", recurso->recurso_letra);
		printf("INSTANCIAS: %d\n", recurso->recurso_instancias);
		n++;
	}
*/
	int finDeBusqueda = NO;

	hayInterbloqueo = NO;

	while(finDeBusqueda == NO)
	{

		 t_personaje* personaje = buscar_Personaje_Ejecutable(listaDisponiblesTemporal, listaDeMarcados, listaPjsBloqueados);

		if(personaje != NULL)
		{
			actualizar_Disponibles_Temporal(listaDisponiblesTemporal, personaje->listaDeRecursosGanados);

			char* simboloPersonaje = malloc(strlen(personaje->simboloPersonaje) + 1);
			strcpy(simboloPersonaje, personaje->simboloPersonaje);

			marcar_Proceso(listaDeMarcados, simboloPersonaje);
		}
		else
		{
			finDeBusqueda = YES;
		}
	}



	analizar_Interbloqueo(listaDeMarcados, listaPjsBloqueados);

	int listaVacia;

	listaVacia = list_is_empty(listaPersonajesBloqueados);

	if(listaVacia)

		hayInterbloqueo = NO;
	else
		hayInterbloqueo = YES;


	if(hayInterbloqueo)
	{
		log_warning(logger, "Hay interbloqueo");
		int cantProcesos = list_size(listaPersonajesBloqueados);
//TODO		printf("CANTIDAD DE PROCESOS BLOQUEADOS: %d", cantProcesos);
		int numProcesos = 0;
		while (numProcesos < cantProcesos)
		{
			char* simboloProceso = malloc(sizeof(char));

			strcpy(simboloProceso, list_get(listaPersonajesBloqueados, numProcesos));

			log_info(logger, "Personaje Bloqueado: %s", simboloProceso);

			numProcesos++;
		}
	}


	}

	list_clean(listaPjsBloqueados);
	list_destroy(listaPjsBloqueados);

	list_clean(listaDeMarcados);
	list_destroy(listaDeMarcados);

	list_clean(listaDisponiblesTemporal);
	list_destroy(listaDisponiblesTemporal);


	return 0;
}



t_personaje *buscar_Personaje_Ejecutable(t_list *listaDisponiblesTemporal,t_list *listaDeMarcados, t_list* listaPjsBloqueados)
{
	int encontrado = NO;

	int cantidadDePersonajes = list_size(listaPjsBloqueados);
	int personajeVisto = 0;
	int marcado;
	t_personaje *personaje = malloc(sizeof(t_personaje));
	while((personajeVisto < cantidadDePersonajes) && (encontrado == NO))
	{
		personaje = list_get(listaPjsBloqueados, personajeVisto);

		//se fija si el pj ya fue marcado, para pasar al sgte pj
		char *simboloPersonaje = malloc(strlen(personaje->simboloPersonaje) + 1);
		strcpy(simboloPersonaje, personaje->simboloPersonaje);

		marcado = buscar_Marcado(listaDeMarcados, simboloPersonaje);

		if(marcado == YES)
		{
			personajeVisto++;
		}
		else
		{
			int listaRecursosGanadosVacia = list_is_empty(personaje->listaDeRecursosGanados);

			if(listaRecursosGanadosVacia == YES)
			{
				encontrado = YES;
			}else
			{
				int puedeEjecutar;
				char *recursoQueBusca = malloc(strlen(personaje->recursoQueBusca) + 1);
				strcpy(recursoQueBusca, personaje->recursoQueBusca);

				puedeEjecutar = verificar_Instancias(listaDisponiblesTemporal, recursoQueBusca);

				if(puedeEjecutar == YES)
				{
					encontrado = YES;
				}else
				{
					personajeVisto++;
				}

			}
		}

	}
	if(encontrado == YES)
	return personaje;
	else
	{
		return personaje = NULL;
	}

}

int buscar_Marcado(t_list *listaDeMarcados,char* simboloPersonaje)
{

	int marcado = NO;



	int cont = 0;
	int cantidadMarcados = list_size(listaDeMarcados);
	int encontrado = FALSE;

	while(cont < cantidadMarcados && encontrado == FALSE)
	{
		char* pjMarcado = malloc(sizeof(char));
		pjMarcado = list_get(listaDeMarcados, cont);

		if(pjMarcado != NULL)
		{
		if( (strcmp(pjMarcado, simboloPersonaje) == 0) )
			encontrado = TRUE;
		else
			cont++;
		}
		else
			cont++;

	}


		if(encontrado == TRUE)
			marcado = YES;


	return marcado;
}

int verificar_Instancias(t_list *listaDisponiblesTemporal, char* recursoQueBusca)
{
	int puedeEjecutar = NO;
	bool es_El_Mismo_Recurso(t_recursosDisponibles *ptr)
								{
									return strcmp(ptr->recurso_letra, recursoQueBusca) == 0;
								}

	t_recursosDisponibles *nodoRecursoDisponible = list_find(listaDisponiblesTemporal, (void *) es_El_Mismo_Recurso);

	if(nodoRecursoDisponible == NULL)
	{
	//TODO	printf("---RECURSO INEXISTENTEEEEEEEEEE----\n");
	}
	else if(nodoRecursoDisponible->recurso_instancias > 0)
	{
		puedeEjecutar = YES;
	}

	return puedeEjecutar;
}

void actualizar_Disponibles_Temporal(t_list* listaDisponiblesTemporal, t_list *listaDeRecursosGanados)
{
	int cantRecursosGanados = list_size(listaDeRecursosGanados);

	int numRecursoGanado = 0;

	t_recursosDisponibles *recursoDisponible = malloc(sizeof(t_recursosDisponibles));

	while(numRecursoGanado < cantRecursosGanados)
		{
		char* recursoGanado = malloc(sizeof(char));
		recursoGanado = list_get(listaDeRecursosGanados, numRecursoGanado);

		bool es_El_Mismo_Recurso(t_recursosDisponibles *ptr)
						{
							return strcmp(ptr->recurso_letra,recursoGanado) == 0;
						}

		recursoDisponible = list_remove_by_condition(listaDisponiblesTemporal, (void *) es_El_Mismo_Recurso);

		recursoDisponible->recurso_instancias++;
//TODO
		//printf("DISPONIBLE TEMPORAL: %d DEL RECURSO :%s\n", recursoDisponible->recurso_instancias, recursoDisponible->recurso_letra);

		bool es_El_Mismo_Recurso_test(t_recursosDisponibles *ptr)
								{
									return strcmp(ptr->recurso_letra,recursoGanado) == 0;
								}
	//	t_recursosDisponibles *test = list_find(listaDeRecursosDisponibles, (void *) es_El_Mismo_Recurso_test);

	//	printf("DISPONIBLE ORIGINAL: %d DEL RECURSO: %s\n", test->recurso_instancias, test->recurso_letra);

		list_add(listaDisponiblesTemporal, recursoDisponible);


		numRecursoGanado++;

		}

}

void marcar_Proceso(t_list *listaDeMarcados,char *simboloPersonaje)
{

	list_add(listaDeMarcados, simboloPersonaje);
}

void analizar_Interbloqueo(t_list *listaDeMarcados, t_list* listaPjsBloqueados)
{
	int cantPersonajes = list_size(listaPjsBloqueados);

	int numPersonaje = 0;

	t_personaje *personaje = malloc(sizeof(t_personaje));


	while(numPersonaje < cantPersonajes)
	{
		personaje = list_get(listaPjsBloqueados, numPersonaje);

		char* simboloPersonaje = malloc(strlen(personaje->simboloPersonaje) + 1);
		strcpy(simboloPersonaje, personaje->simboloPersonaje);


		int cont = 0;
		int cantidadMarcados = list_size(listaDeMarcados);
		int encontrado = FALSE;
		char* pjMarcado = malloc(sizeof(char));

		while(cont < cantidadMarcados && encontrado == FALSE)
		{
			pjMarcado = list_get(listaDeMarcados, cont);


			if( (strcmp(pjMarcado, simboloPersonaje) == 0) )
				encontrado = TRUE;
			else
				cont++;


		}



		if(encontrado == FALSE)
		{
			list_add(listaPersonajesBloqueados, simboloPersonaje);
		}

		numPersonaje++;

	}

}


t_list* filtrar_bloqueados()
{
	bool esta_bloqueado(t_personaje *ptr)
	{
		return ptr->bloqueado == TRUE;
	}

	t_list* listaDeBloqueados = list_filter(listaDePersonajes, (void *) esta_bloqueado);

	return listaDeBloqueados;


}
