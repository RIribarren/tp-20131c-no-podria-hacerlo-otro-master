#include <stdlib.h>
#include "memoria.h"
#include <string.h>
#include <stdbool.h>



t_list* listaParticiones; //Lista de particiones administrativa.






int buscarParticionAdecuada(t_memoria segmento,size_t longSegmento); // No se usa.
int espacioMaximoParticion (t_memoria segmento, size_t longSegmento); // No se usa.
int seRepiteID(t_list* lista,char idAAlmacenar);
int hayParticionLibre(t_list* lista,int longParticion);
void agregarParticion(t_list* listaParticiones,t_memoria segmento,char id,int tamanio,char* contenido);
void copiarContenidoAlSegmento(t_memoria segmento, t_memoria contenido,int comienzoParticion);
void liberarParticion(t_list* lista,t_memoria segmento,char id);
int estaLaParticion(t_list* lista,char idAEliminar);

int agarrarPosicionDelID(t_list* lista,char id); // No se usa.
void eliminarContenidoDelSegmento(t_memoria segmento,t_particion* particionAEliminar);
t_particion* traerParticion(int inicio);
t_particion* armarParticionVacia(t_memoria segmento,int inicio); // Tampoco se usa.





t_memoria crear_memoria(int tamanio) {



	t_particion* particionInicial = (t_particion*) malloc (sizeof(t_particion));
	t_memoria segmento;
	int i;

    segmento = (char*) malloc(tamanio+1);

     listaParticiones = list_create();

        particionInicial->dato = segmento;
	    particionInicial->id = '*';
		particionInicial->libre = 1;
		particionInicial->tamanio = tamanio;
		particionInicial->inicio = 0;

		list_add(listaParticiones,particionInicial);


   for (i = 0; i<tamanio; i++){
		segmento[i]= '*';
	}
    segmento[i] = '\0';
	   return segmento;
}

int almacenar_particion(t_memoria segmento, char id, int tamanio, char* contenido) {

	int longSegmento = strlen(segmento);


   if ((longSegmento < tamanio) || (seRepiteID(listaParticiones,id)) ){

			return -1;
		}

   if (hayParticionLibre(listaParticiones,tamanio) == 0){
	   return 0;
   }

   agregarParticion(listaParticiones,segmento,id,tamanio,contenido);

   return 1;

	}

int eliminar_particion(t_memoria segmento, char id) {

    if (estaLaParticion(listaParticiones,id)) {

	liberarParticion(listaParticiones,segmento,id);
	return 1;
}
  else

return 0;

}


/*	t_particion* particionAEliminar = (t_particion*) malloc(sizeof(t_particion));
	int posicionDelID = agarrarPosicionDelID(listaParticiones,id);

	 if(posicionDelID >= 0){

		 particionAEliminar = list_remove(listaParticiones,posicionDelID);
		 eliminarContenidoDelSegmento(segmento,particionAEliminar);

		free(particionAEliminar);
		return 1;

	}

	 else
		 free(particionAEliminar);
		 return 0;

*/


void liberar_memoria(t_memoria segmento) {

	free(segmento);
    free(listaParticiones);
}

t_list* particiones(t_memoria segmento) {

	t_list* list = list_create();
	t_particion* particion = (t_particion*) malloc(sizeof(t_particion));
    int sig= 0;
	int longitudSegmento = strlen(segmento);

  while (sig < longitudSegmento) {

	  particion = traerParticion(sig);

	t_particion* particionDefinitiva = (t_particion*) malloc(sizeof(t_particion));

	particionDefinitiva->id = particion->id;
	particionDefinitiva->inicio = particion->inicio;
    particionDefinitiva->libre = particion->libre;
    particionDefinitiva->tamanio = particion->tamanio;
    particionDefinitiva->dato = &segmento[particion->inicio];

      list_add(list, particionDefinitiva);

	   sig = sig+particionDefinitiva->tamanio;

    }

  return list;
}





int buscarParticionAdecuada(char* segmento,size_t longSegmento ) {

	int i=0;
	int posicionActual,posicionMaxima;
	int espacioActual,espacioMaximo;

	espacioMaximo = 0;
	posicionMaxima = 0;

	while(i <= longSegmento) {

		espacioActual = 0;

		while ( (i<=longSegmento) && (segmento[i] == '\0') ) {

		  if (posicionActual == 0) {
			  posicionActual = i; }

		  espacioActual++;
		  i++;
			}

		if (espacioActual >= espacioMaximo) {

			espacioMaximo = espacioActual;
			posicionMaxima = posicionActual;
		}

      }
    return posicionMaxima;
}

void copiarContenidoAlSegmento(char* segmento,char* contenido,int comienzoParticion){


	int i,j=0;
	size_t longContenido;

	longContenido = strlen(contenido);


	 for (i=comienzoParticion;j<longContenido;j++){
		segmento[i] = contenido[j];
		i++;
		}

}

int espacioMaximoParticion(char* segmento, size_t longSegmento){
	int i=0;

		int espacioActual,espacioMaximo;

		espacioMaximo = 0;


		while(i <= longSegmento) {

			espacioActual = 0;

			while ( (i<=longSegmento) && (segmento[i] == '\0') ) {

			  espacioActual++;
			  i++;
				}

			if (espacioActual >= espacioMaximo) {

				espacioMaximo = espacioActual;
				}

	      }
	    return espacioMaximo;
	}

int seRepiteID(t_list* lista, char idAAlmacenar) {

	t_particion* particion = (t_particion*) malloc(sizeof(t_particion));
	int sig = 0;
	int longitudLista = lista->elements_count;

	particion = list_get(lista,sig);

	while( (sig< longitudLista) && (particion->id != idAAlmacenar) ){

		sig++;
		particion=list_get(lista,sig);
	}

	if (sig >= longitudLista)

		return 0;

	else

		return 1;


}

int agarrarPosicionDelID(t_list* lista,char idAEliminar){

    int longitudLista = list_size(lista);
	int index = 0;
	t_particion* particion = (t_particion*) malloc(sizeof(t_particion));



	particion = NULL;

	 particion = list_get(lista,index);

	 while ( (index<(longitudLista-1)) && (particion->id != idAEliminar) ) {

		 index++;
	     particion=list_get(lista,index);

	    }



	 if ( (index >= (longitudLista-1) ) && (particion->id != idAEliminar) ) {

		  free(particion);
	      return -1;
       }
	   else

			   free(particion);
			   return index;

}


void eliminarContenidoDelSegmento(t_memoria segmento,t_particion* particion) {

 int i;

 for(i=particion->inicio;i<(particion->inicio+particion->tamanio);i++){

	 segmento[i] = '*';
 }



}

t_particion* traerParticion(int inicio) {

    t_particion* particion = (t_particion*) malloc (sizeof(t_particion));
    t_list* lista = listaParticiones;

    int sig = 0;

    particion = list_get (lista,sig);

      while (particion->inicio != inicio) {

    	 sig++;
         particion = list_get(lista,sig);

      }

    return particion;

}

t_particion* armarParticionVacia(t_memoria segmento,int inicio){

    t_particion* particion = (t_particion*) malloc (sizeof(t_particion));
    int tamanio=0;
    int i = inicio;


         while ( segmento[i] == '\0' ) {

        	   tamanio++;
        	   i++;
	        }


         particion->id = '\0';
         particion->inicio = inicio;
         particion->tamanio = tamanio;
         particion->dato = &segmento[inicio];
         particion->libre = 1 ;


    return particion;


}



int hayParticionLibre(t_list* lista,int longParticion) {

	t_particion* particion = (t_particion*) malloc(sizeof(t_particion));
	int sig = 0;
	int longitudLista = lista->elements_count;
	int hayParticion = 0;

	for (sig = 0;(sig<longitudLista) && (hayParticion == 0); sig++){

		particion = list_get(lista,sig);

		if ((particion->libre == 1) && (particion->tamanio>=longParticion)) {
			hayParticion =1;
		}
	}

	if (hayParticion) {

		return 1;
	}
	else


	 return 0;
	}

void agregarParticion(t_list* lista,t_memoria segmento,char id,int tamanio,char* contenido) {

    t_particion* particion = (t_particion*) malloc(sizeof(t_particion));
    t_particion* particionNueva = (t_particion*) malloc(sizeof(t_particion));
	int sig = 0;
	int posicionLista = 0;
	int longitudLista = lista->elements_count;
	int particionActual = 0;

	if (list_is_empty(lista)){
	particion = NULL;}

	  for (sig= 0; sig <longitudLista; sig++){

		particion = list_get(lista,sig);

		if( (particion->libre == 1) && (particion->tamanio > particionActual)) {

			particionActual = particion->tamanio;
			posicionLista = sig;
		}
	}



	  particion = list_get(lista,posicionLista);

	  copiarContenidoAlSegmento(segmento,contenido,particion->inicio);

	  if (particion->tamanio > tamanio){

      particionNueva->id = id;
	  particionNueva->dato = &segmento[particion->inicio];
      particionNueva->inicio = particion->inicio;
      particionNueva->libre = 0;
	  particionNueva->tamanio = tamanio;

      particion->tamanio = particion->tamanio - tamanio;
      particion->inicio = particion->inicio+tamanio;
      particion->dato = &segmento[particion->inicio+tamanio];

       list_add(lista,particionNueva);

	  }

	  else
	  {
		 particion->id = id;
		 particion->libre = 0;
		 particion->tamanio = tamanio;
	     particion->dato = &segmento[particion->inicio];
	  }

}


int estaLaParticion (t_list* lista, char idAEliminar) {

	t_particion* particion = (t_particion*) malloc(sizeof(t_particion));
	int longitudLista = lista->elements_count;
	int sig= 0;


	particion = list_get(listaParticiones,sig);


	while ((sig < longitudLista) && (particion->id != idAEliminar)) {

		sig++;
		particion=list_get(listaParticiones,sig);

	}

	if (sig>= longitudLista) {


		return 0;
	}
		else

	return 1;

}


void liberarParticion(t_list* lista,t_memoria segmento,char idAEliminar){

	 t_particion* particion = (t_particion*) malloc(sizeof(t_particion));
     int longitudLista = lista->elements_count;
	 int sig= 0;


		particion = list_get(listaParticiones,sig);


		while ((sig < longitudLista) && (particion->id != idAEliminar)) {

			sig++;
			particion=list_get(listaParticiones,sig);

		}

	eliminarContenidoDelSegmento(segmento,particion);

	particion->id = '*';
	particion->libre = 1;

}

