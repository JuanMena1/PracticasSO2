/**********************************************************************
 * Projecto:                Práctica 1 de SO II
 * 
 * Nombre del programa:     PD.c
 * 
 * Autor:                   Juan Mena   
 * 
 * Fecha de creación:       18/02/2019
 * 
 * Proposito:               Eliminar el material creado
 *                          por la ejecución de los demás procesos
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
#include <string.h>
#include <unistd.h>
#include <errno.h>

void borrarMaterial(FILE *p_fp);

int main(int argc, char *argv[]) {
    FILE *p_fp;

     if((p_fp = fopen("estudiantes.txt", "r")) == NULL){
        fprintf(stderr, "[PD] ERROR, ha ocurrido un fallo al abrir el archivo 'estudiantes.txt'\n");
        exit(EXIT_FAILURE);
    } else {
        borrarMaterial(p_fp);
    }
    return 0;
}

//Este método borra los directorios y archivos creados por el programa
void borrarMaterial(FILE *p_fp){
    char comando[1024];
    char dni[1024];

    while (fgets(dni, 1024, p_fp)) {
        char *p_nombre_carpeta = strtok(dni, " ");
        sprintf(comando, "%s %s", "rm -r", p_nombre_carpeta);
        if(system(comando) == -1) {
            fprintf(stderr, "[PD] ERROR, ha ocurrido un fallo al borrar la carpeta del alumno %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
}