#include "bibliotecaInstancia.h"



int main(void)
{

	pLog = log_create("instancia.log", "INSTANCIA", true, LOG_LEVEL_TRACE);
	log_trace(pLog, "Iniciando...");
	instanciaActiva = 1; //true++++++++++++++++++++++++++++++
	pthread_mutex_init(&m_mantenimiento, NULL);//+++++++++++++++++++++++++++++++++++++

	t_config * pConf = config_create("instancia.config");



	int coord_socket = conectarseACoordinador(pConf);
	log_trace(pLog, "Conectada a Coordinador");

		/////////////////////////////////////////////////////////////////
		//tamaño de las entradas q me diec el coordinador
		void * infoEntries;
		int bytes = recvWithBasicProtocol(coord_socket, &infoEntries);
		tBuffer * pBuff = makeBuffer(infoEntries, bytes);
		entryCant = 4;//readIntFromBuffer(pBuff);
		entrySize = 7;//readIntFromBuffer(pBuff);
		freeBuffer(pBuff);
		/////////////////////////////////////////////////////////////////


		cargarTablaDeEntradasYAlmacenamiento(pConf);
		log_trace(pLog, "Tabla de entradas cargada");
		log_trace(pLog, "Almacenamiento creado");

		if( strcmp(config_get_string_value(pConf, "ALG_REEMP"),"CIRC")) algReemp=CIRC;
		if( strcmp(config_get_string_value(pConf, "ALG_REEMP"),"LRU") )algReemp=LRU;
		if( strcmp(config_get_string_value(pConf, "ALG_REEMP"),"BSU") )algReemp=BSU;
		pathMontaje = config_get_string_value(pConf, "PTO_MONTAJE");

//		pthread_t hiloDump;
		//pthread_create(&hiloDump, NULL, (void*)&dump, (void*)pConf);

		while(1)
		{
			void * ordenDeAcceso;

			log_trace(pLog, "A la espera de solicitudes");
			int size= recvWithBasicProtocol(coord_socket, &ordenDeAcceso);
			if(size)
			{
				log_debug(pLog, "Solicitud recibida");

				int aviso = SOLICITUD_ESI_ATENDIENDOSE;
				sendWithBasicProtocol(coord_socket, &aviso, sizeof(int));
				log_trace(pLog, "Se informa al Coordinador que su solicitud esta siendo atendida");

				tBuffer * buffSent = makeBuffer(ordenDeAcceso, size);
				ESISentenciaParseada_t sent;

				sent.operacion = readIntFromBuffer(buffSent);
				if(sent.operacion == ORDEN_COMPACTAR)
					sent.clave = NULL;
				else
				sent.clave = readStringFromBuffer(buffSent);

				if(sent.operacion == SET)
				{
					sent.valor = readStringFromBuffer(buffSent);
					size = strlen(sent.valor)+1;
				}
				else
				{
					sent.valor = NULL;
					size = 0;
				}

				log_debug(pLog, "Sentencia recibida:\n op=%s, clave=%s, valor=%s\n", sent.operacion == SET ? "SET":sent.operacion==GET?"GET":sent.operacion==STORE?"STORE":"ORDEN_COMPACTAR", sent.clave, sent.valor?sent.valor:"No corresponde");

				rtdoEjec_t rtdo = accederRecurso(sent.operacion, sent.clave, sent.valor);
				log_debug(pLog, "El resultado de la sentencia fue %s", rtdo==SUCCESS?"SUCCESS":rtdo==FAILURE?"FAILURE":rtdo==FIN_DE_EJECUCION?"FIN DE EJECUCION":rtdo==DISCONNECTED?"DESCONECTADO":rtdo==NO_HAY_INSTANCIAS_CONECTADAS?"NO HAY INSTANCIAS CONECTADAS":rtdo==ABORTED?"ABORTADO":"SENTENCIA" );

				freeBuffer(buffSent);

				sendWithBasicProtocol(coord_socket, &rtdo, sizeof(rtdoEjec_t));
				log_trace(pLog, "Resultado enviado al Coordinador");
			}
			else
				break;
		}

		log_error(pLog, "Se ha cortado la conexion con el Coordinador\nCerrando instancia...");
		instanciaActiva = 0; //false
		//pthread_join(hiloDump, NULL);

		return 0;

	}
