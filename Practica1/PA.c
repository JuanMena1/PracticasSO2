/**********************************************************************
 * Projecto:                Práctica 1 de SO II
 * 
 * Nombre del programa:     PA.c
 * 
 * Autor:                   Juan Mena   
 * 
 * Fecha de creación:       18/02/2019
 * 
 * Proposito:               Creación de un directorio nombrado 
 *                          por el dni de cada alumno
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
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

void crearDirectorios(FILE *p_fp);

int main (int argc, char *argv[]) {
    FILE *p_fp; 
    char mensaje_notifica[1024] = "Creación de directorios, finalizada.";
    
    if((p_fp = fopen("estudiantes.txt", "r")) == NULL){
        fprintf(stderr, "[PA] ERROR, ha ocurrido un fallo al abrir el archivo 'estudiantes.txt'\n");
        exit(EXIT_FAILURE);
    }
    else {
        crearDirectorios(p_fp);
        write(atoi(argv[0]), mensaje_notifica, strlen(mensaje_notifica)+1);
    }
    fclose(p_fp);
return EXIT_SUCCESS;
}

//Este método crea los directorios de cada alumno
void crearDirectorios(FILE *p_fp) {
        char dni[1024];
        while (fgets(dni, 1024, p_fp)) {
            char *p_nombre_carpeta = strtok(dni, " ");
            if(mkdir(p_nombre_carpeta, 0777) == -1) {
                fprintf(stderr, "[PA] ERROR, ha ocurrido un fallo al crear la carpeta del alumno %s \n", strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
}