 /*
 * orquestador.c
 *
 *  Created on: 17/05/2013
 *      Author: utnso
 */


#include "orquestador.h"
#include "mensajesPlataforma.h"




#define EVENT_SIZE  ( sizeof (struct inotify_event) + 24 )
#define BUF_LEN     ( 1024 * EVENT_SIZE )

void *funcionOrquestador (void *ptr)
{
	int socket_servidor,cliente_nuevo;
    struct sockaddr_in infoSocketServidor;
    socklen_t longitud_del_cliente;
    int nroPuerto;
    int i, n=0;
    int actividad,timeout, tamanio_actual;
    struct pollfd *fds=NULL;
    int nfds;
    char buffer[BUF_LEN];





    //caso para "./personaje localhost puerto"
    /*if (argc < 2)
    {
                perror("Error, no se ingreso el puerto\n");
                exit(-1);
        }
        else
                printf("Puerto ingresado correctamente, servidor online!\n");*/




        nroPuerto = 5000;
        //nroPuerto = atoi(argv[1]); //Asigna a nroPuerto el primer parametro puesto en consola.

        //Haciendo el socket, bind y listen
        socket_servidor = abrir_conexion_servidor(nroPuerto, BACKLOG);

        // Al inicializar inotify este nos devuelve un descriptor de archivo
        int file_descriptor = inotify_init();
        if (file_descriptor < 0) {
                perror("inotify_init");
        }

        // Creamos un monitor sobre un path indicando que eventos queremos escuchar
        int watch_descriptor = inotify_add_watch(file_descriptor, "/home/utnso/workspace/tp-20131c-no-podria-hacerlo-otro/plataforma", IN_MODIFY);



        //Aceptando los clientes, setendo flags para lectura

        nfds=2;
        fds = (struct pollfd *)calloc(1,nfds*sizeof(struct pollfd));
        fds->fd =socket_servidor;
        fds->events = POLLIN;
        (fds+1)->fd = file_descriptor;
        (fds+1)->events = POLLIN;
        timeout = 1000;

        primeraVez = TRUE;

     do
     {
/*
        printf("Waiting on poll()...\n");
        printf("nfds = %d\n",nfds);
        printf("quantum:%d\n",quantum);
  */      //esperando alguna actividad de algun socket
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
        	if(primeraVez ==TRUE)
                continue;
        	if(list_size(pjsOnline) > 0)
        		continue;
        	else
        	break;
        }

        tamanio_actual = nfds;

        for(i=0; i < tamanio_actual; i++){

                //si no hay ningun revent, se vuelve al sgte ciclo del for
                if(fds[i].revents == 0)
                        continue;

                //no se detecta la actividad esperada
                if(fds[i].revents != POLLIN)
                {
                        printf("  Error! revents = %d\n", fds[i].revents);
                       break;
                }

                if((fds+i)->fd==file_descriptor)
                {
                	// printf("Viejo valor del Quantum:%d\n",quantum);

                	 int length = read(file_descriptor, buffer, BUF_LEN);
                	 //sleep(2);
                	 config = config_create("/home/utnso/workspace/tp-20131c-no-podria-hacerlo-otro/plataforma/configPlataforma");
                	 quantum = config_get_int_value(config, "quantum");
                	// printf("El nuevo valor del quantum:%d\n", quantum);

                     if (length < 0)
                     {
                             perror("read");
                     }

                     int offset = 0;


                }

                //el primero socket tiene que ser si o si del servidor, para inciar las escuchas
                if((fds+i)->fd==socket_servidor)
                {
                  //      printf("  Listening socket is readable\n");


                        do {
                                longitud_del_cliente = sizeof(infoSocketServidor);

                                //agrego "una unidad de estructura" para el siguiente socket que se va a conectar
                                fds =(struct pollfd *) realloc (fds,(nfds+1)*sizeof(struct pollfd));

                                cliente_nuevo = aceptar_cliente(socket_servidor,longitud_del_cliente);

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
                                nfds++;
                         //       printf("Se conecto un cliente\n");

                        }while (cliente_nuevo != -1);
                }
                if((fds+i)->fd!=socket_servidor && (fds+i)->fd!=file_descriptor)
                {


                //}

                        //Handshake
                        /*Recibe un paquete del cliente y, dependiendo del paquete.data, el orquestador sabrá si
                                es un pj o un nivel, por lo que deberá decidir que acciones llevar a cabo*/

                        t_paquete *paquete = recibir_Paquete(fds[i].fd);

                        switch(paquete->id)
                        {
                        case HANDSHAKE:
                        {
                                //envio de mensajes con un personaje
                                if((strcmp(paquete->data, "Soy un personaje") == 0))
                                {
                                    //    printf("Handshake del personaje aceptado\n");

                                        destroy_paquete(paquete);
                                        paquete = recibir_Paquete(fds[i].fd);




                                        if(paquete->id == PEDIR_INFO_NIVEL_PLAN)
                                        {
                                        	pthread_mutex_lock(&mutex1);
                                 //       		printf("A punto de enviar los datos del %s al socket: %d\n",paquete->data,fds[i].fd);
                                        		t_niveles *nodoDelNivel = buscar_Nivel(listaDeNiveles, paquete->data);

                                        		char *nombreNivel = malloc(strlen(paquete->data) + 1);
                                        		strcpy(nombreNivel, paquete->data);


												log_info(logger,"Se conecto un Personaje al orquestador. Para pedir la info del %s",nombreNivel);



                                        		enviar_Datos_Nivel(nodoDelNivel,fds[i].fd);

                                        		destroy_paquete(paquete);

                                        		pthread_mutex_unlock(&mutex1);

                                                //el personaje se desconecta, hay que cerrar el socket
                                                //y reallocar la memoria de las structs, se pone valor -1 para que el prox for detecte que ya no sirve.
                                        		log_info(logger,"Se desconecto un Personaje del orquestador. Ya que obtuvo la info del %s", nombreNivel);

                                        		close(fds[i].fd);
                                                fds[i].fd = -1;
                                                /* esto "compacta" el array, quedando agrupados los sockets que siguen conectados o sin error, y ultimos
                                                 * los que dan "-1", entonces al hacer el realloc de una "unidad" menos, se come al ultimo, o sea, al que ya no
                                                 * nos sirve */
                                                int j;
                                                int i;
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



                                        }else if(paquete->id == RESPUESTA_ORQUESTADOR)
                                        {
                                        	char* simbolo = malloc(strlen(paquete->data) + 1);

                                        	strcpy(simbolo,paquete->data);

                                        	t_personajeOrquestador *personajeOrquestador = malloc(sizeof(t_personajeOrquestador));

                                        	personajeOrquestador->socketPersonaje = fds[i].fd;

                                        	personajeOrquestador->simboloPersonaje = malloc(strlen(simbolo) + 1);
                                        	strcpy(personajeOrquestador->simboloPersonaje,simbolo);


											log_info(logger,"Se conecto un el personaje: %s al orquestador. Para esperar la respuesta del orquestador", simbolo);


                                        	list_add(listaPersonajesOrquestador, personajeOrquestador);

                                        	destroy_paquete(paquete);
                                        }

                                }

                                        //envio de mensajes con un nivel
                                else if((strcmp(paquete->data, "Soy un nivel") == 0))
                                {
                                        //aceptamos handshake y recibimos su paquete
                                //        printf("Handshake del nivel aceptado\n");
                                        destroy_paquete(paquete);


                                        paquete = recibir_Paquete(fds[i].fd);

                                        //controlamos si lo que nos pide son los datos del nivel y se los enviamos
                                        if(paquete->id == DATOS_DEL_NIVEL)
                                        {

                                        		//inicializo cola de listos, bloqueados y su correspondiente nombre del nivel que se conecta
                                        		t_info_datos_nivel *info_datos_nivel = inicializar_Listas_Nivel(paquete);

                                                //obtener nivel devuelve un Int igual al nivel, por ej: nivel3, entonces obtener_nivel devuelve 3
                                                //y entonces el puerto que usaremos para el nivel3 es el puerto 5003, quedando ordenado.
                                        		int puerto;

                                        		if(strcmp(info_datos_nivel->nombre_Nivel, "Deadlock") == 0)
                                        			puerto = nroPuerto + 1;
                                        		else
                                        		    puerto = nroPuerto + obtener_Nivel(info_datos_nivel->nombre_Nivel);



													log_info(logger,"Se conecto el %s al orquestador. Para pasarle sus datos.",info_datos_nivel->nombre_Nivel);


                                                //agregamos el nivel a la lista de niveles
                                                t_niveles *nivel = agregar_Nivel_A_Lista(info_datos_nivel,puerto);

                                             	//ver si es necesario o de utilidad guardar las id de los hilos planif en un array(que encima deberia ser dinamico)TODO
                                                pthread_t idHilos[n];

                                                if( (idHilos[n]=pthread_create( &idHilos[n], NULL, funcionPlanificador, nivel->nombre_Nivel)) )
                                                {
                                                        printf("Falló la creacion del hilo: %d\n", n);
                                                }
                                                n +=1;


                                                destroy_paquete(paquete);


                                        }


                                }else
                                {
                                        printf("Error en el Handshake\n");
                                        exit(1);
                                        free(paquete);
                                }
                                break;
                        }
                        case RECURSOS_LIBERADOS:
                        {
                        	size_t longitud_datos = paquete->size_data;
                        	char* datos_serializados = paquete->data;
                        	t_datosParaRecursosLiberados *datosRecursosDeserializados = deserializar_Info_Recursos_Liberados(datos_serializados, longitud_datos);

                        	t_datosParaRecursosLiberados *datosRecursos = malloc(sizeof(t_datosParaRecursosLiberados));

                        	datosRecursos->simboloPersonaje = malloc(strlen(datosRecursosDeserializados->simboloPersonaje) + 1);
                        	strcpy(datosRecursos->simboloPersonaje, datosRecursosDeserializados->simboloPersonaje);

                        	datosRecursos->nombreNivel = malloc(strlen(datosRecursosDeserializados->nombreNivel) + 1);
                        	strcpy(datosRecursos->nombreNivel, datosRecursosDeserializados->nombreNivel);

                        	datosRecursos->listaRecursosLibres = datosRecursosDeserializados->listaRecursosLibres;

                        	free(datosRecursosDeserializados->nombreNivel);
                        	free(datosRecursosDeserializados->simboloPersonaje);
                        	free(datosRecursosDeserializados);

/*
                        	printf("EL PERSONAJE QUE SE FUE ES: %s\n",datosRecursos->simboloPersonaje);
                        	printf("SE FUE DEL NIVEL: %s\n", datosRecursos->nombreNivel);


                        	printf("CANTIDAD DE NODOS TOTALES LIBERADOS POR EL PJ: %d\n", nodosTotales);
*/
                        	int cantNodos = 0;
                        	int nodosTotales = list_size(datosRecursos->listaRecursosLibres);


                        	//preparo la estructura: structRecursos, e inicializo sus listas
                        	//nota: Esto se hace cada vez que el Orquestador recibe recursos liberados.
                        	t_lista_recursos *structRecursos = malloc(sizeof(t_lista_recursos));
                        	structRecursos->listaRecursosAsignados = list_create();
                        	structRecursos->listaRecursosLibres = list_create();
                        	//remuevo el nodo de la listaDeListas que cumpla con el nivel de donde vienen los recursos

                        	//inicio region critica
                        	pthread_mutex_lock( &mutex1 );
                        	t_listas *nodoDeListas = buscar_Mismo_Nivel(datosRecursos->nombreNivel);

                        	//minetras haya recursos por asignar..
                        	while(cantNodos < nodosTotales)
                        	{
                        		//remuevo el primer recurso libre
                        		char *recursoActual = malloc(sizeof(char));
                        		recursoActual = list_remove(datosRecursos->listaRecursosLibres,0);

                        		//t_cola *nodoPj = malloc(sizeof(t_cola));
                        		//remuevo el pj de lista de bloqueados tal que necesita el recurso
                        		t_cola *nodoPj = buscar_Pj_Que_Solicita_Recurso(nodoDeListas->listaBloqueados, recursoActual);

                        		//agrego segun corresonda, el recurso como otorgado a un pj, o como libre
                        		// y ademas paso a listos al pj
                        		nodoDeListas = agregar_Lista_Recursos(nodoPj,nodoDeListas,recursoActual,structRecursos);

                        		cantNodos++;


                        	}

                        	//le mando al nivel los resultados: recursos libres y recursos adignados
                        	enviar_Struct_a_Nivel(structRecursos,fds[i].fd);
                        	list_add(listaDeListas,nodoDeListas);
                        	//fin region critica
                        	pthread_mutex_unlock( &mutex1 );

                        	destroy_paquete(paquete);




                        	break;

                        }
                        case HAY_INTERBLOQUEO:
                        {

                        	char* nombreNivel = malloc(strlen(paquete->data) + 1);
                        	strcpy(nombreNivel, paquete->data);

                        	destroy_paquete(paquete);

                        	paquete = recibir_Paquete(fds[i].fd);

                        	size_t longitud_datos_interbloqueo = paquete->size_data;
                        	char* datos_serializados = paquete->data;

                        	t_list *listaDeSimbolosBloqueados = list_create();

                        	listaDeSimbolosBloqueados = deserializar_Personajes_Bloqueados(datos_serializados, longitud_datos_interbloqueo);
            /*            	int testc = 0;

                        	while(testc < list_size(listaDeSimbolosBloqueados))
                        	{
                        	 char* simbolos = malloc(sizeof(char));
                        	 simbolos = list_get(listaDeSimbolosBloqueados, testc);

                        	printf("lista de simbolos bloqueados deserializados: %s\n", simbolos);

                        	testc++;
                        	}
*/
                        	t_list *listaDePjsBloqueados = filtrar_Solo_Bloqueados(listaDeNiveles, nombreNivel);

                 //       	printf("RECIBI HAY INTERBLOQUEO, CANTIDAD DE PJS FILTRADOS: %d\n", list_size(listaDePjsBloqueados));

                        	int es_el_primero = FALSE;
                        	int contador = 0;
                        	int cantidadBloqueados = list_size(listaDePjsBloqueados);
                        	t_datos_pj *nodoPrimeroPj = malloc(sizeof(t_datos_pj));


                        	if(list_size(listaDePjsBloqueados) > 1)
                        	{

                            while((es_el_primero == FALSE) && contador < cantidadBloqueados)
                        	     {
                        	      nodoPrimeroPj = list_get(listaDePjsBloqueados, contador);

                        	      int encontrado = FALSE;
                        	      int cont=0;
                        	      int cantidadSimbolos = list_size(listaDeSimbolosBloqueados);
                        	      	  while(encontrado == FALSE && cont < cantidadSimbolos)
                        	      	  {
                        	      		  char* simbolo = malloc(sizeof(char));
                        	      				 simbolo = list_get(listaDeSimbolosBloqueados,cont);

                        	      		  if(strcmp(simbolo, nodoPrimeroPj->simboloPj) == 0)
                        	      		  {
                        	      			  encontrado=TRUE;
                        	      		  }
                        	      		  else
                        	      		  {
                        	      			  cont++;
                        	      		  }

                        	      		  free(simbolo);
                        	      	  }

                        	      	  if(encontrado == TRUE)
                        	      		  es_el_primero =TRUE;
                        	      	  else
                        	      		  contador++;
                        	      	  }


                        	avisar_Victima_Seleccionada(listaPersonajesOrquestador,nodoPrimeroPj, nombreNivel);

                        	}

                        	list_clean(listaDeSimbolosBloqueados);
    						break;
                        }
                        case SIGNAL_RECIBIDA:
                        {
                        	pthread_mutex_lock( &mutex1 );

                        	bool es_El_Mismo_Socket(t_personajeOrquestador *ptr)
                        	{
                        	return ptr->socketPersonaje == fds[i].fd;
                        	}


                        	t_personajeOrquestador *nodoVictima = list_find(listaPersonajesOrquestador,(void*)es_El_Mismo_Socket);


                        	bool es_El_Mismo_Simbolo_Bloqueado(t_cola *ptr)
                        	{
                        		return strcmp(ptr->simboloPj, nodoVictima->simboloPersonaje) == 0;
                        	}

                        	t_listas *nodoNivel = buscar_Mismo_Nivel(paquete->data);
                        	t_cola *nodoNoBloqueado = list_remove_by_condition(nodoNivel->listaBloqueados, (void *) es_El_Mismo_Simbolo_Bloqueado);

                        	list_add(nodoNivel->listaDesconexiones, nodoNoBloqueado);

                        	list_add(nodoNivel->listaAnormales, nodoNoBloqueado);

                        	list_add(listaDeListas, nodoNivel);




                        	t_paquete *paquete = crear_Paquete(RESPUESTA_ORQUESTADOR,"Signal",strlen("Signal")+1);
                        	enviar_Paquete(nodoVictima->socketPersonaje,paquete);
                        	free(paquete);

                        	pthread_mutex_unlock( &mutex1 );

                        	break;
                        }
                        case DESCONEXION:
            			{

            				bool es_El_Mismo_Socket(t_personajeOrquestador *ptr)
            				{
            					return ptr->socketPersonaje == fds[i].fd;
            				}

            				t_personajeOrquestador *nodoVictima = list_remove_by_condition(listaPersonajesOrquestador, (void *) es_El_Mismo_Socket);




            		        log_info(logger,"Se desconecto el Personaje: %s del orquestador", nodoVictima->simboloPersonaje);

            				  //el personaje se desconecta, hay que cerrar el socket
                                         //y reallocar la memoria de las structs, se pone valor -1 para que el prox for detecte que ya no sirve.

                                         close(fds[i].fd);
                                         fds[i].fd = -1;
                                         /* esto "compacta" el array, quedando agrupados los sockets que siguen conectados o sin error, y ultimos
                                          * los que dan "-1", entonces al hacer el realloc de una "unidad" menos, se come al ultimo, o sea, al que ya no
                                          * nos sirve */
                                         int j;
                                         int i;
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

                                         	destroy_paquete(paquete);

            			break;
            		}//cierro case DESCONEXION


                }

                }
        }

        	if((primeraVez == FALSE) && (list_size(pjsOnline) == 0))
        		sleep(5);

        }while((primeraVez == TRUE) || (list_size(pjsOnline) > 0));


    //Cerrando sockets

        close (socket_servidor);
        inotify_rm_watch(file_descriptor, watch_descriptor);
        close(file_descriptor);
        return 0;
}
