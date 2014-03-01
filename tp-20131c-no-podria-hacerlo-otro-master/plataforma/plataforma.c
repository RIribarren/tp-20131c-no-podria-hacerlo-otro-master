/*
 * plataforma.c
 *
 *  Created on: 09/05/2013
 *      Author: clavelli
 */


#include "orquestador.h"

#include <commons/log.h>
t_log* logger;

#define BACKLOG 10


//TODO sacar los comentarios y reemplazar socket_cliente por fds[i].fd para que sea multiusuario(DONE)
//TODO hay problemas en el memcpy de escribirMensaje y leerMensaje(BULLSHIT)

int main(int argc, char *argv[]){


        pthread_t hiloOrquestador;
        int rOrq;
        listaDeListas = list_create();
        listaDeNiveles = list_create();
        listaPersonajesOrquestador = list_create();
        pjsOnline = list_create();

        //Config y colas (globales)
        config = config_create("/home/utnso/workspace/tp-20131c-no-podria-hacerlo-otro/plataforma/configPlataforma");
        ipOrquestador = config_get_string_value(config, "plataforma");
        quantum = config_get_int_value(config, "quantum");
  //      printf("quantum:%d\n",quantum);
        tiempoDeEspera = (config_get_double_value(config, "tiempoDeEspera") * 1000000);


        printf("\x1B[2J"); //hace clear de la consola


        logger = log_create(argv[2], "Plataforma", true, LOG_LEVEL_INFO);


        if( (rOrq=pthread_create( &hiloOrquestador, NULL, funcionOrquestador, NULL)) )
        	{
              printf("Error al crear el hilo: %d\n", rOrq);
            }


        pthread_join(hiloOrquestador, NULL);



        int respuesta;



        log_info(logger, "Todos los personajes han terminado el juego!");

        printf("\x1B[2J");
        printf("Desea ejecutar KOOPA?.\n OPCIONES:\n SI = 1\n NO = 2\nRESPUESTA: ");
        scanf("%d", &respuesta);

        if(respuesta == TRUE)
        {

        	execl("/home/utnso/workspace/tp-20131c-no-podria-hacerlo-otro/memoria/koopa", "koopa", argv[1], (char *) 0);
        }else
        {
        	log_info(logger, "Koopa no ha sido ejecutado");
        }



        return 0;
}
