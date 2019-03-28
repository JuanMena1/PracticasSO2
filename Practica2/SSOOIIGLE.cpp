/**********************************************************************
 * Projecto:                Práctica 2 de SO II
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
 * 28/03/2019   Juan Mena   1           Paso de las estructuras por referencia
 *                                      y colas locales en estas
 * ********************************************************************/ 

#include <iostream>
#include <thread>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>
#include <functional>
#include <chrono>
#include <mutex>
#include <iterator>
#include <sstream>

struct HiloBusqueda {
    int id_hilo;
    int comienzo_hilo;
    int final_hilo;
    std::queue <std::string> resultado_busqueda;
};

    std::mutex sem;
    std::vector<std::thread> v_hilos;
    std::vector<HiloBusqueda> v_objetosHilo;
    //std::queue <std::string> resultado_busqueda;
    int apariciones_palabra = 0;

//Este método elimina los imbolos de puntuación que pueda tener una línea
std::string eliminarSimbolos(std::string linea){ 
  
    for (int i = 0, len = linea.size(); i < len; i++) { 
        // ccomprueba si el caracter es un signo de puntuacion
        if (ispunct(linea[i]))/* || linea[i] == "¡" || linea[i] == "¿"*/ { 
            linea.erase(i--, 1); 
            len = linea.size(); 
        } 
    } 
    return linea;
}

//Busca la palabra en las lineas correspondientes a cada hilo
void buscarPalabras(std::string nombre_documento, std::string palabra_busqueda, HiloBusqueda& h){ 
    std::ifstream file(nombre_documento);
    std::string linea;
    int contador_lineas = 1;
    std::vector<std::string> palabras_linea;
    std::string palabra_anterior;
    std::string palabra_posterior;

   while(contador_lineas <= h.final_hilo){
        getline(file, linea);
        if(contador_lineas >= h.comienzo_hilo){
            linea = eliminarSimbolos(linea);
            //Transforma la linea a minúsculas
            std::transform(linea.begin(), linea.end(), linea.begin(), ::tolower);

                std::istringstream iss(linea);
                std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), back_inserter(palabras_linea));

                for(int i=0; i<palabras_linea.size();i++){
                    if(palabras_linea[i] == palabra_busqueda){
                        //Compruebo si la palabra está la primera en la línea
                        if(i-1 <0)
                            palabra_anterior = "*";
                        else
                            palabra_anterior = palabras_linea[i-1];

                        //Compruebo si la palabra está la última en la línea
                        if(i == palabras_linea.size()-1)
                            palabra_posterior = "*";
                        else
                            palabra_posterior=palabras_linea[i+1];

                        h.resultado_busqueda.push("línea " + std::to_string(contador_lineas) + " :: ... "
                        + palabra_anterior + " " + palabras_linea[i] + " " + palabra_posterior + " ...");

                        sem.lock();
                        apariciones_palabra++;
                        sem.unlock();
                    }
                }
        }
        contador_lineas++;
        palabras_linea.clear();
    }
}

//Cuenta las líneas que tiene el documento
int contarLineasDocumento(std::string nombre_documento){
    int lineas = 0;
    std::ifstream file(nombre_documento);
    std::string linea;
    std::string palabra;

    while(!file.eof()){ 
        getline(file,linea); 
        lineas++;
    }
    file.close();
    return lineas;
}

//Imprime los resultados de la busqueda realizada por los hilos
void imprimir_resultados(std::string palabra_busqueda){
    for (HiloBusqueda h : v_objetosHilo) {
        while(!h.resultado_busqueda.empty()){
            std::cout << "\033[1m\033[37m" << "[Hilo " +  std::to_string(h.id_hilo) + " inicio:" + std::to_string(h.comienzo_hilo) 
            + " - final:" + std::to_string(h.final_hilo) + "] :: " << "\033[0m";
            std::cout << "\033[1m\033[37m" << h.resultado_busqueda.front() <<"\033[0m" << std::endl;
            h.resultado_busqueda.pop();
        }
    }

    std::cout << "\033[35m"<< "La palabra " << palabra_busqueda << " se ha encontrado " << apariciones_palabra << " veces" << "\033[0m"<<std::endl;
    std::cout << "FIN DEL PROGRAMA" << std::endl;
}

int main(int argc, char *argv[]){

    if(argc != 4) {
        std::cout << "\033[31m" << "ERROR, se debe ejecutar con las opciones <nombre_documento> <palabra> <num_hilos>" << "\033[0m" <<std::endl;
        exit(1);
    }

    std::string  nombre_documento    = argv[1];
    std::string  palabra_busqueda    = argv[2];
    int     num_hilos                = atoi(argv[3]);
    int     lineas_documento         = 0;
    int     tamano_bloque            = 0;
    int     i;

    std::cout << "INICIO DEL PROGRAMA" << std::endl;

    lineas_documento = contarLineasDocumento(nombre_documento);
    tamano_bloque=lineas_documento/num_hilos;

    for(i=1; i<num_hilos;i++){
        v_objetosHilo.push_back(HiloBusqueda {i, tamano_bloque*(i-1)+1, tamano_bloque*i});
        v_hilos.push_back(std::thread(buscarPalabras,nombre_documento, palabra_busqueda, std::ref(v_objetosHilo[i-1])));
        std::this_thread::sleep_for (std::chrono::milliseconds(100));
    }
    if(i==num_hilos){
        v_objetosHilo.push_back(HiloBusqueda {i, tamano_bloque*(i-1)+1, lineas_documento});
        v_hilos.push_back(std::thread(buscarPalabras, nombre_documento, palabra_busqueda, std::ref(v_objetosHilo[i-1])));
    }

    for_each(v_hilos.begin(), v_hilos.end(), std::mem_fn(&std::thread::join));
    imprimir_resultados(palabra_busqueda);

    return 0;
}