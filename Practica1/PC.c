/**********************************************************************
 * Projecto:                Práctica 1 de SO II
 * 
 * Nombre del programa:     PC.c
 * 
 * Autor:                   Juan Mena   
 * 
 * Fecha de creación:       18/02/2019
 * 
 * Proposito:               Crear el documento con la nota
 *                          necesaria para tener de media un 5
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
#include <errno.h>
#include <unistd.h>

#define NOTAMAXIMA 10

float crearDocumento(FILE *p_fp);

int main(int argc, char *argv[]) {

    FILE  *p_fp;
    float nota_media = 0;
    char  msg_media[32];
    
    if((p_fp = fopen("estudiantes.txt", "r")) == NULL){
        fprintf(stderr, "[PC] ERROR, ha ocurrido un fallo al abrir el archivo 'estudiantes.txt'\n");
        exit(EXIT_FAILURE);
    }
    else {
        nota_media = crearDocumento(p_fp);
        sprintf(msg_media, "%1.2f", nota_media);
        write(atoi(argv[0]), msg_media, strlen(msg_media)+1);
    }
    fclose(p_fp);
    return EXIT_SUCCESS;
}

//Este método crea el documento con la nota necesaria para tener media con al menos 5 puntos
float crearDocumento(FILE *p_fp){
    char    dni[1024];
    char    *p_carpeta_destino     = "";
    char    *nota                  = "";
    char    frase_documento[1024]  = "La nota que debes obtener en este nuevo exámen para superar la prueba es";
    int     nota_necesaria         = 0;
    float   suma_notas             = 0;
    int     contador               = 0;
    FILE    *p_nota_alumno;

    while (fgets(dni, 1024, p_fp)) {
        p_carpeta_destino = strtok(dni, " ");
        strtok(NULL, " ");
        nota = strtok(NULL, " ");
        nota_necesaria = NOTAMAXIMA - atoi(nota);
        suma_notas += atoi(nota);
        strcat(p_carpeta_destino, "/Nota necesaria.txt");
        if((p_nota_alumno = fopen(p_carpeta_destino, "w")) == NULL) {
            fprintf(stderr, "[PC] ERROR, ha ocurrido un fallo al crear el archivo de nota %s \n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        if (fprintf(p_nota_alumno, "%s %d", frase_documento, nota_necesaria) == -1) {
                fprintf(stderr, "[PC] ERROR, ha ocurrido un fallo al escribir en el archivo de nota %s \n", strerror(errno));
                exit(EXIT_FAILURE);
        }
        fclose(p_nota_alumno);
        contador++;
    }
    return suma_notas/contador;
}