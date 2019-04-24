/**********************************************************************
 * Projecto:                Práctica 3 de SO II
 * 
 * Nombre del programa:     Cine.cpp
 * 
 * Autor:                   Juan Mena   
 * 
 * Fecha de creación:       01/04/2019
 * 
 * Proposito:               Simulación de un cine con una taquilla 
 *                          para la compra de tickets y unos puntos
 *							de venta para las palomitas y bebidas.
 *
 * 
 * Historial de revisión:
 * 
 * Fecha        Autor       Ref         Revisión
 * 01/04/2019   Juan Mena   1           Creación del projecto e inicio 
 *                                      de la Fase I de la práctica
 *
 * 15/04/2019	Juan Mena	2			Creación de la clase cliente e 
 *										implementación de pagarTicekts
 *
 * 23/04/2019	Juan Mena 	3			Pagar y ventana tickets funcionan
 *										correctamente
 *
 * *******************************************************************/

#include <iostream>
#include <thread>
#include <string>
#include <queue>
#include <chrono>
#include <mutex>
#include <functional>
#include <algorithm>
#include <condition_variable>
#include "Cliente.cpp"
#include "PeticionTicket.cpp"
#include "Colores.h"

#define NUM_HILOS 10

//Clase cliente con comprar tickets, comprar palomitas

struct PuntoVenta {
	int cantidad_palomitas;
	int cantidad_bebidas;
};

int asientos_sala = 72;
std::mutex sem_tickets;
std::mutex sem_taquilla;
std::mutex sem_pagar;
std::mutex sem_palomitas;
std::vector <Cliente> vector_clientes;
std::queue <std::thread> cola_tickets;
std::queue <std::thread> cola_palomitas;
std::queue <PeticionTicket> cola_peticiones_taquilla;
std::condition_variable cv;
int turno = 0;


/*void reponedorPalomitas(){
	while(1){
		pv.cantidad_palomitas=;
		pv.cantidad_bebidas=;
		std::this_thread::sleep_for (std::chrono::milliseconds(3000));
	}
}*/

void pagarTarjeta(){
	while(1){
		sem_pagar.lock();
		std::this_thread::sleep_for (std::chrono::milliseconds(500));
		std::cout <<"Pago realizado"<< std::endl;
	}
}

void servicioPalomitas(PuntoVenta &pv){
	while(1){
		if(pv.cantidad_bebidas==0 || pv.cantidad_palomitas==0){
			sem_reponedor.unlock();
		}else{
			pv.cantidad_bebidas -= 
			pv.cantidad_palomitas -=
		}
	}
}

void servicioTaquilla(){
	std::cout <<"Abre la taquilla"<< std::endl;
	while(1){
		sem_taquilla.lock();
		Peticion p = cola_peticiones_taquilla.front();
		cola_peticiones_taquilla.pop();

		if((asientos_sala - p.getNumTickets()) > 0) {
			asientos_sala -= p.getNumTickets();
			std::cout << YELLOW <<"Hilo " <<p.getId()<< " ha comprado los tickets"<< RESET <<std::endl;
		} else {
			std::cout << RED <<"El número de tickets es mayor que los asientos libres"<< RESET <<std::endl;
		}
	}
}

void Cliente::comprarTickets(){
	std::cout << "Hilo " << this->id << " llega a la cola de los tickets" << std::endl;
	std::unique_lock<std::mutex> lk_tickets(sem_tickets);
	//std::unique_lock<std::mutex> lk_pagar(sem_pagar, std::defer_lock);
	
	//std::lock(lk_taquilla, lk_pagar);

	std::this_thread::sleep_for (std::chrono::milliseconds(1000));
	std::cout << CYAN << "Hilo " << this->id << " entra a taquilla para comprar " << this->num_tickets << " tickets" << RESET<<std::endl;

	cola_peticiones_taquilla.push(Peticion(this->id, this->num_tickets));
	
	sem_taquilla.unlock();
	sem_pagar.unlock();
	//cola_palomitas.push(std::move(cola_tickets.front()));
	//cola_tickets.pop();
	//cola_palomitas.front() = &Cliente::comprarPalomitas();
}

void Cliente::comprarPalomitas(){
	std::cout << "Hilo " << this->id << " llega a la cola de las palomitas" << std::endl;
	std::unique_lock<std::mutex> lk_palomitas(sem_palomitas);
	//std::unique_lock<std::mutex> lk_pagar(sem_pagar, std::defer_lock);
	
	//std::lock(lk_palomitas, lk_pagar);
	std::cout << "Hilo " << this->id << " entra a taquilla para comprar " << this->num_bebidas << " bebidas y " <<this->num_palomitas << " palomitas" << std::endl;

}

int main(int argc, char *argv[]) {
	int i;

	sem_taquilla.lock();
	sem_pagar.lock();

	for(i=0;i<NUM_HILOS;i++)
		vector_clientes.push_back(Cliente(i+1, (rand() % 5)+1, (rand() % 5)+1, (rand() % 5)+1));

	for(i=0;i<NUM_HILOS;i++){
		cola_tickets.push(std::thread(&Cliente::comprarTickets, &vector_clientes[i]));
	 	std::this_thread::sleep_for (std::chrono::milliseconds(100));

	//std::this_thread::sleep_for (std::chrono::milliseconds(2000));
	}

	std::thread Taquilla(servicioTaquilla);
	std::thread Pagar(pagarTarjeta);

	/*for(i=0;i<NUM_HILOS;i++){
		turno = i+1;
		cv.notify_all();
	}*/

	for(i=0;i<NUM_HILOS;i++){
		cola_tickets.front().join();
		cola_tickets.pop();
	}

	/*for(i=0;i<NUM_HILOS;i++){
		cola_palomitas.front().join();
		cola_palomitas.pop();
	}*/

	Taquilla.detach();
	Pagar.detach();
	return 0;
}