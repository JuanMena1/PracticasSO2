/**********************************************************************
 * Projecto:                Práctica 1 de SO II
 * 
 * Nombre del programa:     Manager.c
 * 
 * Autor:                   Juan Mena   
 * 
 * Fecha de creación:       18/02/2019
 * 
 * Proposito:               Gestión de procesos para la
 *                          realización paralela de tareas
 * 
 * Historial de revisión:
 * 
 * Fecha        Autor       Ref         Revisión
 * 04/03/2019   Juan Mena   1           Solucionar errores de buenos   
 *                                      hábitos de programación
 * 
 * ********************************************************************/ 

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define LECTURA   0
#define ESCRITURA 1
#define NUM_HIJOS 3

void matarProcesos();
void manejador(int sig);
void crearDirectorios(char pa_tuberia[256]);
void copiarModeloExamen();
void crearDocumentoNota(char wrPc_tuberia[256]);
void borrarMaterial();

pid_t g_procesos[NUM_HIJOS];
FILE *gp_logFile;

int main(int argc, char *argv[]){

    int  tuberiaPcManager[2];
    int  tuberiaNotificarManager[2];
    char wrPc_tuberia[256];
    char pa_tuberia[256];
    char buffer_pc[256];
    char buffer_notifica[1024];

    pipe(tuberiaPcManager);
    pipe(tuberiaNotificarManager);

    sprintf(wrPc_tuberia, "%d", tuberiaPcManager[ESCRITURA]);
    sprintf(pa_tuberia, "%d", tuberiaNotificarManager[ESCRITURA]);

    if(signal(SIGINT, manejador) == SIG_ERR) {
        fprintf(stderr, "[MANAGER] ERROR, fallo en la captura de la señal Ctrl+C\n");
        exit(EXIT_FAILURE);
    }

    if((gp_logFile = fopen("log.txt", "w")) == NULL){
        fprintf(stderr, "[MANAGER] ERROR, ha ocurrido un fallo al crear el archivo 'log.txt'\n");
        exit(EXIT_FAILURE);
    }
    fprintf(gp_logFile, "*******Log del sistema*******\n");
    
    crearDirectorios(pa_tuberia);  
    //Esperamos a que termine el proceso PA
    wait(NULL);     
    read(tuberiaNotificarManager[LECTURA], buffer_notifica, sizeof(buffer_notifica));   //Esperamos el mensaje de fin de PA
    fprintf(gp_logFile, "%s\n", buffer_notifica);

    copiarModeloExamen();

    crearDocumentoNota(wrPc_tuberia);
    //Esperamos a que terminen los g_procesos PB y PC
    wait(NULL);
    wait(NULL);

    fprintf(gp_logFile, "Copia de modelos de exámen, finalizada.\n");
    fprintf(gp_logFile, "Creación de archivos para alcanzar la nota de corte, finalizada.\n");
    read(tuberiaPcManager[LECTURA], buffer_pc, sizeof(buffer_pc));      //Leemos la nota media de la tuberia
    fprintf(gp_logFile, "La nota media de la clase es: %s\n", buffer_pc);
    
    fprintf(gp_logFile, "*******FIN DEL PROGRAMA*******");
    fclose(gp_logFile); 
    printf("El programa ha finalizado con éxito.\n");
    return 0;
}

//Este método se encarga de matar a los procesos cuando ocurre una interrupción del usuario
void matarProcesos(){
    int i;
    for(i=0;i<NUM_HIJOS;i++){
        if(g_procesos[i]){
            kill(g_procesos[i], SIGKILL);
        }
    }
}

void manejador(int sig){
    signal(sig, manejador);
    printf("\nSeñal Ctrl+C recibida.\n");
    matarProcesos();
    fprintf(gp_logFile, "Se ha producido una interrupción del usuario 'Ctrl+C'");
    fclose(gp_logFile);
    borrarMaterial();
    exit(EXIT_SUCCESS);
}

//Este método se encarga de ejecutar el proceso PA
void crearDirectorios(char pa_tuberia[256]){
    if((g_procesos[0]=fork())==-1){
        fprintf(stderr, "[MANAGER] ERROR, ha ocurrido un fallo en el fork del proceso A, %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    } else if (g_procesos[0] == 0) {
        char *arg_list[] = {pa_tuberia};
        char *var_list[] = {NULL};

        execve("./PA", arg_list, var_list);
        fprintf(stderr, "[MANAGER] ERROR, ha ocurrido un error al ejecutar 'execve' del proceso A\n");
        exit(EXIT_FAILURE);
    }
    
}

//Este método se encarga de ejecutar el proceso PB
void copiarModeloExamen(){
        if((g_procesos[1]=fork())==-1){
        fprintf(stderr, "[MANAGER] ERROR, ha ocurrido un fallo en el fork del proceso B, %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    } else if (g_procesos[1] == 0) {
        char *arg_list[] = {NULL};
        char *var_list[] = {NULL};

        execve("./PB", arg_list, var_list);
        fprintf(stderr, "[MANAGER] ERROR, ha ocurrido un error al ejecutar 'execve' del proceso B\n");
        exit(EXIT_FAILURE);
    } 
}

//Este método se encarga de ejecutar el proceso PC
void crearDocumentoNota(char wrPc_tuberia[256]){
     if((g_procesos[2]=fork())==-1){
        fprintf(stderr, "[MANAGER] ERROR, ha ocurrido un fallo en el fork del proceso C, %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    } else if (g_procesos[2] == 0) {
        char *arg_list[] = {wrPc_tuberia};
        char *var_list[] = {NULL};

        execve("./PC", arg_list, var_list);
        fprintf(stderr, "[MANAGER] ERROR, ha ocurrido un error al ejecutar 'execve' del proceso C\n");
        exit(EXIT_FAILURE);
    }
}

//Este método se encarga de ejecutar el proceso PD
void borrarMaterial(){
    int pid_t;
    if((pid_t=fork())==-1){
        fprintf(stderr, "[MANAGER] ERROR, ha ocurrido un fallo en el fork del proceso D, %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    } else if (pid_t == 0) {
        char *arg_list[] = {};
        char *var_list[] = {NULL};
    
        execve("./PD", arg_list, var_list);
        fprintf(stderr, "[MANAGER] ERROR, ha ocurrido un error al ejecutar 'execve' del proceso D\n");
        exit(EXIT_FAILURE);
    } else {
        wait(NULL);
    }
}

