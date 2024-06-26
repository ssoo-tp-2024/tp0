#include "client.h"

int main(void)
{
	/*---------------------------------------------------PARTE 2-------------------------------------------------------------*/

	int conexion;
	char *ip;
	char *puerto;
	char *valor;

	t_log *logger;
	t_config *config;

	/* ---------------- LOGGING ---------------- */

	// Inicio el log
	logger = iniciar_logger();

	// Usando el logger creado previamente
	log_debug(logger, "Log tipo [DEBUG] => No se muestra");
	log_trace(logger, "Log tipo [TRACE] => No se muestra");
	log_info(logger, "Log tipo [INFO] => Si se muestra");
	log_warning(logger, "Log tipo [WARNING] => Si se muestra");
	log_error(logger, "Log tipo [ERROR] => Si se muestra");

	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

	config = iniciar_config();

	// Usando el config creado previamente, leemos los valores del config y los
	// dejamos en las variables 'ip', 'puerto' y 'valor'
	if (!config_has_property(config, "IP") || !config_has_property(config, "PUERTO") || !config_has_property(config, "VALOR"))
	{
		perror("El archivo de configuracion no contiene las llaves necesarias para correr el cliente.");
		config_destroy(config);
		exit(EXIT_FAILURE);
	}
	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");
	valor = config_get_string_value(config, "VALOR");

	// Loggeamos el valor de config
	log_info(logger, "[IP]\t%s", ip);
	log_info(logger, "[PUERTO]\t%s", puerto);
	log_info(logger, "[VALOR]\t%s", valor);

	/* ---------------- LEER DE CONSOLA ---------------- */

	leer_consola(logger);

	/*---------------------------------------------------PARTE 3-------------------------------------------------------------*/

	// ADVERTENCIA: Antes de continuar, tenemos que asegurarnos que el servidor esté corriendo para poder conectarnos a él

	// Creamos una conexión hacia el servidor
	conexion = crear_conexion(ip, puerto);

	// Enviamos al servidor el valor de CLAVE como mensaje
	enviar_mensaje(valor, conexion);

	// Armamos y enviamos el paquete
	paquete(conexion);

	terminar_programa(conexion, logger, config);

	/*---------------------------------------------------PARTE 5-------------------------------------------------------------*/
	// Proximamente
}

t_log *iniciar_logger(void)
{
	char *file_name = "tp0.log";				// Nombre del archivo donde se guardaran los logs
	char *process_name = "SSOO";				// Nombre del proceso para loguear
	bool forward_logs_to_console = true;		// Reenviar logs a la consola ademas del archivo
	t_log_level min_log_level = LOG_LEVEL_INFO; // Minimo nivel para loguear

	t_log *nuevo_logger = log_create(file_name, process_name, forward_logs_to_console, min_log_level);
	if (nuevo_logger == NULL)
	{
		perror("No se pudo crear el archivo de log.");
		exit(EXIT_FAILURE);
	}

	return nuevo_logger;
}

t_config *iniciar_config(void)
{
	t_config *nuevo_config = config_create("./cliente.config");
	if (nuevo_config == NULL)
	{
		perror("No se pudo leer el archivo de configuración");
		exit(EXIT_FAILURE);
	}

	return nuevo_config;
}

void leer_consola(t_log *logger)
{
	char *leido = NULL;

	// La primera te la dejo de yapa
	do
	{
		if (leido != NULL)
			free(leido);
		leido = readline("> ");
		log_debug(logger, "Valor leido: %s", leido);
	} while (strcmp(leido, ""));

	// El resto, las vamos leyendo y logueando hasta recibir un string vacío

	// ¡No te olvides de liberar las lineas antes de regresar!
	free(leido);
}

void paquete(int conexion)
{
	// Ahora toca lo divertido!
	char *leido = NULL;
	t_paquete *paquete = crear_paquete();

	// Leemos y esta vez agregamos las lineas al paquete
	do
	{
		if (leido != NULL)
			free(leido);
		leido = readline("> ");
		if (strcmp(leido, ""))
			agregar_a_paquete(paquete, leido, strlen(leido) + 1);
	} while (strcmp(leido, ""));

	enviar_paquete(paquete, conexion);

	// ¡No te olvides de liberar las líneas y el paquete antes de regresar!
	eliminar_paquete(paquete);
	free(leido);
}

void terminar_programa(int conexion, t_log *logger, t_config *config)
{
	/* Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config)
	  con las funciones de las commons y del TP mencionadas en el enunciado */

	log_info(logger, "Terminando programa");

	// Libero el archivo de log
	log_destroy(logger);

	// Libero el socket con el server
	liberar_conexion(conexion);

	// Libero el archivo de configuración
	config_destroy(config);
}
