/*
 * serializarPlataforma.c
 *
 *  Created on: 11/05/2013
 *      Author: utnso
 */


#include "serializarPlataforma.h"



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
		printf("Fallo en el send del paquete\n");
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

	*longitud_Stream = (SIZE_CABECERA + paquete->size_data * sizeof(char)); //es la suma entre el tamaño de la
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

	paquete->data = malloc(size_data);

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
		{	//recibi tod0 el cuerpo, lo deserializo

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
		if ( nroBytesRecibidos	<= 0) {
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

char *serializar_Info(t_info_nivel_plan *info, size_t longitud_info)
{
	char* stream = NULL;
	int offset = 0, tmp_size = 0;


	stream = malloc(longitud_info);

	//pasando la ip del planif.
	tmp_size = (strlen(info->ip_Plan) + 1);
	memcpy(stream, info->ip_Plan, tmp_size);

	offset += tmp_size;

	//pasando el puerto del planif.
	tmp_size = sizeof(int32_t);
	memcpy(stream + offset, &info->puerto_Plan, tmp_size);

	offset += tmp_size;

	//pasando la ip del nivel
	tmp_size = strlen(info->ip_Nivel) + 1;
	memcpy(stream + offset, info->ip_Nivel, tmp_size);

	offset += tmp_size;

	//pasando el puerto del nivel
	tmp_size = sizeof(int32_t);
	memcpy(stream + offset, &info->puerto_Nivel, tmp_size);

	return stream;
}

t_info_nivel_plan *deserializar_Info(char* data, size_t longitud_info)
{
        t_info_nivel_plan *info = malloc(longitud_info);
        int offset = 0, tmp_size = 0;

        //TODO hacer el memset(paquete, '0', sizeof(t_paquete))??

    //saco la ip del planif.
        //averiguo la cantidad de bytes que tiene el string ip_plan
        for(tmp_size = 1; (data)[tmp_size - 1] != '\0'; tmp_size ++);

        info->ip_Plan = malloc(tmp_size);

        memcpy(info->ip_Plan,data, tmp_size);

        offset += tmp_size;

    //saco el puerto del planif.
        tmp_size = sizeof(int32_t);
        memcpy(&info->puerto_Plan,data+offset, tmp_size);

        offset += tmp_size;

    //saco la ip del nivel
        //averiguo la cantidad de bytes que tiene el string ip_nivel
        for(tmp_size = 1; (data + offset)[tmp_size - 1] != '\0'; tmp_size ++);

        info->ip_Nivel = malloc(tmp_size);

        memcpy(info->ip_Nivel,data+offset, tmp_size);

        offset += tmp_size;

    //saco el puerto del nivel
        tmp_size = sizeof(int32_t);
        memcpy(&info->puerto_Nivel,data+offset, tmp_size);

        return info;
}


t_info_datos_nivel *deserializar_Info_Datos_Nivel(char* data, size_t longitud_info_nivel)
{
 t_info_datos_nivel *info = malloc(longitud_info_nivel);
    int offset = 0, tmp_size = 0;

   //se recibe el nombre del nivel
    for(tmp_size = 1; (data)[tmp_size - 1] != '\0'; tmp_size ++);

            info->nombre_Nivel = malloc(tmp_size);

            memcpy(info->nombre_Nivel,data, tmp_size);

            offset += tmp_size;


    //se recibe la ip del nivel
    for(tmp_size = 1; (data+offset)[tmp_size - 1] != '\0'; tmp_size ++);

            info->ip_Nivel = malloc(tmp_size);

            memcpy(info->ip_Nivel,data+offset, tmp_size);

            offset += tmp_size;


    //se recibe el puerto del nivel
            tmp_size = sizeof(int32_t);
            memcpy(&info->puerto_Nivel,data+offset, tmp_size);

            return info;
}

t_datosParaRecursosLiberados *deserializar_Info_Recursos_Liberados(char* datos_serializados,size_t longitud_datos)
{
	t_datosParaRecursosLiberados *datosParaRecursos = malloc(longitud_datos);
	datosParaRecursos->listaRecursosLibres = list_create();

	int offset = 0, tmp_size = 0;

	//agarro el nombre del nivel
	for(tmp_size = 1; (datos_serializados)[tmp_size - 1] != '\0'; tmp_size ++);

	 datosParaRecursos->nombreNivel = malloc(tmp_size);

	 memcpy(datosParaRecursos->nombreNivel, datos_serializados, tmp_size);

	 offset += tmp_size;

	 //agarro el simbolo del personaje
	 for(tmp_size = 1; (datos_serializados+offset)[tmp_size - 1] != '\0'; tmp_size ++);

	 datosParaRecursos->simboloPersonaje = malloc(tmp_size);

	 memcpy(datosParaRecursos->simboloPersonaje, datos_serializados + offset, tmp_size);

	 offset += tmp_size;

	 //creo un nodo por cada letra
	 while(offset < longitud_datos)
	 {
		 for(tmp_size = 1; (datos_serializados+offset)[tmp_size - 1] != '\0'; tmp_size ++);

		 char* recurso = malloc(1);

		 memcpy(recurso, datos_serializados + offset, tmp_size);

	//	 printf("RECURSO QUE DESERIALIZO: %s\n", recurso);

		 list_add(datosParaRecursos->listaRecursosLibres, recurso);

		 offset += tmp_size;

	 }

	// printf("CANTIDAD DE NODOS QUE HICE: %d\n", list_size(datosParaRecursos->listaRecursosLibres));
	 return datosParaRecursos;

}

char* serializar_Recursos_Asignados(t_list *listaRecursosAsignados, size_t longitud_datos_asignados, int cantRecursosAsignados)
{
	int offset = 0, tmp_size = 0;
	int cantNodosSacados = 0;
	char* stream = malloc(longitud_datos_asignados);
	t_recursos_asignados *unPj = malloc(sizeof(t_recursos_asignados));

	while (cantNodosSacados < cantRecursosAsignados)
	{

		unPj= list_remove(listaRecursosAsignados,0);

		//agrego el simbolo del personaje
		tmp_size =  strlen(unPj->simboloPj) + 1;
		memcpy(stream + offset, unPj->simboloPj, tmp_size);

		offset += tmp_size;

		//agrego el recurso asignado a dicho personaje
		tmp_size = strlen(unPj->recursoAsignado) + 1;
		memcpy(stream + offset, unPj->recursoAsignado, tmp_size);

//		printf("EL RECURSO: %s, FUE ASIGNADO A: %s", unPj->recursoAsignado, unPj->simboloPj);
		offset += tmp_size;

		cantNodosSacados++;
	}

	return stream;
}


char* serializar_Recursos_Libre(t_list* listaRecursosLibres, size_t longitud_datos_libres, int cantRecursosLibres)
{
	int offset = 0, tmp_size = 0;
	int cantNodosSacados = 0;
	char* stream = malloc(longitud_datos_libres);
	char* recursoLibre = malloc(sizeof(char));

	while (cantNodosSacados < cantRecursosLibres)
	{
		recursoLibre = list_remove(listaRecursosLibres,0);
//		printf("RECURSO QUE VOY A LIBERAR: %s\n", recursoLibre);

		//agrego el recurso libre
		tmp_size = strlen(recursoLibre) + 1;
		memcpy(stream + offset, recursoLibre, tmp_size);

		offset += tmp_size;

		cantNodosSacados++;
	}
	return stream;
}

t_list *deserializar_Personajes_Bloqueados(char *datos_serializados, size_t longitud_datos)
{

	int tmp_size = 0, offset = 0;
	 t_list *listaDeSimbolosBloqueados = list_create();

	while(offset < longitud_datos)
	{
		char* simbolo = malloc(sizeof(char));
	for(tmp_size = 1; (datos_serializados+offset)[tmp_size - 1] != '\0'; tmp_size ++);

	 memcpy(simbolo, datos_serializados + offset, tmp_size);

	// printf("RECIBI EL SIMBOLO BLOQUEADO: %s\n", simbolo);

	 list_add(listaDeSimbolosBloqueados, simbolo);

	 offset += tmp_size;
	}

	return listaDeSimbolosBloqueados;

}
