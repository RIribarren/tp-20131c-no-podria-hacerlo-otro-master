/*
 * iniciarNivel.c
 *
 *  Created on: 16/05/2013
 *      Author: utnso
 */


#include "iniciarNivel.h"
#include <stdbool.h>



//se pasa como parametro la ip completa, devuelve el puertoo

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

t_nivel *iniciar_Nivel(t_config *config)
{
                t_nivel *nivel = malloc(sizeof(t_nivel));
                char *direccionIP_Puerto_Orq;
                char *direccionIP_Puerto_Local;

            //TODO
                	ListaItems = NULL;


                        nivel->niv_nombre = config_get_string_value(config, "nombre");

                        //separando "local" en ip y puerto
                        direccionIP_Puerto_Local = config_get_string_value(config,"local");


                        nivel->niv_ip = obtener_IP(direccionIP_Puerto_Local);
                        nivel->niv_puerto = obtener_Puerto(direccionIP_Puerto_Local);
                        nivel->niv_tiempoDeadlock = config_get_int_value(config,"TiempoChequeoDeadlock");
                        nivel->niv_recovery = config_get_int_value(config,"Recovery");


                        //separando "orquestador" en ip y puerto
                        direccionIP_Puerto_Orq = config_get_string_value(config, "orquestador");


                        nivel->ip_Orquestador = obtener_IP(direccionIP_Puerto_Orq);
                        nivel->puerto_orquestador = obtener_Puerto(direccionIP_Puerto_Orq);

                        nivel->cantidadDeCajas = config_get_int_value(config, "cantidadDeCajas");

                       //voy a crear una lista en la que cada nodo es un struct de una caja (nivel) con sus respectivos datos

                       nivel->listaDeCajas = list_create();
                       int cajasAgregadas = 1;
                       char* stringCaja;
                       char* stringCompleto;
                       while(cajasAgregadas <= nivel->cantidadDeCajas)
                       {
                    	   //agrega el indice correspondiente para crear el string Caja#, donde # es el numero de caja
                    	   stringCaja = agregar_Indice_Al_String(cajasAgregadas);

                    	   //ejemplo de stringCompleto: Flores,F,3,23,5
                    	   stringCompleto = config_get_string_value(config, stringCaja);

                    	   //hay que separar cada dato y meterlo en el struct del datosCaja
                    	   t_datosCaja *datosCaja = malloc(sizeof(t_datosCaja));

                    	   datosCaja = conseguir_Info_Caja(stringCompleto);

                    	   //agrego la caja a la lista de recursos disponibles
                    	   agregar_Lista_Recurso_Disponible(datosCaja);

  //   TODO   	   creando el escenario

                    	   char letra = datosCaja->caja_letra[0];

                    	   CrearCaja(&ListaItems, letra, datosCaja->caja_posx, datosCaja->caja_posy, datosCaja->caja_instancias);


                    	   //ahora agrego el struct a la lista en el lugar en donde indica el indice (cajasAgregadas)

                    	   list_add(nivel->listaDeCajas, datosCaja);


                    	   cajasAgregadas++;
                       }

                       // TODO
                       nivel_gui_dibujar(ListaItems);

        return nivel;
}

char* agregar_Indice_Al_String(int indice)
{

                   char* buffer = malloc(20);
                   memset(buffer, '\0', 20);

                   sprintf(buffer, "Caja%d", indice);


                   return buffer;

}

t_datosCaja *conseguir_Info_Caja(char* caja)
{

	t_datosCaja *infoCaja = malloc(sizeof(t_datosCaja));

	int i, j, k, l, m;
	int nroComas = 0;
	int comasTotales = 4;
	char* buffer = malloc(30);
	memset(buffer,'\0', 30);

	      while(nroComas < comasTotales)
	      {
	              for(i=0; caja[i] != ','; i++)
	              {
	                      buffer[i]= caja[i];
	              }

	              infoCaja->caja_nombre = buffer;

	              nroComas++;
	              int inicio = 0;
	              i++;
	              char* buffer2 = malloc(30);
	              memset(buffer2,'\0', 30);

	              for(j = i; caja[j] != ','; j++)
	              {
	                      buffer2[inicio] = caja[j];
	                      inicio++;
	              }

	              infoCaja->caja_letra = buffer2;

	              nroComas++;
	              j++;
	              inicio = 0;
	              char* buffer3 = malloc(30);
	              memset(buffer3,'\0', 30);

	              for(k = j; caja[k] != ','; k++)
	              {
	                   buffer3[inicio] = caja[k];
	                   inicio++;
	              }

	              int caja_instancias;
	              caja_instancias = atoi(buffer3);

	              infoCaja->caja_instancias = caja_instancias;

	              k++;
	              nroComas++;
	              inicio = 0;
	              memset(buffer3,'\0', 30);

	              for(l = k; caja[l] != ','; l++)
	              {
	                      buffer3[inicio] = caja[l];
	                      inicio++;
	              }

	              int caja_posx;
	              caja_posx = atoi(buffer3);

	             infoCaja->caja_posx = caja_posx;

	              l++;
	              nroComas++;
	              inicio = 0;
	              memset(buffer3,'\0', 30);

	              for(m = l; (caja[m] != ',' && caja[m] != '\0'); m++)
	              {
	                                      buffer3[inicio] = caja[m];
	                                      inicio++;
	              }

	              int caja_posy;
	              caja_posy = atoi(buffer3);

	              infoCaja->caja_posy = caja_posy;

	              nroComas++;


	      }

	      return infoCaja;
}


void agregar_Personaje(int socketPersonaje)
{
	t_personaje *personaje = malloc(sizeof(t_personaje));
	personaje->socketPersonaje = socketPersonaje;
	personaje->recursoQueBusca = '\0';
	personaje->listaDeRecursosGanados = list_create();
	personaje->bloqueado = FALSE;

	list_add(listaDePersonajes, personaje);
}

void agregar_Recurso(int socketPersonaje, char* recurso)
{
	t_personaje *personaje = malloc(sizeof(t_personaje));


	bool es_El_Mismo_Personaje(t_personaje *ptr)
			{
				return ptr->socketPersonaje == socketPersonaje;
			}

	//saco el personaje de la lista
	personaje = list_remove_by_condition(listaDePersonajes,(void *) es_El_Mismo_Personaje);

	char *recursoAux = malloc(strlen(recurso) + 1);
	strcpy(recursoAux, recurso);
	//agrego el recurso a la lista del nodo del personaje
	list_add(personaje->listaDeRecursosGanados, recursoAux);



	//vuelvo a agregar al personaje a la lista
	list_add(listaDePersonajes, personaje);


}

void agregar_Simbolo(int socketPersonaje,char* simboloPersonaje)
{
	t_personaje *personaje = malloc(sizeof(t_personaje));

	bool es_El_Mismo_Personaje(t_personaje *ptr)
			{
				return ptr->socketPersonaje == socketPersonaje;
			}


	//saco el personaje de la lista
	personaje = list_remove_by_condition(listaDePersonajes,(void *) es_El_Mismo_Personaje);

	//agrego el simbolo al personaje correspondiente
	personaje->simboloPersonaje = malloc(strlen(simboloPersonaje) + 1);
	strcpy(personaje->simboloPersonaje, simboloPersonaje);



	//vuelvo a agregar el personaje a la lista
	list_add(listaDePersonajes, personaje);

}

void dibujar_Nueva_Posicion(int socketPersonaje,t_destino *nuevaPos)
{
	t_personaje *personaje = malloc(sizeof(t_personaje));

		bool es_El_Mismo_Personaje(t_personaje *ptr)
				{
					return ptr->socketPersonaje == socketPersonaje;
				}


		//saco el personaje de la lista
		personaje = list_find(listaDePersonajes,(void *) es_El_Mismo_Personaje);

		char *simbolo = malloc(strlen(personaje->simboloPersonaje) + 1);
		strcpy(simbolo,personaje->simboloPersonaje);

	//	printf("ACA DIBUJO AL PERSONAJE: %s, al lugar x: %d e y: %d\n", simbolo, nuevaPos->x, nuevaPos->y);
//TODO
		char aux_simbolo = simbolo[0];

		//dibuja el movimiento
//TODO
		int posX, posY;

		posX = nuevaPos->x;
		posY = nuevaPos->y;

		MoverPersonaje(ListaItems,aux_simbolo, posX, posY);


		nivel_gui_dibujar(ListaItems);


}

void inicializar_Posicion(char* simboloPersonaje)
{
	//TODO
		char aux_simbolo = simboloPersonaje[0];

			//dibuja el movimiento
	//TODO
		CrearPersonaje(&ListaItems, aux_simbolo, 1 , 1);

		 nivel_gui_dibujar(ListaItems);

}

void agregar_Recurso_Buscado(int socketPersonaje,char* recurso)
{
	t_personaje *personaje = malloc(sizeof(t_personaje));

			bool es_El_Mismo_Personaje(t_personaje *ptr)
					{
						return ptr->socketPersonaje == socketPersonaje;
					}


			//saco el personaje de la lista
			personaje = list_remove_by_condition(listaDePersonajes,(void *) es_El_Mismo_Personaje);

			//agrego el recurso que busca
			personaje->recursoQueBusca = malloc(strlen(recurso) + 1);
			strcpy(personaje->recursoQueBusca, recurso);

			personaje->bloqueado = TRUE;

			//devuelvo el nodo a la lista
			list_add(listaDePersonajes, personaje);

}

void agregar_Lista_Recurso_Disponible(t_datosCaja *datosCaja)
{
	t_recursosDisponibles *recursoDisponible = malloc(sizeof(t_recursosDisponibles));

	recursoDisponible->recurso_letra = datosCaja->caja_letra;
	recursoDisponible->recurso_instancias = datosCaja->caja_instancias;

	//agrego a la lista de recursos disponibles
	list_add(listaDeRecursosDisponibles, recursoDisponible);

}

void quitar_Recurso_Buscado(int socketPersonaje, char* recurso)
{
	t_personaje *personaje = malloc(sizeof(t_personaje));

			bool es_El_Mismo_Personaje(t_personaje *ptr)
					{
						return ptr->socketPersonaje == socketPersonaje;
					}


			//saco el personaje de la lista
			personaje = list_remove_by_condition(listaDePersonajes,(void *) es_El_Mismo_Personaje);

			//agrego el recurso que busca


			//devuelvo el nodo a la lista
			list_add(listaDePersonajes, personaje);

}

int cantidad_Recursos_Ganados(int socketPersonaje)
{
	t_personaje *personaje = malloc(sizeof(t_personaje));
	int cantRecursosGanados;

	bool es_El_Mismo_Personaje(t_personaje *ptr)
					{
						return ptr->socketPersonaje == socketPersonaje;
					}


			//saco el personaje de la lista
			personaje = list_find(listaDePersonajes,(void *) es_El_Mismo_Personaje);


			cantRecursosGanados = list_size(personaje->listaDeRecursosGanados);

			return cantRecursosGanados;

}

char* simbolo_Personaje(int socketPersonaje)
{
	t_personaje *personaje = NULL;

	char* simboloPersonaje;

		bool es_El_Mismo_Personaje(t_personaje *ptr)
						{
							return ptr->socketPersonaje == socketPersonaje;
						}


				//saco el personaje de la lista
				personaje = list_find(listaDePersonajes,(void *) es_El_Mismo_Personaje);

			simboloPersonaje = personaje->simboloPersonaje;

			return simboloPersonaje;

}

t_personaje *buscar_Eliminar_Personaje(int socketPersonaje)
{


			bool es_El_Mismo_Personaje(t_personaje *ptr)
							{
								return ptr->socketPersonaje == socketPersonaje;
							}
			//saco el personaje de la lista
			t_personaje	*personaje = list_remove_by_condition(listaDePersonajes,(void *) es_El_Mismo_Personaje);

	return personaje;
}

char* eliminar_Recurso(t_list *listaDeRecursosGanados)
{
	char* recurso = list_remove(listaDeRecursosGanados, 0);

	return recurso;
}

void actualizar_Recursos(t_list *listaRecursosLibres, t_list* listaDeCajas)
{
	int cantRecursosLibres = list_size(listaRecursosLibres);
	int recursoLibre = 0;

	t_recursosDisponibles *recursoDisponible = malloc(sizeof(t_recursosDisponibles));
	t_datosCaja *datosCaja = malloc(sizeof(t_datosCaja));

	while(recursoLibre < cantRecursosLibres)
	{
		char* recurso = malloc(sizeof(char));
		recurso = list_remove(listaRecursosLibres, 0);

		//todo		printf("ACTUALIZO EL RECURSO LIBERADO: %s\n", recurso);

				bool es_El_Mismo_Recurso_Disponible(t_recursosDisponibles *ptr)
										{
											return strcmp(ptr->recurso_letra,recurso) == 0;
										}

				recursoDisponible = list_remove_by_condition(listaDeRecursosDisponibles, (void *) es_El_Mismo_Recurso_Disponible);

				recursoDisponible->recurso_instancias++;

		//todo		printf("AHORA TIENE: %d INSTANCIAS\n", recursoDisponible->recurso_instancias);

				list_add(listaDeRecursosDisponibles, recursoDisponible);


				bool es_El_Mismo_Recurso_DatosCaja(t_datosCaja *ptr)
												{
													return strcmp(ptr->caja_letra,recurso) == 0;
												}
				datosCaja = list_remove_by_condition(listaDeCajas, (void *) es_El_Mismo_Recurso_DatosCaja);

				datosCaja->caja_instancias++;

//todo				printf("datoscaja tiene ahora: %d\n", datosCaja->caja_instancias);

				//TODO parte grafica, ver si esto funca

				char simb = datosCaja->caja_letra[0];

				BorrarItem(&ListaItems, simb);

				CrearCaja(&ListaItems, simb , datosCaja->caja_posx, datosCaja->caja_posy, datosCaja->caja_instancias);

				nivel_gui_dibujar(ListaItems);


				list_add(listaDeCajas, datosCaja);

				recursoLibre++;
			}


}

void actualizar_lista_personajes(t_list *listaRecursosAsignados)
{
	int cantPjsAsignados = list_size(listaRecursosAsignados);
	int pjAsignado = 0;
	t_recursos_asignados *recursoAsignado = malloc(sizeof(t_recursos_asignados));

	while(pjAsignado < cantPjsAsignados)
	{
		recursoAsignado = list_remove(listaRecursosAsignados, 0);

		asignar_Recurso_Personaje_Asignado(recursoAsignado);

		pjAsignado++;

	}

}

void asignar_Recurso_Personaje_Asignado(t_recursos_asignados *recursoAsignado)
{
	char *simboloPersonaje = malloc(strlen(recursoAsignado->simboloPj) + 1);
	strcpy(simboloPersonaje,recursoAsignado->simboloPj);

	char *recurso = malloc(strlen(recursoAsignado->recursoAsignado) + 1);
	strcpy(recurso,recursoAsignado->recursoAsignado);

	t_personaje *personaje = malloc(sizeof(t_personaje));

	bool es_El_Mismo_Personaje(t_personaje *ptr)
								{
									return strcmp(ptr->simboloPersonaje, simboloPersonaje) == 0;
								}

	personaje = list_remove_by_condition(listaDePersonajes,(void *) es_El_Mismo_Personaje);

	personaje->recursoQueBusca = '\0';

	personaje->bloqueado = FALSE;

	list_add(personaje->listaDeRecursosGanados, recurso);

	list_add(listaDePersonajes, personaje);

}
