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
#include "Cliente.cpp"
#include "PeticionTicket.cpp"
#include "PeticionComidaBebida.cpp"
#include "SemaforoContador.cpp"
#include "Colores.h"

#define NUM_HILOS 10


struct InfoPuntoVenta {
	int id;
	int cantidad_palomitas;
	int cantidad_bebidas;
};

struct SolicitudReponer {
	int id_pv;
	int cantidad_reponer;
};

struct SolicitudPago {
	int num_cliente;
	int tipo_pago;
};

//Clase cliente con comprar tickets, comprar palomitas

int asientos_sala = 72;
std::mutex sem_tickets;
std::mutex sem_taquilla;
std::mutex sem_pagar;
std::mutex sem_palomitas;
std::mutex sem_puntoventa;
std::mutex sem_reponedor;
SemaforoContador sem_cont(3);
std::vector <Cliente> vector_clientes;
std::queue <std::thread> cola_tickets;
std::queue <std::thread> cola_palomitas;
std::queue <PeticionTicket> cola_peticiones_taquilla;
std::queue <PeticionComidaBebida> cola_peticiones_palomitas;
std::queue <SolicitudReponer> cola_peticiones_reponer;
std::queue <SolicitudPago> cola_pagos;

InfoPuntoVenta pv;

void reponedorPalomitas(){
	while(1){
		sem_reponedor.lock();
		SolicitudReponer sr = cola_peticiones_reponer.front();
		cola_peticiones_reponer.pop();
		std::cout <<RED<< "Solicitud de reponer" <<RESET<<std::endl;
		pv.cantidad_bebidas = sr.cantidad_reponer;
		pv.cantidad_palomitas = sr.cantidad_reponer;
		std::this_thread::sleep_for (std::chrono::milliseconds(1500));
		//std::cout << "Repuestos "<<sr.cantidad_reponer <<" palomitas y " <<sr.cantidad_reponer << " bebidas" << RESET <<std::endl;
		std::cout << RED<<"Repuestos palomitas y bebidas de Punto de Venta "<<sr.id_pv << RESET <<std::endl;		
		std::this_thread::sleep_for (std::chrono::milliseconds(500));		
		sem_puntoventa.unlock();
	}
}

void pagarTarjeta(){
	while(1){
		sem_pagar.lock();
		std::this_thread::sleep_for (std::chrono::milliseconds(500));
		SolicitudPago sp = cola_pagos.front();
		cola_pagos.pop();
		if(sp.tipo_pago == 1){
			std::cout << BOLDYELLOW <<"Hilo "<< sp.num_cliente << " paga sus tickets"<< RESET <<std::endl;
		}
		else{
			std::cout << BOLDYELLOW <<"Hilo "<< sp.num_cliente << " paga sus palomitas y bebida"<< RESET <<std::endl;
		}
	}
}
/*¿Hacer que compruebe si la cola está vacía cada vez?*/
void servicioPalomitas(int id, int cantidad_maxima){
	pv={id,cantidad_maxima, cantidad_maxima};
	std::cout << "Abre Punto de venta "<< id<<" con " << pv.cantidad_palomitas << " palomitas y " << pv.cantidad_bebidas << " bebidas" << std::endl;
	while(1){
		sem_puntoventa.lock();
		PeticionComidaBebida pcb = cola_peticiones_palomitas.front();
		cola_peticiones_palomitas.pop();
		std::this_thread::sleep_for (std::chrono::milliseconds(1000));		
		if(pv.cantidad_bebidas - pcb.getNumBebidas() <0 || pv.cantidad_palomitas - pcb.getNumPalomitas()<0){
			SolicitudReponer sr{id, cantidad_maxima};
			cola_peticiones_reponer.push(std::ref(sr));
			sem_reponedor.unlock();
			sem_puntoventa.lock();
			pv.cantidad_bebidas -= pcb.getNumBebidas();
			pv.cantidad_palomitas -= pcb.getNumPalomitas();
			std::cout << BLUE << "Quedan " << pv.cantidad_palomitas << " palomitas y " << pv.cantidad_bebidas << " bebidas" << RESET <<std::endl;

		}else{
			pv.cantidad_bebidas -= pcb.getNumBebidas();
			pv.cantidad_palomitas -= pcb.getNumPalomitas();
			std::cout << BLUE << "Quedan " << pv.cantidad_palomitas << " palomitas y " << pv.cantidad_bebidas << " bebidas" << RESET <<std::endl;
		}
		sem_palomitas.unlock();
	}
}

void servicioTaquilla(){
	std::cout <<"Abre la taquilla"<< std::endl;
	while(1){
		sem_taquilla.lock();
		PeticionTicket p = cola_peticiones_taquilla.front();
		cola_peticiones_taquilla.pop();
		std::this_thread::sleep_for (std::chrono::milliseconds(1000));		
		if((asientos_sala - p.getNumTickets()) > 0) {
			asientos_sala -= p.getNumTickets();
			std::cout << YELLOW << "Hilo " <<p.getId()<< " ha comprado " << p.getNumTickets()<<" tickets"<< RESET <<std::endl;
		} else {
			std::cout << RED <<"El número de tickets es mayor que los asientos libres"<< RESET <<std::endl;
		}
		sem_tickets.unlock();
	}
}
void Cliente::comportamientoCliente(){
	Cliente::comprarTickets();
	/*cola_palomitas.push(std::move(cola_tickets.front()));
	cola_tickets.pop();*/
	Cliente::comprarPalomitas();
	std::cout << BOLDMAGENTA<< "Hilo " << this->id << " entra a la sala" << RESET <<std::endl;
	std::this_thread::sleep_for (std::chrono::milliseconds(1000));
}

void Cliente::comprarTickets(){
	std::cout << "Hilo " << this->id << " llega a la cola de los tickets" << std::endl;
	std::unique_lock<std::mutex> lk_tickets(sem_tickets);
	//std::unique_lock<std::mutex> lk_pagar(sem_pagar, std::defer_lock);
	
	//std::lock(lk_taquilla, lk_pagar);

	std::this_thread::sleep_for (std::chrono::milliseconds(1000));
	std::cout << CYAN << "Hilo " << this->id << " entra a taquilla para comprar " << this->num_tickets << " tickets" << RESET<<std::endl;

	cola_peticiones_taquilla.push(PeticionTicket(this->id, this->num_tickets));
	cola_pagos.push(SolicitudPago{this->id,1});

	sem_taquilla.unlock();
	sem_pagar.unlock();
	sem_tickets.lock();
}

void Cliente::comprarPalomitas(){
	std::cout << "Hilo " << this->id << " llega a la cola de las palomitas" << std::endl;
	std::lock_guard<std::mutex> lk_palomitas(sem_palomitas);
	//std::unique_lock<std::mutex> lk_pagar(sem_pagar, std::defer_lock);
	
	//std::lock(lk_palomitas, lk_pagar);
	std::this_thread::sleep_for (std::chrono::milliseconds(1000));
	std::cout << "Hilo " << this->id << " entra a taquilla para comprar " << this->num_palomitas << " palomitas y " <<this->num_bebidas << " bebidas" << std::endl;

	cola_peticiones_palomitas.push(PeticionComidaBebida(this->id, this->num_palomitas, this->num_bebidas));
	cola_pagos.push(SolicitudPago{this->id,2});

	sem_puntoventa.unlock();
	sem_pagar.unlock();
	sem_palomitas.lock();
}

int main(int argc, char *argv[]) {
	int i;

	sem_taquilla.lock();
	sem_pagar.lock();
	sem_puntoventa.lock();
	sem_reponedor.lock();

	for(i=0;i<NUM_HILOS;i++)
		vector_clientes.push_back(Cliente(i+1, (rand() % 5)+1, (rand() % 5)+1, (rand() % 5)+1));

	for(i=0;i<NUM_HILOS;i++){
		cola_tickets.push(std::thread(&Cliente::comportamientoCliente, &vector_clientes[i]));
	 	std::this_thread::sleep_for (std::chrono::milliseconds(100));
	}

	std::thread Taquilla(servicioTaquilla);
	std::thread Pagar(pagarTarjeta);
	std::thread PuntoVenta1(servicioPalomitas, 1, 10);
	std::thread PuntoVenta2(servicioPalomitas, 2, 8);
	std::thread PuntoVenta3(servicioPalomitas, 3, 7);
	std::thread Reponedor(reponedorPalomitas);

	/*for(i=0;i<NUM_HILOS;i++){
		turno = i+1;
		cv.notify_all();
	}*/

	for(i=0;i<NUM_HILOS;i++){
		cola_tickets.front().join();
		cola_tickets.pop();
	}

	Taquilla.detach();
	Pagar.detach();
	PuntoVenta1.detach();
	PuntoVenta2.detach();
	PuntoVenta3.detach();
	Reponedor.detach();

	 std::this_thread::sleep_for (std::chrono::milliseconds(5000));

	/*for(i=0;i<NUM_HILOS;i++){
		cola_palomitas.front().join();
		cola_palomitas.pop();
	}*/
	
	return 0;
}