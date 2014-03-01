/*
 * personaje.c
 *
 *  Created on: 05/05/2013
 *      Author: utnso
 */

#include "iniciarPersonaje.h"
#include "conexionPersonaje.h"
#include "mensajesPersonaje.h"
#include "serializarPersonaje.h"
#include <commons/config.h>
#include <commons/log.h>
#include <signal.h>


#define YES 1
#define NO 0

int empezarDeCero;
int muerto; //bandera para la señal SIGTERM y la de interbloqueo
int perdioVida; // idem





int main(int argc, char *argv[]){


	logger = log_create(argv[2], "Personaje", true, LOG_LEVEL_INFO);
	t_config *config = malloc(sizeof(t_config));
	t_personaje *personaje = malloc(sizeof(t_personaje));
	int socketPersonaje, socketPlanificador, socketNivel;
	empezarDeCero = YES;
	muerto = NO;
	int gano = NO;
	int bloqueado;
	int nivel;
	//handler para el tratamiento de señales
	void handler_Signal(int numero_senial)
	{
		switch(numero_senial)
		{
		case SIGUSR1:
		{
			personaje->pj_vidas++;
			log_info(logger, "SIGNAL SIGUSR1 recibida, ahora %s tiene %d vidas", personaje->pj_nombre, personaje->pj_vidas);

			break;
		}
		case SIGTERM:
		{
			if(personaje->pj_vidas > 0)
			{
			personaje->pj_vidas--;
			log_info(logger, "SIGNAL SIGTERM recibida, ahora %s tiene %d vidas", personaje->pj_nombre, personaje->pj_vidas);
			perdioVida = YES;

				if(bloqueado == YES)
				{
				t_paquete* paquete = crear_Paquete(SIGNAL_RECIBIDA, personaje->pj_planDeNiveles[nivel], strlen(personaje->pj_planDeNiveles[nivel]) + 1);
				enviar_Paquete(socketPersonaje, paquete);
				destroy_paquete(paquete);
				}

			}
			else{
				log_info(logger, "SIGNAL SIGTERM recibida, %s ha muerto", personaje->pj_nombre);
				muerto = YES;
				perdioVida = YES;
				empezarDeCero = YES;

				if(bloqueado == YES)
				{
					t_paquete* paquete = crear_Paquete(SIGNAL_RECIBIDA, personaje->pj_planDeNiveles[nivel], strlen(personaje->pj_planDeNiveles[nivel]) + 1);
					enviar_Paquete(socketPersonaje, paquete);
					destroy_paquete(paquete);
				}


			}



			//le pasa al nivel los recursos obtenidos

			break;
		}
		}
	}


	//Config
	config = config_create(argv[1]);

	while(empezarDeCero == YES)
	{
	//Iniciando el personaje con el archivo config
	personaje = iniciar_Personaje(config);
	empezarDeCero = NO;
	muerto = NO;


	//Conectandose al Orquestador

	socketPersonaje = conectar(personaje->ip_Orquestador,personaje->puerto_orquestador);
	//TODO log
	log_info(logger, "Conectado al orquestador con ip: %s:%d, desde: %s", personaje->ip_Orquestador, personaje->puerto_orquestador, personaje->ip_local);



	 //Handshake con el orquestador
	 hacer_Handshake(socketPersonaje);

	 //Recorre los niveles de su plan
	 nivel=0; //variable para recorrer el plan de niveles
	 int recurso = 0; //variable para recorrer los recursos del vector recursosAObtener (linea 52)
	 int nivelesRecorridos = 0; //contador
	 int nivelesTotales = personaje->pj_cant_niveles; //cant Maxima de niveles
	 int cantRecursosConsumidos = 0; //contador




	 //while para recorrer el plan de niveles
	 while((nivelesRecorridos < nivelesTotales) && (muerto == NO))
	 {
		 signal(SIGUSR1, handler_Signal);
		 signal(SIGTERM, handler_Signal);

		 int recursosTotalesDelNivel = personaje->pj_cantidadDeRecursos[nivel];
		 char **recursosAObtener = obtener_Recursos_Nivel(personaje->pj_planDeNiveles[nivel], config); //obtengo el vector de los recursos
																									 // ejemplo: [F,H,F,M]


		 //Pedir IP y Puerto del plan. y del nivel, al orquestador.
		 t_info_nivel_plan *info = pedir_Info_NivPla(socketPersonaje, personaje->pj_planDeNiveles[nivel]);


		 //cerrar conexion con el orquestador
		 close(socketPersonaje);
		 log_info(logger, "Desconectado del orquestador (ya se recibio la info del nivel) con ip: %s:%d, desde: %s", personaje->ip_Orquestador, personaje->puerto_orquestador, personaje->ip_local);


		 //conectandose al nivel y al planificador, handshake en ambos

		 socketPlanificador = conectar(info->ip_Plan,info->puerto_Plan);
		 hacer_Handshake(socketPlanificador);

		 //TODO log
		 log_info(logger, "Conectado al planificador con ip: %s:%d desde: %s", info->ip_Plan, info->puerto_Plan, personaje->ip_local);

		 char* simbolo = malloc(strlen(personaje->pj_simbolo) + 1);
		 strcpy(simbolo,personaje->pj_simbolo);

		 enviar_Simbolo(socketPlanificador, simbolo);

		 socketNivel = conectar(info->ip_Nivel, info->puerto_Nivel);
		 hacer_Handshake(socketNivel);

		 //TODO log
		 log_info(logger, "Conectado al nivel con ip: %s:%d, desde: %s", info->ip_Nivel, info->puerto_Nivel, personaje->ip_local);


		 enviar_Simbolo(socketNivel, simbolo);



		 //iniciar las banderas y cosas del personaje

		 int tieneDestino = NO;
		 t_destino *destino = malloc(sizeof(t_destino));
		 t_destino *posActual = malloc(sizeof(t_destino));
		 posActual->x = 1;
		 posActual->y = 1;
		 int terminoNivel = NO; // flag para ver si termino con los recursos de un nivel
		 perdioVida = NO;
		 bloqueado = NO;
		 int victimaInterbloqueo = NO;


		 //while para recorrer el nivel
		 while(terminoNivel == NO && perdioVida == NO && muerto == NO)
		 {

			 //Verifica si el personaje tiene o no el destino del recurso
			 if(tieneDestino == NO)
			 {
				 destino = pedir_Destino(socketNivel, recursosAObtener[recurso]);

				 tieneDestino = YES;
			 }

		//mandando el "estoy listo" al planificador
		estoy_Listo(socketPlanificador);



		 //esperar "movimiento permitido" del planificador y realizarlo
		//TODO log
		log_info(logger, "Mi posicion actual es: (%d,%d)", posActual->x, posActual->y);

		posActual = realizar_Movimiento(socketPlanificador, posActual, destino);

		log_info(logger, "Ahora estoy en: (%d,%d)", posActual->x, posActual->y);

		//envio mi nueva posicion al nivel para que la dibuje en la pantalla
		enviar_Nueva_Posicion(socketNivel, posActual);

		 //verifica si esta en el lugar exacto del recurso
		int resultado;

		resultado = verificar_Misma_Posicion(posActual, destino);

			if(resultado == YES)
			{
				int otorgado; //bandera para saber si el nivel le va a otorgar o no el recurso
				//TODO log
				log_info(logger, "Enviada una solicitud del recurso %s al nivel", recursosAObtener[recurso]);
				//pide la instancia al Nivel
				otorgado = pedir_Instancia(socketNivel, recursosAObtener[recurso], posActual);

				if(otorgado == NO)
				{
					//TODO log
					log_info(logger, "El recurso NO fue otorgado");
					//se bloquea
					log_info(logger, "Personaje BLOQUEADO");

					turno_Terminado(socketPlanificador, "BLOQUEADO");

					enviar_Recurso(socketPlanificador, recursosAObtener[recurso]);



					//Conectandose al Orquestador
					log_info(logger, "Conectado al orquestador (debido al bloqueo) con ip: %s:%d, desde: %s", personaje->ip_Orquestador, personaje->puerto_orquestador, personaje->ip_local);
					socketPersonaje = conectar(personaje->ip_Orquestador,personaje->puerto_orquestador);


					//Handshake con el orquestador
					hacer_Handshake(socketPersonaje);
					bloqueado = YES;
					//enviar un "espero respuesta" al orquestador
					enviar_Espero_Respuesta(socketPersonaje, personaje->pj_simbolo);


					//recibiendo la respuesta
					t_paquete *paquete = recibir_Paquete(socketPersonaje);

					switch(paquete->id)
					{
					case RESPUESTA_ORQUESTADOR:
					{
						if((strcmp(paquete->data, "Morite") == 0))
						{
							perdioVida= YES;
							personaje->pj_vidas--;
							victimaInterbloqueo = YES;
							if(personaje->pj_vidas < 0)
								{
									muerto = YES;
								}
							bloqueado = NO;
							avisar_Desconexion(socketPersonaje);
							close(socketPersonaje);
							log_info(logger, "Desconectado del orquestador (por victima de interbloqueo) con ip: %s:%d, desde: %s", personaje->ip_Orquestador, personaje->puerto_orquestador, personaje->ip_local);
						}
						else if((strcmp(paquete->data, "Signal") == 0))
						{
							bloqueado = NO;
							avisar_Desconexion(socketPersonaje);
							close(socketPersonaje);
							log_info(logger, "Desconectado del orquestador (por la señal SIGTERM) con ip: %s:%d, desde: %s", personaje->ip_Orquestador, personaje->puerto_orquestador, personaje->ip_local);
						}
						else if((strcmp(paquete->data, "Estas desbloqueado") == 0))
						{
							//TODO log
							log_info(logger, "El personaje fue DESBLOQUEADO");
							//recibio el recurso, o sea es == otorogado yes.
							//termina el turno y verifica si termino con todos los recursos

								cantRecursosConsumidos++;
								bloqueado = NO;

								//comprueba si termino el nivel
								if(cantRecursosConsumidos == recursosTotalesDelNivel)
								{
									terminoNivel = YES;
								}



								//turno_Terminado(socketPlanificador, "LISTO CON RECURSO");

								avisar_Desconexion(socketPersonaje);
								close(socketPersonaje);
								log_info(logger, "Desconectado del orquestador (por debloqueo) con ip: %s:%d, desde: %s", personaje->ip_Orquestador, personaje->puerto_orquestador, personaje->ip_local);

								if(terminoNivel == YES && perdioVida == NO && muerto == NO && bloqueado == NO)
								{
									//reseteamos todos los flags para el proximo nivel
									nivel++; //variable para avanzar en el plan de niveles
									nivelesRecorridos++; //flag para saber la cantidad de niveles terminados
									tieneDestino = NO;
									free(destino);
									recurso = 0;
									cantRecursosConsumidos = 0;
								/*	avisar_Desconexion(socketPlanificador);

									t_paquete *paquete = recibir_Paquete(socketPlanificador);

												 	if(strcmp(paquete->data, "OK") == 0)
												 	{
												 		printf("---CONFIRMACION DE DESCONEXION RECIBIDA---\n");
												 	}

												 	free(paquete);


												 close(socketPlanificador);
*/


								}
								else if(bloqueado == NO)//el pj todavia debe llegar a los otros recursos
								{
									//va al siguiente recurso
									recurso++;
									tieneDestino = NO;
									free(destino);
								}//cierra el else

						}
						else printf("Error al recibir una respuesta del orquestador\n");


						break;
					}
					default: {perror("Error, no recibi una respuesta valida del orquestador\n");}
					}


				}
				else //otorgado == YES, es decir, recibe la instancia al recurso
				{
					//TODO log
					log_info(logger, "El recurso SI fue otorgado");

					//termina el turno y verifica si termino con todos los recursos

					cantRecursosConsumidos++;

					//comprueba si termino el nivel
					if(cantRecursosConsumidos == recursosTotalesDelNivel)
					{
						terminoNivel = YES;
					}

					turno_Terminado(socketPlanificador, "LISTO CON RECURSO");



					if(terminoNivel == YES && perdioVida == NO && muerto == NO && bloqueado == NO)
					{
						//reseteamos todos los flags para el proximo nivel
						nivel++; //variable para avanzar en el plan de niveles
						nivelesRecorridos++; //flag para saber la cantidad de niveles terminados
						tieneDestino = NO;
						free(destino);
						recurso = 0;
						cantRecursosConsumidos = 0;



					}
					else if(bloqueado == NO)//el pj todavia debe llegar a los otros recursos
					{
						//va al siguiente recurso
						recurso++;
						tieneDestino = NO;
						free(destino);
					}//cierra el else
				}//cierra el otorgado

			}else //resultado == NO
			{
				turno_Terminado(socketPlanificador, "LISTO");
			}



			//vemos si el personaje perdio una vida, tiene que reiniciar el nivel de 0
			 if(perdioVida == YES)
					 {
						 recurso = 0;
						 tieneDestino = NO;
						 terminoNivel = NO;
						 cantRecursosConsumidos = 0;
					 }


			 if(muerto == YES)
			 {
				 perdioVida = YES;
				 empezarDeCero = YES;
			 }
		 }//cierra el while, recorrio un nivel

		 if(perdioVida == NO && muerto == NO && bloqueado == NO){
			 //TODO log
		 log_info(logger, "Desconectandose del orquestador, nivel y planificador por nivel finalizado");

		 }

		 //pregunto si el pj perdio una vida, en caso afirmativo reinicio el nivel





		 if(nivelesRecorridos == nivelesTotales && perdioVida == NO && muerto == NO)
		 {
			 //termino el juego
			 gano = YES;
			 avisar_Juego_Terminado(socketPlanificador);


			 t_paquete *paquete = recibir_Paquete(socketPlanificador);

			 destroy_paquete(paquete);

			 close(socketPlanificador);
			 log_info(logger, "Desconectado del planificador (por juego terminado) con ip: %s:%d, desde: %s", info->ip_Plan, info->puerto_Plan, personaje->ip_local);

		 }
		 else{//termino un nivel o perdio una vida o estoy muerto
			 //TODO log
			 avisar_Desconexion(socketPlanificador);



			 	t_paquete *paquete = recibir_Paquete(socketPlanificador);



			 	destroy_paquete(paquete);


			 close(socketPlanificador);
			 log_info(logger, "Desconectado del planificador con ip: %s:%d, desde: %s", info->ip_Plan, info->puerto_Plan, personaje->ip_local);

		 }


		 if(victimaInterbloqueo)
		 {
			 avisar_Desconexion_Interbloqueo(socketNivel);
		 }
		 else
		 {
		 avisar_Desconexion(socketNivel);
		 }

		 close(socketNivel);
		 log_info(logger, "Desconectado del nivel con ip: %s:%d, desde: %s", info->ip_Nivel, info->puerto_Nivel, personaje->ip_local);

		 //sin este if, el personaje hacia dos veces la conexion al orquestador
		 if((muerto == NO) && (gano == NO)){ //aca solamente me conecto al orquestador si el personaje no se murio
		 //conexion con el orquestador
		 socketPersonaje = conectar(personaje->ip_Orquestador,personaje->puerto_orquestador);
		 //TODO log
		 log_info(logger, "Conectado al orquestador (para pedir la info del siguiente nivel) con ip: %s:%d, desde: %s", personaje->ip_Orquestador, personaje->puerto_orquestador, personaje->ip_local);

		 hacer_Handshake(socketPersonaje);
		 }


		 destroy_info(info);
	 }//cierra el while, recorrio todos los niveles, ya le avise al planificador y al nivel que termine t0do el game

	}//cierra el while de empezar de cero


	log_info(logger, "%s ha termiando el juego", personaje->pj_nombre);
	destroy_personaje(personaje);
	config_destroy(config);
	return 0;
}
