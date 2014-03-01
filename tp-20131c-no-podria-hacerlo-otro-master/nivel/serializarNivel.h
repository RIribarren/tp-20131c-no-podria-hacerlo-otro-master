#ifndef SERIALIZARNIVEL_H_
#define SERIALIZARNIVEL_H_


#include "iniciarNivel.h"


typedef struct {
        int8_t id;
        int16_t size_data;
        char *data;
} __attribute__ ((packed)) t_paquete;

#define SIZE_CABECERA (sizeof(int8_t) + sizeof(int16_t)) //la cabecera se compone de un id y del tamaño del mensaje que se va a enviar

typedef struct{
        char* nombre_Nivel;
        char* ip_Nivel;
        int32_t puerto_Nivel;
} __attribute__ ((packed)) t_info_datos_nivel;

typedef struct{
	int8_t posX;
	int8_t posY;
	char* recurso;
} __attribute__ ((packed)) t_datosParaPedirInstancia;



t_paquete *crear_Paquete(int8_t id, char *data, int16_t size_data);

int enviar_Paquete(int socket,t_paquete *paquete);

t_paquete *recibir_Paquete(int socket);

char *serializar_Paquete(t_paquete *paquete,size_t *longitud_Stream);

t_info_datos_nivel *deserealizar_Info_Nivel(char* data, size_t longitud_info_nivel);

char *serializar_Datos_Nivel(t_info_datos_nivel *info, size_t longitud_info_nivel);

t_paquete *deserializar_Cabecera(char* data);

char *serializar_Destino(t_destino *destino, size_t longitud_destino);

t_destino *deserializar_Destino(char *data, size_t longitud_Destino);

t_datosParaPedirInstancia *deserializar_Info_Pedir_Instancia(char* info_serializada, size_t longitud_info);

char *serializar_Respuesta_Otorgado(char *stringRespuesta,size_t longitud_respuesta);

char* serializar_Recursos_Para_Orquestador(t_personaje *personaje, size_t longitud_datos, int cantRecursosGanados, char* nombreNivel);

char* serializar_Personajes_Bloqueados(t_list *listaPersonajesBloqueados,size_t longitud_datos);

t_list *deserializar_Recursos_Libres(char *datos_serializados,size_t longitud_datos);

t_list *deserializar_Recursos_Asignados(char* datos_serializados, size_t longitud_datos);

#endif /* SERIALIZARNIVEL_H_ */
