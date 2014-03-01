/*
 * planificador.c
 *
 *  Created on: 17/05/2013
 *      Author: utnso
 */


#include "planificador.h"

/*
static void personaje_Destroy(t_datos_pj *self){
	free(self->simboloPj);
	free(self);
}*/


void *funcionPlanificador(char* nombreNivel){

	int socket_personaje;
	int socket_planificador;
	char* recurso_personaje;
	char* simbolo_personaje;
	int actividad,timeout, tamanio_actual,cantMovimientos=0;
	struct pollfd *fds=NULL;
	int i;
	int nfds;
	int cliente_nuevo;
	struct sockaddr_in infoSocketServidor;
	socklen_t longitud_del_cliente;
	int ya_saque_uno = TRUE;
	int estaBloqueado= FALSE;
	int esta_vacia= TRUE;
	char* simboloOnline;
	int quantumAux = quantum;
	int alguienJugando = FALSE;

        //Se encuentra permanentemente atento a conexiones de personajes entrantes y actua sobre ellos.
	/*
	tenemos por ejemplo 5 niveles
	tenemos 5 hilos planif, que usan la misma funcion
	tenemos que abrir 5 sockets que hagan listen por conexiones de personajes en el puerto correspondiene al nivel
	con un poll, un fds posicion 0 esta el socket haciendo listen
	si hay actividad en el socket haciendo listen, aceptar las conexiones y guardarlas en el fds.
	A) **mutex_lock** sacamos un personaje de la cola, su socket, y si esta en fds ( o sea ya lo aceptamos ), le mandamos "mov permitido"
	acto seguido esperamos la respuesta de ese socket, o sea, el turno concluido
	lo guardamos en la cola de listo o bloqueados segun corresponda **mutex_unlock**, hace un sleep(n), y seguimos con el siguiente perosnaje de la cola de listo
	si el socket esta en fds se repite A)
	*/

	t_niveles *nodoDelNivel = buscar_Nivel(listaDeNiveles, nombreNivel);
	int nroPuerto = nodoDelNivel->puerto_Planif;
//	printf("------------------------------------------------------\n");
	//printf("Planificador del %s, su puerto es: %d. Esperando por Personajes!\n",nombreNivel,nodoDelNivel->puerto_Planif);

	socket_planificador = abrir_conexion_servidor(nroPuerto, BACKLOG);


	nfds=1;
	fds = (struct pollfd *)calloc(1,nfds*sizeof(struct pollfd));
	fds->fd =socket_planificador;
	fds->events = POLLIN;
	timeout = -1;
	t_cola *primerNodo = malloc(sizeof(t_cola));

	while (1)
	{
			if(cantMovimientos==quantum || cantMovimientos == 0)
			{
			quantumAux=quantum;
			}

	//		printf("Waiting on poll planificador %s...\n",nombreNivel);
	  //      printf("nfds = %d\n",nfds);
	        //esperando alguna actividad de algun socket
	        actividad = poll(fds, nfds, timeout);

	        //error en el poll
	        if (actividad < 0)
	        {
	                perror("  poll() failed");
	                break;
	        }

	        //finalizacion por timeout
	        if (actividad == 0)
	        {
	                printf("  poll() timed out.  End program.\n");
	                break;
	        }

	        pthread_mutex_lock( &mutex1 );
	 //       printf("--------SIZE DE LA LISTA DE LISTAS ANTES-------------\n hay: %d NODOS en ListaDeListas\n", list_size(listaDeListas));
	        t_listas *nodoDelNivel = buscar_Mismo_Nivel(nombreNivel);
	        int valor = list_is_empty(nodoDelNivel->listaListos);
	        if(valor==0)
	        {
	        	esta_vacia =FALSE;
	        }
	        else
	        {
	        	esta_vacia =TRUE;
	        }
	        list_add(listaDeListas, nodoDelNivel);
	        pthread_mutex_unlock( &mutex1 );

	        //sacando el proximo
	        if (ya_saque_uno==FALSE && cantMovimientos == 0 && esta_vacia== FALSE)
	      	       {

	        		 //sacamos al 1° personaje de la cola
	      	     	 //saco el nodo del nivel, con las colas
	      	//     	 printf("--------SIZE DE LA LISTA DE LISTAS ANTES-------------\n hay: %d NODOS en ListaDeListas\n", list_size(listaDeListas));

	      	     	 pthread_mutex_lock( &mutex1 );
	      	     	 t_listas *nodoDelNivel = buscar_Mismo_Nivel(nombreNivel);


	      	     	 primerNodo = list_remove(nodoDelNivel->listaListos,0);

	      	     	 mostrar_Lista(nodoDelNivel, nombreNivel);

	     // 	     	 printf("----------------------------\n");
	    //  	     	 printf("El primer pj es: %d ", primerNodo->socketPj);
	      //	     	 printf("y su recurso: %s\n", primerNodo->recursoPj);

	      	     	 //devuelvo el nodoDelNivel a la lista global

	      	     	 socket_personaje = primerNodo->socketPj;
	      	     	 recurso_personaje = primerNodo->recursoPj;
	      	     	 simbolo_personaje = primerNodo->simboloPj;

	      	     	 log_info(logger, "Turno otorgado al personaje: %s en el %s", simbolo_personaje, nombreNivel);

	      	     	 list_add(listaDeListas, nodoDelNivel);
	      	     	 pthread_mutex_unlock( &mutex1 );

	      	//     	 printf("--------SIZE DE LA LISTA DE LISTAS DPS-------------\n hay: %d NODOS en listaDeListas\n", list_size(listaDeListas));

	      	     	 alguienJugando = TRUE;
	      	     	 ya_saque_uno=TRUE;
	      	       }

	        tamanio_actual = nfds;

	        for(i=0; i < tamanio_actual; i++){

	        		//si no hay ningun revent, se vuelve al sgte ciclo del for
	                if(fds[i].revents == 0)
	                        continue;

	                //no se detecta la actividad esperada
	                if(fds[i].revents != POLLIN)
	                {
	                	//if(fds[i].revents != POLLOUT)
	                	//{
	                        printf("  Error! revents = %d\n", fds[i].revents);
	                        break;
	                	//}
	                }




	                if((fds+i)->events==POLLIN && (fds+i)->fd == socket_personaje)
	                {
	                	t_paquete *paquete = recibir_Paquete(fds[i].fd);

	                	switch(paquete->id)
	                	{

	                		case ME_QUIERO_MOVER:
	                		{
	                	//		if(cantMovimientos < quantumAux)
	                		//	{
	                			//el personaje se quiere mover
	                			free(paquete);

	             //   			printf("el personaje se quiere mover\n");

	                			alguienJugando = TRUE;

	                			avisar_Movimiento_Permitido(primerNodo->socketPj);

	                			paquete = recibir_Paquete(fds[i].fd);
	                			switch(paquete->id)
	                				{
	                				case TURNO_CONCLUIDO:
	                				{
	                					//espera saber si el personaje se encuentra bloqueado o no
	                					if(strcmp(paquete->data, "LISTO") == 0)
	                					{
	                	//					printf("personaje listo \n");
	                						cantMovimientos++;

	                						if(cantMovimientos==quantumAux)
	                						{
	                						t_cola *personaje = malloc(sizeof(t_cola));
	                						personaje->recursoPj = '\0';
	                						personaje->socketPj = socket_personaje;

	                						personaje->simboloPj = malloc(strlen(simbolo_personaje) + 1);
	                						strcpy(personaje->simboloPj,simbolo_personaje);

	                						pthread_mutex_lock( &mutex1 );
	                						t_listas *nodoDelNivel = buscar_Mismo_Nivel(nombreNivel);

	                						list_add(nodoDelNivel->listaListos, personaje);

	                						list_add(listaDeListas, nodoDelNivel);
	                						pthread_mutex_unlock( &mutex1 );

	                						alguienJugando = FALSE;
	                						cantMovimientos=0;

	                						}

	                					}
	                					if(strcmp(paquete->data, "LISTO CON RECURSO") == 0)
	                					{
	                			//			printf("personaje listo con recurso \n");
	                						cantMovimientos = quantumAux;

	                						if(cantMovimientos==quantumAux)
	                							{
	                							t_cola *personaje = malloc(sizeof(t_cola));
	                							personaje->recursoPj = '\0';
	                							personaje->socketPj = socket_personaje;

	                							personaje->simboloPj = malloc(strlen(simbolo_personaje) + 1);
	                							strcpy(personaje->simboloPj,simbolo_personaje);

	                							pthread_mutex_lock( &mutex1 );
	                							t_listas *nodoDelNivel = buscar_Mismo_Nivel(nombreNivel);

	                							//cambiarle el campo "BLOQUEADO" a FALSE
	                							cambiar_Estado_Bloqueado(socket_personaje, simbolo_personaje, nombreNivel);

	                							list_add(nodoDelNivel->listaListos, personaje);

	                							list_add(listaDeListas, nodoDelNivel);
	                							pthread_mutex_unlock( &mutex1 );

	                							alguienJugando = FALSE;
	                							cantMovimientos=0;

	                							}

	                					}
	                					else if(strcmp(paquete->data, "BLOQUEADO") == 0)
	                					{

	                						free(paquete);
	                						paquete=recibir_Paquete(fds[i].fd);

	                		//				printf("personaje bloqueado \n");
	                						estaBloqueado=TRUE;
	                						t_cola *personaje = malloc(sizeof(t_cola));

	                						personaje->recursoPj = malloc(strlen(paquete->data) + 1);
	                						strcpy(personaje->recursoPj,paquete->data);
	                						personaje->socketPj = socket_personaje;

	                						personaje->simboloPj = malloc(strlen(simbolo_personaje) + 1);
	                						strcpy(personaje->simboloPj,simbolo_personaje);

	                						pthread_mutex_lock( &mutex1 );
	                						t_listas *nodoDelNivel = buscar_Mismo_Nivel(nombreNivel);

	                						list_add(nodoDelNivel->listaBloqueados, personaje);
	                						list_add(listaDeListas, nodoDelNivel);


	                						//preparo el nodo para actualizar el estado en la listaDePjsJugando
	                						t_datos_pj *nodoPj = malloc(sizeof(t_datos_pj));
											nodoPj->socketPj = fds[i].fd;

											nodoPj->simboloPj = malloc(strlen(simbolo_personaje) + 1);
											strcpy(nodoPj->simboloPj,simbolo_personaje);
											nodoPj->bloqueado = TRUE;

											//inicio contador, y nodos
											int cont =0;
											int es_el_mismo=FALSE;
											int index;
											t_niveles *nivel = buscar_Nivel_De_Lista_Niveles(listaDeNiveles, nombreNivel);
											t_datos_pj *nodoPjEnJuego = malloc(sizeof(t_datos_pj));

											while(es_el_mismo==FALSE)
											{
												nodoPjEnJuego = list_get(nivel->listaDePjsJugando,cont);
												if(strcmp(nodoPjEnJuego->simboloPj, simbolo_personaje) == 0)
												{
													es_el_mismo=TRUE;
													index=cont;
												}
												else{cont++;}
											}

											t_datos_pj *aux = list_replace(nivel->listaDePjsJugando,index,nodoPj);
											free(aux);
											free(aux->simboloPj);
											list_add(listaDeNiveles, nivel);

											pthread_mutex_unlock( &mutex1 );

	                					}
	                					usleep(tiempoDeEspera);
	                					if(estaBloqueado==TRUE)
	                					{
	                						alguienJugando = FALSE;
	                						estaBloqueado=FALSE;
	                						cantMovimientos=0;
	                					}

	                					break;
	                				}//cierro case TURNO_CONCLUIDO

	                				default: {printf("error, no se recibe: turno concluido");}


	                			}//cierro switch interno
//	                			}


	                			free(paquete);
//	                		}


	                			break;
	                		}//cierro case ME_QUIERO_MOVER
	                		case DESCONEXION:
	                		{
	                			//if(strcmp(paquete->data, "Termine") == 0)
	                			//{
	                			pthread_mutex_lock( &mutex1 );
	               // 				printf("Se desconecto el socket %d\n",fds[i].fd);
	                			//sacamos al Pj de la listaDePjsJugando
	                				cantMovimientos = 0;
	                				int cont =0;
									int es_el_mismo=FALSE;
									int index;
									//TODO mutex

									t_niveles *nivel = buscar_Nivel_De_Lista_Niveles(listaDeNiveles, nombreNivel);
									t_datos_pj *nodoPjEnJuego = malloc(sizeof(t_datos_pj));

									while(es_el_mismo==FALSE)
									{
										nodoPjEnJuego = list_get(nivel->listaDePjsJugando,cont);
										if(nodoPjEnJuego->socketPj == fds[i].fd)
										{
											es_el_mismo=TRUE;
											index=cont;
										}
										else{cont++;}
									}



	                			//list_remove_and_destroy_element(nivel->listaDePjsJugando, index,(void*) personaje_Destroy);

									t_datos_pj *pj = list_remove(nivel->listaDePjsJugando, index);

									simboloOnline = malloc(strlen(pj->simboloPj) + 1);
									strcpy(simboloOnline,pj->simboloPj);

									if(strcmp(paquete->data,"Juego terminado") == 0)
										               {
										                	sacar_Pj_Online(simboloOnline);
										                }

	                	//			printf("CANTIDAD PJS JUGANDO AHORA: %d\n", list_size(nivel->listaDePjsJugando));

	                				bool es_El_Mismo_Socket(t_cola* ptr)
	                				{
	                					  return fds[i].fd == ptr->socketPj;
	                				}


	                				t_listas *nodoNivel = buscar_Mismo_Nivel(nombreNivel);

	                				t_cola* pjADesconectar = list_remove_by_condition(nodoNivel->listaDesconexiones, (void *) es_El_Mismo_Socket);

	                				list_add(listaDeListas, nodoNivel);


	                				list_add(listaDeNiveles,nivel);
	                				pthread_mutex_unlock( &mutex1 );
	                				//envia un OK
	                				free(paquete);

	                				paquete = crear_Paquete(CONFIRMACION_DESCONEXION, "OK", strlen("OK") + 1);
	                				enviar_Paquete(socket_personaje, paquete);


									  log_info(logger,"Se desconecto el Personaje: %s del planificador del %s", pj->simboloPj,nombreNivel);


	                				close(fds[i].fd);
	                				fds[i].fd = -1;

	                				/* esto "compacta" el array, quedando agrupados los sockets que siguen conectados o sin error, y ultimos
	                				 * los que dan "-1", entonces al hacer el realloc de una "unidad" menos, se come al ultimo, o sea, al que ya no
	                				 * nos sirve */
	                				int j;
	                				for (i = 0; i < nfds; i++)
	                				{
	                					if (fds[i].fd == -1)
	                					{
	                						for(j = i; j < nfds; j++)
	                						{
	                							fds[j].fd = fds[j+1].fd;
	                						}
	                						nfds--;
	                					}
	                				}
	                				fds = (struct pollfd *)realloc(fds,nfds*sizeof(struct pollfd));

	                				alguienJugando = FALSE;


	                				free(paquete);

	                			//}
	                			break;
	                		}//cierro case DESCONEXION
	                		default: {printf("comportamiento no esperado\n");}
	                	}//cierro switch

	                	if(alguienJugando == FALSE)
	                		ya_saque_uno = FALSE;


	                }//cierro if((fds+i)->events==POLLIN && (fds+i)->fd==socket_personaje)

	                if((fds+i)->fd != socket_planificador && (fds+i)->fd != socket_personaje && (fds+i)->fd > 0)
	                {
	                	bool es_El_Mismo_Socket(t_cola* ptr)
	                	{
	                		return fds[i].fd == ptr->socketPj;
	                	}

	                	pthread_mutex_lock( &mutex1 );
	                	t_listas *nodoNivel = buscar_Mismo_Nivel(nombreNivel);


	                	t_cola* pjADesconectar = list_remove_by_condition(nodoNivel->listaDesconexiones, (void *) es_El_Mismo_Socket);

	                	if(pjADesconectar != NULL)
	                	{
	                		t_paquete *paquete = recibir_Paquete(fds[i].fd);

	                		if( paquete->id == DESCONEXION)
	                		{


	                //		printf("Se desconecto el socket %d\n",fds[i].fd);
						//sacamos al Pj de la listaDePjsJugando

							//TODO mutex

							t_niveles *nivel = buscar_Nivel_De_Lista_Niveles(listaDeNiveles, nombreNivel);



							bool es_El_Mismo_Socket_Jugando(t_datos_pj* ptr)
								                	{
								                		return fds[i].fd == ptr->socketPj;
								                	}

						//list_remove_and_destroy_element(nivel->listaDePjsJugando, index,(void*) personaje_Destroy);

							t_datos_pj *pj = list_remove_by_condition(nivel->listaDePjsJugando, (void *) es_El_Mismo_Socket_Jugando);

							simboloOnline = malloc(strlen(pj->simboloPj) + 1);
							strcpy(simboloOnline,pj->simboloPj);

							if(strcmp(paquete->data,"Juego terminado") == 0)
											   {
													sacar_Pj_Online(simboloOnline);
												}

			//				printf("CANTIDAD PJS JUGANDO AHORA: %d\n", list_size(nivel->listaDePjsJugando));
							list_add(listaDeNiveles,nivel);

							//envia un OK
						//	free(paquete);

							paquete = crear_Paquete(CONFIRMACION_DESCONEXION, "OK", strlen("OK") + 1);
							enviar_Paquete(fds[i].fd, paquete);




							  log_info(logger,"Se desconecto el Personaje: %s del planificador del nivel: %s", pj->simboloPj,nombreNivel);



							close(fds[i].fd);
							fds[i].fd = -1;

							/* esto "compacta" el array, quedando agrupados los sockets que siguen conectados o sin error, y ultimos
							 * los que dan "-1", entonces al hacer el realloc de una "unidad" menos, se come al ultimo, o sea, al que ya no
							 * nos sirve */
							int j;
							for (i = 0; i < nfds; i++)
							{
								if (fds[i].fd == -1)
								{
									for(j = i; j < nfds; j++)
									{
										fds[j].fd = fds[j+1].fd;
									}
									nfds--;
								}
							}
							fds = (struct pollfd *)realloc(fds,nfds*sizeof(struct pollfd));




							//free(paquete);

							}
	                		}

	                	list_add(listaDeListas, nodoNivel);
	                	pthread_mutex_unlock( &mutex1 );
					}

	                if((fds+i)->fd==socket_planificador)
	                {
	      //          printf(" Socket de escucha recibiendo nuevas conexiones\n");


	                    do {
	                          longitud_del_cliente = sizeof(infoSocketServidor);

	                          //agrego "una unidad de estructura" para el siguiente socket que se va a conectar
	                          fds =(struct pollfd *) realloc (fds,(nfds+1)*sizeof(struct pollfd));

	                          cliente_nuevo = aceptar_cliente(socket_planificador,longitud_del_cliente);

	                          //error
	                          if (cliente_nuevo < 0)
	                          {
	                        	  if (errno != EWOULDBLOCK)
	                        	  {
	                        		  perror("  accept() failed");
	                           }

	                        	  //elimino esa "unidad de estructura"
	                        	  fds = (struct pollfd *)realloc(fds,nfds*sizeof(struct pollfd));
	                        	  break;
	                          }

	                          //agrego el nuevo socket
	                          fds[nfds].fd = cliente_nuevo;
	                          fds[nfds].events = POLLIN;
	               //           printf("Se conecto un personaje al planificador!, su socket es: %d\n", fds[nfds].fd);

	                          t_paquete *paquete = recibir_Paquete(fds[nfds].fd);
	                          //Handshake personaje-planificador
								 if((strcmp(paquete->data, "Soy un personaje") == 0))
								 {
					//				printf("handshake con el personaje %d correcto\n",(fds+nfds)->fd);

									free(paquete);
									paquete = recibir_Paquete(fds[nfds].fd);
									t_cola *personaje = malloc(sizeof(t_cola));
									personaje->recursoPj = '\0';
									personaje->socketPj = fds[nfds].fd;

									personaje->simboloPj = malloc(strlen(paquete->data) + 1);
									strcpy(personaje->simboloPj,paquete->data);

					//				printf("El pj que acaba de hacer handshake es: %s\n",personaje->simboloPj);

									 simboloOnline = malloc(strlen(paquete->data) + 1);
									 strcpy(simboloOnline,paquete->data);

									agregar_Pj_A_Online(simboloOnline);


									pthread_mutex_lock( &mutex1 );
									t_listas *nodoDelNivel = buscar_Mismo_Nivel(nombreNivel);

									//agregamos el Pj a la listaDePjsJugando
									t_datos_pj *nodoPj = malloc(sizeof(t_datos_pj));
									nodoPj->socketPj = fds[nfds].fd;

									nodoPj->simboloPj = malloc(strlen(paquete->data) + 1);
									strcpy(nodoPj->simboloPj,paquete->data);

									nodoPj->bloqueado = FALSE;



									  log_info(logger,"Se conecto el Personaje: %s al planificador del nivel: %s", nodoPj->simboloPj,nombreNivel);


									//inicio region critica
									//pthread_mutex_lock( &mutex1 );
									t_niveles *nivel = buscar_Nivel_De_Lista_Niveles(listaDeNiveles, nombreNivel);

									list_add(nivel->listaDePjsJugando,nodoPj);
									list_add(listaDeNiveles, nivel);
									//fin region critica
									//pthread_mutex_unlock( &mutex1 );
									list_add(nodoDelNivel->listaListos, personaje);
									list_add(listaDeListas, nodoDelNivel);
									pthread_mutex_unlock( &mutex1 );

									free(paquete);
								 }


	                          nfds++;

	                          if(alguienJugando == FALSE)
	                          ya_saque_uno = FALSE;


	                  	  }while (cliente_nuevo != -1);
	                }

	                //Recorre cola de listos secuencialmente
	                      //Envía "Movimiento Permitido"
	                      //Recibe "Turno concluido"
	                      //Espera un tiempo predefinido para próximo personaje

	                      //sacamos al 1° personaje de la cola
	                     // t_cola *cola = queue_pop(colaListos);
	                     // socket_personaje = cola->socketPj;
	                     // recurso_personaje = cola->recursoPj;
	                     // personaje_destroy(cola);




	}//fin del for
	}//fin del while (1)
	return 0;
}
