/**********************************************************************
 * Projecto:                Práctica 2 de SO II
 * 
 * Nombre del programa:     SSOOIIGLE.c
 * 
 * Autor:                   Juan Mena   
 * 
 * Fecha de creación:       18/03/2019
 * 
 * Proposito:               Realización de un buscador de palabras dado 
 *                          un documento y un nº de hilos para ello
 * 
 * Historial de revisión:
 * 
 * Fecha        Autor       Ref         Revisión
 * 18/03/2019   Juan Mena   1           Apertura del fichero, contar lineas del archivo, 
 *                                      creación de la estructura 'HiloBusqueda' y creación 
 *                                      del vector de hilos y el vector de objetos                                      
 *
 * 21/03/2019   Juan Mena   2           Búsqueda de palabras en una línea y asignación
 *                                      de cada parte del documento a cada hilo
 *
 * 23/03/2019   Juan Mena   3           Tratado del documento para elimiar signos
 *                                      de puntuación y pasado a minúsculas
 *
 * 28/03/2019   Juan Mena   4           Creación de una cola en la estructura,
 *                                      paso por referencia y colores añadidos
 *
 * 01/04/2019   Juan Mena   5           Añadidos comentarios y renombrado
 *                                      de variables
 * ********************************************************************/ 

#include <iostream>
#include <thread>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>
#include <functional>
#include <mutex>
#include <iterator>
#include <sstream>

struct HiloBusqueda {
    int id_hilo;
    int comienzo_hilo;
    int final_hilo;
    std::queue <std::string> resultado_busqueda;
};

    std::mutex                  sem;
    std::vector<std::thread>    v_hilos;                 //Vector para gurdar los hilos creados
    std::vector<HiloBusqueda>   v_objetosHilo;           //Vector para guardar las instancias de la estructura HiloBusqueda
    int                         apariciones_palabra = 0; //Veces que se ha encontrado la palabra

//Elimina los simbolos de puntuación que pueda tener una línea
std::string eliminarSimbolos(std::string linea){ 
  
    for (int i = 0, len = linea.size(); i < len; i++) { 
        // comprueba si el caracter es un signo de puntuacion
        if (ispunct(linea[i])) { 
            linea.erase(i--, 1); 
            len = linea.size(); 
        } 
    } 
    return linea;
}

//Busca la palabra en las líneas correspondientes a cada hilo
void buscarPalabras(std::string nombre_documento, std::string palabra_busqueda, HiloBusqueda& h){ 
    std::ifstream            file(nombre_documento);
    std::string              linea;                     //Línea del documento con la que se está trabajando
    int                      contador_lineas = 1;       //Nº de linea en la que estamos trabajando
    std::vector<std::string> palabras_linea;            //Nº de palabras de la línea con la que trabajamos  
    std::string              palabra_anterior;
    std::string              palabra_posterior;

   while(contador_lineas <= h.final_hilo){
        getline(file, linea);
        if(contador_lineas >= h.comienzo_hilo){
            linea = eliminarSimbolos(linea);

            //Transforma la línea a minúsculas
            std::transform(linea.begin(), linea.end(), linea.begin(), ::tolower);

                std::istringstream iss(linea);
                std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), back_inserter(palabras_linea));

                for(int i=0; i<palabras_linea.size();i++){
                    if(palabras_linea[i] == palabra_busqueda){
                        //Comprueba si la palabra es la primera en la línea
                        if(i-1 <0)
                            palabra_anterior = "*";
                        else
                            palabra_anterior = palabras_linea[i-1];

                        //Comprueba si la palabra es la última en la línea
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
    std::ifstream   file(nombre_documento); 
    int             num_lineas = 0;             //Lineas que tiene el documento
    std::string     linea;                      //Línea del documento

    while(!file.eof()){ 
        getline(file,linea); 
        num_lineas++;
    }
    file.close();
    return num_lineas;
}

//Imprime los resultados de la búsqueda realizada por los hilos
void imprimir_resultados(std::string palabra_busqueda){
    for (HiloBusqueda h : v_objetosHilo) {
        while(!h.resultado_busqueda.empty()){
            std::cout << "\033[1m\033[37m" << "[Hilo " +  std::to_string(h.id_hilo) + " inicio:" + std::to_string(h.comienzo_hilo) 
            + " - final:" + std::to_string(h.final_hilo) + "] :: " << "\033[0m";
            std::cout << "\033[1m\033[32m" << h.resultado_busqueda.front() <<"\033[0m" << std::endl;
            h.resultado_busqueda.pop();
        }
    }

    std::cout << "\033[35m"<< "La palabra '" << palabra_busqueda << "' se ha encontrado " << apariciones_palabra << " veces" << "\033[0m"<<std::endl;
    std::cout << "FIN DEL PROGRAMA" << std::endl;
}

int main(int argc, char *argv[]){

    //Comprueba si se han introducido los 4 parámetros necesarios
    if(argc != 4) {
        std::cout << "\033[31m" << "ERROR, se debe ejecutar con las opciones <nombre_documento> <palabra> <num_hilos>" << "\033[0m" <<std::endl;
        exit(EXIT_FAILURE);
    }

    std::string nombre_documento        = argv[1];
    std::string palabra_busqueda        = argv[2];
    int         num_hilos               = atoi(argv[3]);
    int         num_lineas_documento    = 0;             
    int         tamano_bloque           = 0;             //Tamaño de las particiones excepto la última
    int         i;

    std::cout << "INICIO DEL PROGRAMA" << std::endl;

    num_lineas_documento =  contarLineasDocumento(nombre_documento);
    tamano_bloque        =  num_lineas_documento/num_hilos;


    for(i=1; i<num_hilos;i++){ //Se crean todos los hilos excepto el último
        v_objetosHilo.push_back(HiloBusqueda {i, tamano_bloque*(i-1)+1, tamano_bloque*i});
        v_hilos.push_back(std::thread(buscarPalabras,nombre_documento, palabra_busqueda, std::ref(v_objetosHilo[i-1])));
        std::this_thread::sleep_for (std::chrono::milliseconds(100));
    }
    //Se crea el último
    if(i==num_hilos){
        v_objetosHilo.push_back(HiloBusqueda {i, tamano_bloque*(i-1)+1, num_lineas_documento});
        v_hilos.push_back(std::thread(buscarPalabras, nombre_documento, palabra_busqueda, std::ref(v_objetosHilo[i-1])));
    }

    for_each(v_hilos.begin(), v_hilos.end(), std::mem_fn(&std::thread::join));
    imprimir_resultados(palabra_busqueda);

    return 0;
}