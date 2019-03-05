/**********************************************************************
 * Projecto:                Práctica 1 de SO II
 * 
 * Nombre del programa:     PB.c
 * 
 * Autor:                   Juan Mena   
 * 
 * Fecha de creación:       18/02/2019
 * 
 * Proposito:               Copiar el modelo de exámen 
 *                          correspondiente a cada alumno
 * 
 * Historial de revisión:
 * 
 * Fecha        Autor       Ref         Revisión
 * 04/03/2019   Juan Mena   1           Solucionar errores de buenos   
 *                                      hábitos de programación
 * 
 * ********************************************************************/ 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void copiarExamenes(FILE *p_fp);

int main (int argc, char *argv[]) {
    FILE *p_fp;
    
    if((p_fp = fopen("estudiantes.txt", "r")) == NULL){
        fprintf(stderr, "[PB] ERROR, ha ocurrido un fallo al abrir el archivo 'estudiantes.txt'\n");
        exit(EXIT_FAILURE);
    }
    else {
        copiarExamenes(p_fp);
    }
    fclose(p_fp);
    return EXIT_SUCCESS;
}

//Este método copia cada modelo de exámen en el directorio de cada alumno
void copiarExamenes(FILE *p_fp){
    char dni[1024];
    char *p_tipo_examen       = "";
    char *p_carpeta_destino   = "";
    char *p_carpeta_origen    = "Examenes/";
    char *p_formato           = ".pdf";
    char comando[1024];
    
    while (fgets(dni, 1024, p_fp)) {
        p_carpeta_destino = strtok(dni, " ");
        p_tipo_examen = strtok(NULL, " ");
        strcat(p_tipo_examen, p_formato);
        sprintf(comando, "%s %s%s %s/%s", "cp", p_carpeta_origen, p_tipo_examen, p_carpeta_destino, p_tipo_examen);
        if(system(comando)==-1){
            fprintf(stderr, "[PB] ERROR, ha ocurrido un fallo al realizar a copia del modelo de exámen\n");
            exit(EXIT_FAILURE);
        }
    }
}