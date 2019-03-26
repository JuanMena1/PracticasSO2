#include <iostream>
#include <queue>

class HiloBusqueda{
    private:
        int id_hilo;
        int comienzo_hilo;
        int final_hilo;
        std::queue <std::string> informacion_busqueda;
    public:
    HiloBusqueda(int id, int comienzo, int final): id_hilo(id), comienzo_hilo(comienzo), final_hilo(final){};
    int getFinalHilo();
    int getComienzoHilo();
    //void buscarPalabras(std::string nombre_documento);
};
int HiloBusqueda::getComienzoHilo(){return comienzo_hilo;}
int HiloBusqueda::getFinalHilo() {return final_hilo;}
