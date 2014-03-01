/*
 * nivel.c
 *
 *  Created on: 16/05/2013
 *      Author: utnso
 */

#include "iniciarNivel.h"
#include <commons/config.h>
#include "conexionNivel.h"
#include "mensajesNivel.h"
#include "interbloqueo.h"
#include <signal.h>
#include <commons/log.h>



int main(int argc, char *argv[]){
		listaDePersonajes = list_create();
		listaDeRecursosDisponibles = list_create();

		logger = log_create(argv[2], "Nivel", false, LOG_LEVEL_INFO);

        int socketNivel;

        int rows, cols;


    //TODO    //dibujando el escenario
                nivel_gui_inicializar();

              nivel_gui_get_area_nivel(&rows, &cols);



        //Config

        t_config *config = config_create(argv[1]);


      //Iniciando el nivel con el archivo config

        t_nivel *nivel = iniciar_Nivel(config);



        //Conectandose al Orquestador
        //conectar_Orquestador(ip, puerto)

    ///TODO    printf("Conectandose al orquestador...\n");

        socketNivel = conectar(nivel->ip_Orquestador, nivel->puerto_orquestador);

        log_info(logger, "Conectado al Orquestador con ip %s:%ld, desde %s:%ld", nivel->ip_Orquestador, nivel->puerto_orquestador, nivel->niv_ip, nivel->niv_puerto);


        //handshake con el Orquestador
        hacer_Handshake(socketNivel);



        //enviar info del nivel al Orquestador

        enviar_Datos_Nivel(nivel, socketNivel);



        //conexiones de personajes

         int socket_servidor,cliente_nuevo;
         struct sockaddr_in infoSocketServidor;
         socklen_t longitud_del_cliente;
         int nroPuerto = nivel->niv_puerto;
         int actividad, timeout, tamanio_actual;
         struct pollfd *fds=NULL;
         int nfds;
         int i;
      //   int hayUnPersonaje; //bandera para inciciar el algoritmo de interbloqueo

         //el nivel abre las conexiones
         socket_servidor = abrir_conexion_servidor(nroPuerto, BACKLOG);

         	 //nfds es la variable donde guarda la cantidad de sockets
             nfds=1;
             //seteando memoria dinamica para guardar 1 socket
             fds = (struct pollfd *)calloc(1,nfds*sizeof(struct pollfd));

             fds->fd =socket_servidor;
             fds->events = POLLIN;
             timeout = -1;
             temporizador = nivel->niv_tiempoDeadlock;


             ejecutarInterbloqueo = NO;



             void handler_Signal(int numero_senial)
             	{
             		switch(numero_senial)
             		{
             		case SIGALRM:
             		{
             			ejecutarInterbloqueo = YES;

             			break;
             		}
             		}
             	}


             alarm(temporizador);

             while (1)
             {

            	 signal(SIGALRM, handler_Signal);


            	 if(ejecutarInterbloqueo == YES)
            	 {
            		 int rInter;
            		 pthread_t hiloInterbloqueo;

            		 if( (rInter=pthread_create( &hiloInterbloqueo, NULL, detectar_Interbloqueo, NULL)) )
            		 {
            			 printf("Error al crear el hilo: %d\n", rInter);
            		 }


            		 pthread_join(hiloInterbloqueo, NULL);

            		 alarm(temporizador);
            		 ejecutarInterbloqueo = NO;

            	 }




				 if(hayInterbloqueo)
				 {
					 //TODO indicar por log los procesos de listaPersonajesBloqueados



					 if(nivel->niv_recovery)
					 {
						 char* simbolos_serializados;
						 int cantPersonajes = list_size(listaPersonajesBloqueados);
						 size_t longitud_datos = cantPersonajes * 2;

						 t_paquete *paquete = malloc(sizeof(t_paquete));

						 paquete = crear_Paquete(HAY_INTERBLOQUEO, nivel->niv_nombre, strlen(nivel->niv_nombre)+ 1);
						 enviar_Paquete(socketNivel, paquete);

						 free(paquete->data);
						 free(paquete);

						 simbolos_serializados = serializar_Personajes_Bloqueados(listaPersonajesBloqueados, longitud_datos);

						 list_clean(listaPersonajesBloqueados);

						 paquete = crear_Paquete(HAY_INTERBLOQUEO, simbolos_serializados, longitud_datos);

						 int nroBytesEnviados = enviar_Paquete(socketNivel, paquete);

						 hayInterbloqueo = FALSE;

						 if(nroBytesEnviados < 0)
						 {
							 perror("Error al enviar el paquete de interbloqueo al orquestador\n");
							 exit(1);
						 }
					 }

				 }






      //TODO           printf("Waiting on poll()...\n");

       //TODO          printf("nfds = %d\n",nfds);

                 //esperando alguna actividad de algun socket
                 actividad = poll(fds, nfds, timeout);

                 //error en el poll
                 if (actividad < 0)
                 {
                       /*  perror("  poll() failed");
                         break;
                         */
                	 continue;
                 }

                 //finalizacion por timeout
                 if (actividad == 0)
                 {
   //TODO                	 //creacion del hilo para el algoritmo de interbloqueo


                         continue;
                 }

                 tamanio_actual = nfds;

                 for(i=0; i < tamanio_actual; i++){

                         //si no hay ningun revent, se vuelve al sgte ciclo del for
                         if(fds[i].revents == 0)
                                 continue;

                         //no se detecta la actividad esperada
                         if(fds[i].revents != POLLIN)
                         {
           //TODO                      printf("  Error! revents = %d\n", fds[i].revents);
                                 break;
                         }

                         //el primer socket tiene que ser si o si del servidor, para inciar las escuchas
                         if((fds+i)->fd==socket_servidor)
                         {
         //TODO                        printf("  Listening socket is readable\n");


                                 do {
                                         longitud_del_cliente = sizeof(infoSocketServidor);

                                         //modifico la memoria dinamica para que albergue ahora a 2 structs de sockets
                                         fds =(struct pollfd *) realloc (fds,(nfds+1)*sizeof(struct pollfd));

                                         cliente_nuevo = aceptar_cliente(socket_servidor,longitud_del_cliente);

                                         //error
                                         if (cliente_nuevo < 0)
                                         {
                                                 if (errno != EWOULDBLOCK)
                                                 {
                                                         perror("  accept() failed");
                                                 }

                                                 //elimino esa "unidad de estructura" agregada
                                                 fds = (struct pollfd *)realloc(fds,nfds*sizeof(struct pollfd));
                                                 break;
                                         }

                                         //agrego el nuevo socket
                                         fds[nfds].fd = cliente_nuevo;
                                         fds[nfds].events = POLLIN;
                                         nfds++;
         //TODO                                printf("Se conecto un cliente\n");

                                 }while (cliente_nuevo != -1);
                         } else


                        	 //empieza a operar
                                 {

                        	 	 	 	 int id;
                        	 	 	 	 t_paquete *paquete = malloc(sizeof(t_paquete));
                                         paquete = recibir_Paquete(fds[i].fd);

                                         id = paquete->id;

                                         //hace X cosa segun la id, definida en defines.h
                                          switch(id)
                                          {
                                          case HANDSHAKE:
                                          {

                                              if((strcmp(paquete->data, "Soy un personaje") == 0))
                                              {
            //TODO                                     printf("Handshake del personaje aceptado\n");
                                                 agregar_Personaje(fds[i].fd);
                                                 free(paquete->data);
                                                 free(paquete);

                                                 //recibir el simbolo
                                                 paquete = recibir_Paquete(fds[i].fd);

                                                 char* simboloPersonaje = malloc(strlen(paquete->data) + 1);
                                                 strcpy(simboloPersonaje, paquete->data);

                                                 agregar_Simbolo(fds[i].fd, simboloPersonaje);

                                                 log_info(logger,"Se conecto el Personaje: %s", simboloPersonaje);

                                         //TODO dibujar el nuevo pj a la posicion (1,1)

                                                 inicializar_Posicion(simboloPersonaje);

                                                 free(simboloPersonaje);
                                                 free(paquete->data);
                                                 free(paquete);


                                              }else
                                              {
                                                 perror("Error en el Handshake con el personaje");
                                                 exit(1);
                                              }

                                               break;
                                          }
                                          case NUEVA_POSICION:

                                          {
                                        	  char* info_serializada = paquete->data;
                                        	  size_t longitud_datos = paquete->size_data;

                                        	  t_destino *nuevaPos = deserializar_Destino(info_serializada, longitud_datos);

                                        	  dibujar_Nueva_Posicion(fds[i].fd,nuevaPos);

                                        	  free(paquete->data);
                                        	  free(paquete);

                                        	  break;
                                          }

                                          case DESTINO: //recibe del personaje, el pedido de la ubicacion de un recurso
                                          {
                                        	  	  	  	char* recurso = malloc(strlen(paquete->data) + 1);
                                        	  	  	  	strcpy(recurso, paquete->data);
                                                        t_destino *destino = buscar_Destino(recurso, nivel->listaDeCajas);


                                                        //ahora le mandamos los datos

                                                        enviar_Destino(fds[i].fd, destino);

                                                        free(recurso);
                                                        free(paquete->data);
                                                        free(paquete);
                                                        free(destino);
                                                        break;
                                          }
                                          case PEDIR_INSTANCIA:
                                          {
                                        	  //recibiendo el paquete con la posX, posY y el recurso "pedido"

                                        	  char *info_serializada = paquete->data;
                                        	  size_t longitud_info = paquete->size_data;

                                        	  t_datosParaPedirInstancia *datos = deserializar_Info_Pedir_Instancia(info_serializada, longitud_info);


                                        	  int respuesta;

                       //TODO grafica      	  //verificando que el recurso "pedido" este en el lugar X e Y correctos
                                        	  respuesta = verificar_Posicion(datos, nivel->listaDeCajas);

                                        	  char* recurso = malloc(strlen(datos->recurso) + 1);
                                        	  strcpy(recurso, datos->recurso);

                                        	  //agrego el recurso consumido al personaje
                                        	  if(respuesta == YES)
                                        	  {

                                        		  agregar_Recurso(fds[i].fd, recurso);


                                        	  }else
                                        		  //agrego a la lista de personajes, el mismo y el recurso que busca
                                        		  agregar_Recurso_Buscado(fds[i].fd, recurso);



                                        	  //enviando la respuesta al personaje
                                        	  enviar_Respuesta_Otorgado(fds[i].fd, respuesta);

                                        	  free(paquete->data);
                                        	  free(paquete);
                                        	  free(datos->recurso);
                                        	  free(datos);
                                        	  break;

                                          }
                                          case DESCONEXION:
                                          {


                                          		//1. mandar la struct serializada (simbolo y recursos) al orquestador
                                          		//TODO pasar esto a una funcion
                                          		int cantRecursosGanados = cantidad_Recursos_Ganados(fds[i].fd);


                                          		t_personaje *personaje = buscar_Eliminar_Personaje(fds[i].fd);

                                          		char simbolo = personaje->simboloPersonaje[0];

                                          		log_info(logger,"Se desconecto el Personaje: %c", simbolo);

                                          		if(strcmp(paquete->data, "Interbloqueo") == 0)
                                          		{
                                          			log_info(logger, "La victima por el interbloqueo es: %c", simbolo);
                                          		}


		                                          		free(paquete->data);
		                                          		free(paquete);

//TODO
                                          		BorrarItem(&ListaItems, simbolo);
                                         		nivel_gui_dibujar(ListaItems);

                                          		//1 por el simbolo (+1) + 1 char por cada recurso ganado (F,M,etc) + 1 por caracter vacio + strlen nombre del nivel (+1)
                                          		size_t longitud_datos = strlen(personaje->simboloPersonaje) + 1 + strlen(nivel->niv_nombre) + 1 + (cantRecursosGanados * 2);

                                          		char* datos_serializados = serializar_Recursos_Para_Orquestador(personaje, longitud_datos, cantRecursosGanados, nivel->niv_nombre);


												paquete = malloc(sizeof(t_paquete));
                                          		paquete = crear_Paquete(RECURSOS_LIBERADOS, datos_serializados, longitud_datos);

                                          		int nroBytesEnviados = enviar_Paquete(socketNivel, paquete);

                                          		if (nroBytesEnviados < 0)
                                          		{
                                          			perror("Error al enviar los recursos liberados al orquestador\n");
                                          			exit(1);
                                          		}

                                          		//TODO recibir del orquestador los recursos asignados a X personajes y los que sobraron

                                          		//TODO editar la lista de personajes, agregandoles el recurso asignado
                                          		free(paquete->data);
                                          		free(paquete);

                                          		paquete = recibir_Paquete(socketNivel);
                                          		if(paquete->id == RECURSOS_LIBRES)
                                          		{
                                          			size_t longitud_datos_libres = paquete->size_data;
                                          			char* datos_serializados = paquete->data;

                                          			if(longitud_datos_libres > 0)
                                          			{
                                          				t_list *listaRecursosLibres = list_create();

                                          				listaRecursosLibres = deserializar_Recursos_Libres(datos_serializados, longitud_datos_libres);

                                          				actualizar_Recursos(listaRecursosLibres, nivel->listaDeCajas);

                                          				list_clean(listaRecursosLibres);
                                          				list_destroy(listaRecursosLibres);
                                          			}
                                          		}
                                          		if(paquete->id == RECURSOS_LIBRES_CERO)
                                          		{

                                          		}

                                          		//recibo los personajes con sus recursos asignados
                                          		//paquete = malloc(sizeof(t_paquete));
                                          		free(paquete->data);
                                          		free(paquete);
                                          		paquete = recibir_Paquete(socketNivel);
                                          		//TODO PORQUE GARCHA NO RECIBE ESTE PUTO MENSAJE DIOS
                                          		if(paquete->id == RECURSOS_ASIGNADOS)
                                          		{
                                          			size_t longitud_datos_asignados = paquete->size_data;
                                          			char* datos_serializados = paquete->data;


                                          			if(longitud_datos_asignados > 0)
                                          			{
                                          				 t_list *listaRecursosAsignados = list_create();

                                          				 listaRecursosAsignados = deserializar_Recursos_Asignados(datos_serializados, longitud_datos_asignados);

                                          				 actualizar_lista_personajes(listaRecursosAsignados);

                                          		/*		list_clean(listaRecursosAsignados);
                                          				list_destroy(listaRecursosAsignados);*/
                                          			}
                                       			}
                                          		if(paquete->id == RECURSOS_ASIGNADOS_CERO)
                                          		{

                                          		}
                                          		free(paquete->data);
                                          		free(paquete);





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





                                                       			break;
                                                       		}//cierro case DESCONEXION









                                          }//cierra switch



                                 }//cierra del else del poll, o sea, cuando empieza a laburar con el socket
                 }//cierra el for del poll, que busca cual fue el socket que tuvo un revent
             }//cierra while del poll
             return 0;
}
