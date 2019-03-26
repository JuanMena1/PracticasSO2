/**********************************************************************
 * Projecto:                Práctica 1 de SO II
 * 
 * Nombre del programa:     SSOOIIGLE.c
 * 
 * Autor:                   Juan Mena   
 * 
 * Fecha de creación:       19/03/2019
 * 
 * Proposito:               Realización de un buscador de palabras dado 
 *                          un documento y un nº de hilos para ello
 * 
 * Historial de revisión:
 * 
 * Fecha        Autor       Ref         Revisión
 * 26/03/2019   Juan Mena   1           Encuentra varias instancias   
 *                                      de una palabra en la misma línea
 * 
 * ********************************************************************/ 

#include <iostream>
#include <thread>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>
#include <functional>


class HiloBusqueda{
    private:
        int id_hilo;
        int comienzo_hilo;
        int final_hilo;
        std::queue <std::string> informacion_busqueda;
    public:
    HiloBusqueda(int id, int comienzo, int fin): id_hilo(id), comienzo_hilo(comienzo), final_hilo(fin){};
    int getFinalHilo();
    int getComienzoHilo();
    void buscarPalabras(std::string nombre_documento, std::string palabra_busqueda);
};
int HiloBusqueda::getComienzoHilo(){return comienzo_hilo;}
int HiloBusqueda::getFinalHilo() {return final_hilo;}


std::vector<std::thread> v_hilos;
std::vector<HiloBusqueda> v_objetoHilo;

void buscarPalabras(std::string nombre_documento, std::string palabra_busqueda){    
    std::ifstream file(nombre_documento);
    std::string linea;
    int contador_lineas = 0;

   /* while(contador_lineas <= HiloBusqueda::getFinalHilo()){
        contador_lineas++;
        if(contador_lineas >= HiloBusqueda::getComienzoHilo()){*/
    while(!file.eof()){
        contador_lineas++;
            /*Buscar la palabra en la linea*/
            getline(file, linea);
            size_t pos = linea.find(palabra_busqueda, 0);
            while(pos != std::string::npos){
                std::cout << "Palabra '" << palabra_busqueda << "' encontrada en la línea " << contador_lineas << std::endl;
                pos = linea.find(palabra_busqueda,pos+1);
            }
        }
    }
//}

/*¿Tiene que ser un hilo que cree un objeto hiloBusqueda?*/

void crearHilos(int num_hilos, int tamano_bloque, int total_lineas, std::string nombre_documento, std::string palabra_busqueda){
    int i;
    for(i=1; i<num_hilos;i++){
        v_objetoHilo.push_back(HiloBusqueda (i, tamano_bloque*(i-1)+1, tamano_bloque*i));
        std::cout << "Hilo " << i << " línea "<< tamano_bloque*(i-1)+1 << " hasta " << tamano_bloque*i << std::endl;
        v_hilos.push_back(std::thread(buscarPalabras,nombre_documento, palabra_busqueda ));
    }
    if(i==num_hilos){
        v_objetoHilo.push_back(HiloBusqueda(i, tamano_bloque*(i-1)+1,total_lineas));
        std::cout << "Hilo " << i << " línea "<< tamano_bloque*(i-1)+1 << " hasta " <<total_lineas << std::endl;
        v_hilos.push_back(std::thread(buscarPalabras, nombre_documento, palabra_busqueda));
    }
}

int contarLineasDocumento(std::string nombre_documento){
    int lineas = 0;
    std::ifstream file(nombre_documento);
    std::string linea;
    std::string palabra;

    while(!file.eof()){ 
        getline(file,linea); 
        lineas++;
    }
    std::cout << "Lineas del archivo: " << lineas <<std::endl;
    file.close();
    return lineas;
}

int main(int argc, char *argv[]){

    if(argc != 4) {
        std::cout << "ERROR, se debe ejecutar con las opciones <nombre_documento> <palabra> <num_hilos>" << std::endl;
        exit(1);
    }

    std::string  nombre_documento    = argv[1];
    std::string  palabra_busqueda    = argv[2];
    int     num_hilos                = atoi(argv[3]);
    int     lineas_documento         = 0;

    lineas_documento = contarLineasDocumento(nombre_documento);
    crearHilos(num_hilos, lineas_documento/num_hilos, lineas_documento, nombre_documento, palabra_busqueda);

    for_each(v_hilos.begin(), v_hilos.end(), std::mem_fn(&std::thread::join));


    return 0;
}