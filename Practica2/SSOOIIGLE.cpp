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
    HiloBusqueda(int id, int comienzo, int final): id_hilo(id), comienzo_hilo(comienzo), final_hilo(final){};
};

std::vector <HiloBusqueda> v_hilos;

void crearHilos(int num_hilos, int tamano_bloque){
    for(int i=1; i<=num_hilos;i++){
        v_hilos.push_back(HiloBusqueda (i, tamano_bloque*(i-1)+1,tamano_bloque*i));
        std::cout << tamano_bloque*(i-1)+1 << " " << tamano_bloque*i << std::endl;
    }
}

int contarLineasDocumento(std::string nombre_documento){
    int lineas = 0;
    std::ifstream file;
    file.open(nombre_documento, std::ifstream::in);
    while(file.good())
        if(file.get()=='\n')
            lineas++;
    std::cout << "Lineas del archivo:" << lineas <<std::endl;
    file.close();
    return lineas;
}

void buscarPalabras(){

}

int main(int argc, char *argv[]){

    if(argc != 4) {
        std::cout << "ERROR, se debe ejecutar con las opciones <nombre_documento> <palabra> <num_hilos>" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string  nombre_documento    = argv[1];
    std::string  palabra_busqueda    = argv[2];
    int     num_hilos                = atoi(argv[3]);
    int     lineas_documento            = 0;

    lineas_documento = contarLineasDocumento(nombre_documento);
    crearHilos(num_hilos, lineas_documento/num_hilos);

    return 0;
}