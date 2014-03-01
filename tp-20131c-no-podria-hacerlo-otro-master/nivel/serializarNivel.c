/*
 * serializarNivel.c
 *
 *  Created on: 18/05/2013
 *      Author: utnso
 */


#include "serializarNivel.h"




static int nuevo_Send(int socket, char *stream, size_t longitud_Stream, int flag);

static int nuevo_Recv(int socket, char *stream, size_t longitud_Stream, int flag);


t_paquete *crear_Paquete(int8_t id, char *data, int16_t size_data)
{
        t_paquete *paquete = malloc(sizeof(t_paquete));

        paquete->id = id;
        paquete->size_data = size_data;
        paquete->data = malloc(size_data);


        //TODO verificar si el size_data > 0??

        memcpy(paquete->data,data,size_data);

        return paquete;
}

int enviar_Paquete(int socket, t_paquete *paquete)
{
        size_t longitud_Stream = 0;
        char *stream = NULL;
        int nroBytesEnviados = 0;

        stream = serializar_Paquete(paquete, &longitud_Stream); //Aca sale retornado el paquete serializado
                                                                                                                        //Y LA LONGITUD DEL MISMO (longitud_stream)

        //nuevo_Send(int32_t socket, t_paquete *paquete, int32_t longitud_Stream, int flag);
        nroBytesEnviados = nuevo_Send(socket, stream, longitud_Stream, 0);

        if(nroBytesEnviados < 0){
       //TODO         perror("Fallo en el send del paquete\n");
                exit(1);
        }
        free(stream);

        return 0;
}

char *serializar_Paquete(t_paquete *paquete, size_t *longitud_Stream)
{
        char *stream = NULL;
        int offset = 0, tmp_size = 0;
        *longitud_Stream = 0;

        *longitud_Stream = (SIZE_CABECERA + (paquete->size_data) * sizeof(char)); //es la suma entre el tamaño de la
                                                                                                                                                        //cabecera + el cuerpo (de tamaño variable)

        stream = malloc(*longitud_Stream);

        memset(stream, '0', *longitud_Stream);

        //pasando el id
        tmp_size = sizeof(int8_t);
        memcpy(stream, &paquete->id, tmp_size);

        offset += tmp_size;

        //pasando el tamaño
        tmp_size = sizeof(int16_t);
        memcpy(stream + offset, &paquete->size_data, tmp_size);

        offset += tmp_size;

        //pasando la data del cuerpo
        tmp_size = paquete->size_data;
        memcpy(stream + offset, paquete->data, tmp_size);

        offset += tmp_size;

        *longitud_Stream = offset; //aca sale la variable modificada!. Tiene el tamaño completo de tod0 el stream serializado

        return stream;
}


t_paquete *deserializar_Cabecera(char* data)
{
        t_paquete *paquete = malloc(sizeof(t_paquete));
        int offset = 0, tmp_size = 0;

         memset(paquete, '0', sizeof(t_paquete));

        //sacando el id
        tmp_size = sizeof(int8_t);
        memcpy(&paquete->id,data, tmp_size);

        offset += tmp_size;

        //sacando el lenght
        tmp_size = sizeof(int16_t);
        memcpy(&paquete->size_data,data+offset, tmp_size);

        return paquete;
}

void deserializar_Cuerpo(t_paquete *paquete, char *stream)
{
        int size_data = paquete->size_data;

        paquete->data = malloc(size_data * sizeof(char *));

        memcpy(paquete->data, stream, paquete->size_data);
}



static int nuevo_Send(int socket, char *stream, size_t longitud_Stream, int flag)
{
        int nroBytesEnviados = 0;
        int offset = 0;

        size_t tmp_size_stream = longitud_Stream;

        while(offset < tmp_size_stream)
        {
                nroBytesEnviados = send(socket, stream + offset, tmp_size_stream, flag | MSG_NOSIGNAL);

                if(nroBytesEnviados < 0){
                        perror("Error en enviar el stream\n");
                        if(errno == EINTR)
                                continue;
                        else
                                exit(1);
                        }
                else if(nroBytesEnviados == 0){
                        perror("Error en enviar el stream, se corto la conexion\n");
                        exit(1);
                }else
                {
                offset += nroBytesEnviados;
                tmp_size_stream -= nroBytesEnviados;
        }
        }
        return nroBytesEnviados;

}

t_paquete *recibir_Paquete(int socket){
        char *stream = malloc(SIZE_CABECERA);
        uint32_t nroBytesRecibidos = 0;

        memset(stream, '0', SIZE_CABECERA);


        //Recibiendo la cabecera
        nroBytesRecibidos = nuevo_Recv(socket, stream, SIZE_CABECERA, 0);

        if(nroBytesRecibidos <= 0){
                perror("Error en el recv de la cabecera");
                free(stream);
                return NULL;
        }
        else //recibi toda la cabecera, ahora la deserializo
        {
                t_paquete *paquete = deserializar_Cabecera(stream);
                free(stream);

                //ahora recibo el cuerpo

                stream = malloc(paquete->size_data);

                nroBytesRecibidos = nuevo_Recv(socket, stream, paquete->size_data, 0);


                if(nroBytesRecibidos <= 0){
                        perror("Problema en el recv del cuerpo");
                        free(stream);
                        free(paquete);
                        return NULL;
                }
                else
                {       //recibi tod0 el cuerpo, lo deserializo

                deserializar_Cuerpo(paquete, stream);
                free(stream);

                return paquete;

                }

        }
}

static int nuevo_Recv(int socket, char *stream, size_t longitud_Stream, int flag)
{
        int nroBytesRecibidos = 0;
        int offset = 0;
        size_t tmp_longitud_Stream = longitud_Stream;

        while( offset < tmp_longitud_Stream ) {
                nroBytesRecibidos = recv(socket, stream + offset, tmp_longitud_Stream, flag | MSG_NOSIGNAL);
                if ( nroBytesRecibidos  <= 0) {
                        if(errno == EINTR)
                                continue;
                        else{
                                return nroBytesRecibidos;
                        perror("Error en el recv\n");
                        exit(1);}
                }
                else{
                        offset += nroBytesRecibidos;
                        tmp_longitud_Stream -= nroBytesRecibidos;
                }
        }
        return nroBytesRecibidos;
}


char *serializar_Datos_Nivel(t_info_datos_nivel *info, size_t longitud_info_nivel)
{
        char* stream = NULL;
        int offset = 0, tmp_size = 0;


        stream = malloc(longitud_info_nivel);

        //se pasa el nombre del nivel
        tmp_size = (strlen(info->nombre_Nivel) + 1);
        memcpy(stream, info->nombre_Nivel, tmp_size);

        offset += tmp_size;

        //se pasa la ip del nivel
        tmp_size = (strlen(info->ip_Nivel) + 1);
        memcpy(stream + offset, info->ip_Nivel, tmp_size);

        offset += tmp_size;

        //se pasa el puerto del nivel
        tmp_size = sizeof(int32_t);
        memcpy(stream + offset, &info->puerto_Nivel, tmp_size);

        return stream;
}

char *serializar_Destino(t_destino *destino, size_t longitud_destino)
{
	char *stream = NULL;
	int offset = 0, tmp_size = 0;

	stream = malloc(longitud_destino);

	//pasamos al stream el posX
	tmp_size = sizeof(int8_t);
	memcpy(stream, &destino->x, tmp_size);

	offset += tmp_size;

	//pasamos el posY

	tmp_size = sizeof(int8_t);
	memcpy(stream + offset, &destino->y, tmp_size);

	return stream;
}

t_destino *deserializar_Destino(char *data, size_t longitud_Destino)
{
	int offset = 0, tmp_size = 0;
	t_destino *destino = malloc(longitud_Destino);

	//paso la variable X
	tmp_size = sizeof(int8_t);
	memcpy(&destino->x, data, tmp_size);

	offset += tmp_size;

	//paso la variable Y
	tmp_size = sizeof(int8_t);
	memcpy(&destino->y, data + offset, tmp_size);

	return destino;
}

t_datosParaPedirInstancia *deserializar_Info_Pedir_Instancia(char* data, size_t longitud_info)
{
	t_datosParaPedirInstancia *datos = malloc(sizeof(t_datosParaPedirInstancia));
	int offset = 0, tmp_size = 0;

	//recibiendo la posX
	tmp_size = sizeof(int8_t);
	memcpy(&datos->posX, data, tmp_size);

	offset += tmp_size;

	//recibiendo la posY
	tmp_size = sizeof(int8_t);
	memcpy(&datos->posY, data + offset, tmp_size);

	offset += tmp_size;

	//recibiendo el recurso, que es un char*
	//averiguo la cantidad de bytes que tiene el string recurso

	for(tmp_size = 1; (data)[tmp_size - 1] != '\0'; tmp_size ++);

	datos->recurso = malloc(tmp_size * sizeof(char *));

	memcpy(datos->recurso, data + offset, tmp_size);

	return datos;
}


char* serializar_Recursos_Para_Orquestador(t_personaje *personaje, size_t longitud_datos, int cantRecursosGanados, char* nombreNivel)
{
	char *stream = NULL;
	int offset = 0, tmp_size = 0;
	int cantNodosSacados = 0;
	char* recurso;

	stream = malloc(longitud_datos);

	//agrego el nombre del nivel
	tmp_size = strlen(nombreNivel) + 1;
	memcpy(stream, nombreNivel, tmp_size);

	offset += tmp_size;

	//agrego el simbolo del personaje
	tmp_size = strlen(personaje->simboloPersonaje) + 1; //porque es un char
	memcpy(stream + offset, personaje->simboloPersonaje, tmp_size);

	offset += tmp_size;

	//hago un while para sacar todos los nodos e ir metiendo la letra al stream
	while(cantNodosSacados < cantRecursosGanados)
	{
		recurso = eliminar_Recurso(personaje->listaDeRecursosGanados);
//TODO	printf("RECURSO QUE SERIALIZO: %s\n", recurso);

		tmp_size = strlen(recurso) + 1;
		memcpy(stream + offset, recurso, tmp_size);

		offset+= tmp_size;

		cantNodosSacados++;
	}


	return stream;

}


char* serializar_Personajes_Bloqueados(t_list *listaPersonajesBloqueados,size_t longitud_datos)
{
	char *stream = malloc(longitud_datos);

	int tmp_size = 0, offset = 0;
	int cantidadPersonajes = list_size(listaPersonajesBloqueados);
	int personajeAgregado = 0;

	while(personajeAgregado < cantidadPersonajes)
	{
		char* simboloPersonaje = malloc(sizeof(char));
		simboloPersonaje = list_get(listaPersonajesBloqueados, personajeAgregado);
		tmp_size = strlen(simboloPersonaje) + 1;

	//TODO	printf("PERSONAJE BLOQUEADO ENVIADO: %s\n", simboloPersonaje);

		memcpy(stream + offset, simboloPersonaje, tmp_size);

		offset += tmp_size;

		personajeAgregado ++;
	}

	return stream;
}


t_list *deserializar_Recursos_Libres(char *datos_serializados,size_t longitud_datos)
{
	t_list* listaRecursosLibres = list_create();
	int tmp_size = 0, offset = 0;


//TODO	printf("RECIBO LOS RECURSOS SOBRANTES\n");
	while(offset < longitud_datos)
	{
		for(tmp_size = 1; (datos_serializados+offset)[tmp_size - 1] != '\0'; tmp_size ++);
		char* recurso = malloc(sizeof(char) * tmp_size);

		memcpy(recurso, datos_serializados + offset, tmp_size);


		list_add(listaRecursosLibres, recurso);

		offset += tmp_size;
	}


	return listaRecursosLibres;
}

t_list *deserializar_Recursos_Asignados(char* datos_serializados, size_t longitud_datos)
{
	t_recursos_asignados *recursoAsignado = malloc(sizeof(t_recursos_asignados));
	t_list *recursosAsignados = list_create();


	int tmp_size = 0, offset = 0;

	//TODO printf("RECIBO LOS RECURSOS ASIGNADOS A TAL PERSONAJE\n");
	while(offset < longitud_datos )
	{

		for(tmp_size = 1; (datos_serializados+offset)[tmp_size - 1] != '\0'; tmp_size ++);
		char* simbolo = malloc(sizeof(char) * tmp_size);
		memcpy(simbolo, datos_serializados + offset, tmp_size);

		offset += tmp_size;


		for(tmp_size = 1; (datos_serializados+offset)[tmp_size - 1] != '\0'; tmp_size ++);
		char* recurso = malloc(sizeof(char) * tmp_size);
		memcpy(recurso, datos_serializados + offset, tmp_size);

		offset += tmp_size;

		recursoAsignado->simboloPj = malloc(strlen(simbolo) + 1);
		strcpy(recursoAsignado->simboloPj,simbolo);

		recursoAsignado->recursoAsignado = malloc(strlen(recurso) + 1);
		strcpy(recursoAsignado->recursoAsignado, recurso);

	//TODO	printf("EL PERSONAJE: %s, RECIBIO EL RECURSO: %s DEL ORQUESTADOR\n", simbolo, recurso);

		list_add(recursosAsignados, recursoAsignado);

	}

	return recursosAsignados;

}
