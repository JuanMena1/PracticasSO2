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
 * 04/05/2019	Juan Mena	4			Añadida variable de condicion pagar
 *										controlar los Puntos de Venta
 *
 * 07/0502019	Juan Mena	5			Añadidos turnos para las colas y 
 *										manejador de interrupción Crtl + C
 *
 * *******************************************************************/

#include <iostream>
#include <thread>
#include <queue>
#include <chrono>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <algorithm>
#include "Cliente.cpp"
#include "PeticionTicket.cpp"
#include "PeticionComidaBebida.cpp"
#include "Colores.h"
#include <signal.h>
#include <unistd.h>


struct InfoPuntoVenta {
	int id;
	int cantidad_palomitas;
	int cantidad_bebidas;
	int cantidad_maxima;
};

struct SolicitudPago {
	int num_cliente;
	int tipo_pago;
};

//Clase cliente con comprar tickets, comprar palomitas

int num_hilos = 10;
int asientos_sala = 30;
bool asientos_suficientes {true};
std::mutex sem_tickets;
std::mutex sem_taquilla;
std::mutex sem_pagar;
std::mutex sem_palomitas;
std::mutex sem_puntoventa;
std::mutex sem_reponedor;
std::mutex s_manager_tickets;
std::mutex s_manager_palomitas;

std::condition_variable cv_puntos_venta;
std::condition_variable cv_tickets;
std::condition_variable cv_palomitas;

std::vector <Cliente> vector_clientes;
std::vector <std::thread> sala;

std::queue <std::thread> cola_hilos_tickets;
std::queue <std::thread> cola_hilos_palomitas;
std::queue <PeticionTicket> cola_peticiones_taquilla;
std::queue <PeticionComidaBebida> cola_peticiones_palomitas;
std::queue <InfoPuntoVenta*> cola_peticiones_reponer;
std::queue <SolicitudPago> cola_pagos;

int turno_tickets = 0;
int turno_palomitas = 0;

void reponer(){
	while(1){
		sem_reponedor.lock();
		InfoPuntoVenta *info_reponer = cola_peticiones_reponer.front();
		cola_peticiones_reponer.pop();
		std::cout <<RED<< "[REPONEDOR]\t Solicitud de reponer" <<RESET<<std::endl;
		info_reponer->cantidad_bebidas = info_reponer->cantidad_maxima;
		info_reponer->cantidad_palomitas = info_reponer->cantidad_maxima;
		std::this_thread::sleep_for (std::chrono::milliseconds(1500));
		std::cout << RED<<"[REPONEDOR]\t Repuestos " << info_reponer->cantidad_palomitas <<" palomitas y " << info_reponer->cantidad_bebidas <<" bebidas de Punto de Venta "<<info_reponer->id << RESET <<std::endl;		
		std::this_thread::sleep_for (std::chrono::milliseconds(500));		
		sem_puntoventa.unlock();
	}
}
/*Variable de condicion*/
void pagarTarjeta(){
	while(1){
		sem_pagar.lock();
		SolicitudPago sp = cola_pagos.front();
		cola_pagos.pop();
		if(sp.tipo_pago == 1){
			std::cout << BOLDYELLOW <<"[REALIZAR_PAGO]\t Hilo "<< sp.num_cliente << " paga sus tickets"<< RESET <<std::endl;
			s_manager_tickets.unlock();
		}
		else{
			std::cout << BOLDYELLOW <<"[REALIZAR_PAGO]\t Hilo "<< sp.num_cliente << " paga sus palomitas y bebidas"<< RESET <<std::endl;
			s_manager_palomitas.unlock();
		}
	}
}

void PuntoVenta(InfoPuntoVenta& pv){
	std::cout << "[PUNTO_VENTA " << pv.id << "]\t Abre Punto de venta "<< pv.id<<" con " << pv.cantidad_palomitas << " palomitas y " << pv.cantidad_bebidas << " bebidas" << std::endl;
	while(1){
		std::unique_lock<std::mutex> lk_puntoventa(sem_puntoventa);
		cv_puntos_venta.wait(lk_puntoventa, []{return !cola_peticiones_palomitas.empty();});

		PeticionComidaBebida pcb = cola_peticiones_palomitas.front();
		cola_peticiones_palomitas.pop();
		//std::cout <<YELLOW << "[PUNTO_VENTA " << pv.id << "]\t Hilo " << pcb.getId() << " entra a Punto de Venta " << pv.id <<RESET<< std::endl;
		std::this_thread::sleep_for (std::chrono::milliseconds(1000));		
		if(pv.cantidad_bebidas - pcb.getNumBebidas() <0 || pv.cantidad_palomitas - pcb.getNumPalomitas()<0){
			cola_peticiones_reponer.push(&pv);

			sem_reponedor.unlock();
			sem_puntoventa.lock();
			std::cout << CYAN << "[PUNTO_VENTA " << pv.id << "]\t Reposición realizada"<< RESET <<std::endl;
			pv.cantidad_bebidas -= pcb.getNumBebidas();
			pv.cantidad_palomitas -= pcb.getNumPalomitas();
			std::cout << BLUE << "[PUNTO_VENTA " << pv.id << "]\t Punto de venta " << pv.id << " Quedan " << pv.cantidad_palomitas << " palomitas y " << pv.cantidad_bebidas << " bebidas" << RESET <<std::endl;

		}else{
			pv.cantidad_bebidas -= pcb.getNumBebidas();
			pv.cantidad_palomitas -= pcb.getNumPalomitas();
			std::cout << BLUE << "[PUNTO_VENTA " << pv.id << "]\t Punto de venta " << pv.id << " Quedan " << pv.cantidad_palomitas << " palomitas y " << pv.cantidad_bebidas << " bebidas" << RESET <<std::endl;
		}
		lk_puntoventa.unlock();
		sem_palomitas.unlock();
	}
}

void servicioTaquilla(){
	std::cout <<"[TAQUILLA]\t Abre la taquilla"<< std::endl;
	while(1){
		sem_taquilla.lock();
		PeticionTicket p = cola_peticiones_taquilla.front();
		cola_peticiones_taquilla.pop();
		std::this_thread::sleep_for (std::chrono::milliseconds(250));		
		if((asientos_sala - p.getNumTickets()) >= 0) {
			asientos_sala -= p.getNumTickets();
			std::cout << YELLOW << "[TAQUILLA]\t Hilo " <<p.getId()<< " ha comprado " << p.getNumTickets()<<" tickets"<< RESET <<std::endl;
			if(!asientos_suficientes)
				asientos_suficientes = true;
			
		} else {
			std::cout  << BOLDRED<< "[C_TICKETS]\t Hilo " << p.getId() << " quiere más asientos de los que hay libres" << RESET<<std::endl;
			asientos_suficientes = false;
		}
		sem_tickets.unlock();
	}
}
void Cliente::comportamientoCliente(){
	Cliente::comprarTickets(this->id);
	if(asientos_suficientes){
		cola_hilos_palomitas.push(std::move(cola_hilos_tickets.front()));
		cola_hilos_tickets.pop();
		Cliente::comprarPalomitas(this->id);
		std::this_thread::sleep_for (std::chrono::milliseconds(500));
		std::cout << BOLDMAGENTA << "[GENERAL]\t Hilo " << this->id << " entra a la sala a ver una peli" << RESET <<std::endl;
	}else{
		std::cout << BOLDMAGENTA << "[GENERAL]\t Hilo " << this->id << " se va del cine" << RESET <<std::endl;
	}
}

void Cliente::comprarTickets(int const id){
	std::cout << "[C_TICKETS]\t Hilo " << this->id << " llega a la cola de los tickets" << std::endl;
	std::unique_lock<std::mutex> lk_tickets(sem_tickets);

	cv_tickets.wait(lk_tickets, [id]{return(turno_tickets==id);});
	std::cout <<BOLDWHITE <<"[C_TICKETS]\t ** Turno para tickets " << this->id << " **"<< RESET<<std::endl;

	std::this_thread::sleep_for (std::chrono::milliseconds(1000));
	std::cout << CYAN << "[C_TICKETS]\t Hilo " << this->id << " entra a taquilla para comprar " << this->num_tickets << " tickets" << RESET<<std::endl;

	cola_peticiones_taquilla.push(PeticionTicket(this->id, this->num_tickets));
	sem_taquilla.unlock();
	sem_tickets.lock();
	
	if(asientos_suficientes){
		cola_pagos.push(SolicitudPago{this->id,1});
		sem_pagar.unlock();
		std::this_thread::sleep_for (std::chrono::milliseconds(500));
	} else {
		s_manager_tickets.unlock();
	}
}

void Cliente::comprarPalomitas(int const id){
	std::cout  << GREEN << "[C_PALOMITAS]\t Hilo " << this->id << " llega a la cola de las palomitas" << RESET<<std::endl;
	std::unique_lock<std::mutex> lk_palomitas(sem_palomitas);
	cv_palomitas.wait(lk_palomitas, [id]{return(turno_palomitas==id);});
	std::cout <<BOLDWHITE <<"[C_PALOMITAS]\t -- Turno para palomitas " << this->id << " --"<< RESET<<std::endl;

	std::this_thread::sleep_for (std::chrono::milliseconds(1000));
	std::cout << "[C_PALOMITAS]\t Hilo " << this->id << " va a comprar " << this->num_palomitas << " palomitas y " <<this->num_bebidas << " bebidas" << std::endl;

	cola_peticiones_palomitas.push(PeticionComidaBebida(this->id, this->num_palomitas, this->num_bebidas));
	cola_pagos.push(SolicitudPago{this->id,2});

	cv_puntos_venta.notify_one();
	sem_palomitas.lock();
	sem_pagar.unlock();
}

void manejador(int sig){
    signal(sig, manejador);
    std::cout << BOLDRED << "\n[MANEJADOR]\t Señal Ctrl+C recibida. Sacabó el cine" << RESET << std::endl;
    kill(getpid(), SIGKILL);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
	int i;

	if(signal(SIGINT, manejador) == SIG_ERR) {
    	fprintf(stderr, "[MANAGER]\t ERROR, fallo en la captura de la señal Ctrl+C\n");
    	exit(EXIT_FAILURE);
    }

	sem_taquilla.lock();
	sem_pagar.lock();
	sem_reponedor.lock();
	s_manager_tickets.lock();
	s_manager_palomitas.lock();

	std::thread Taquilla(servicioTaquilla);
	std::this_thread::sleep_for (std::chrono::milliseconds(100));
	std::thread Pagar(pagarTarjeta);

	InfoPuntoVenta pv1={1,10,10,10};
	std::thread PuntoVenta1(PuntoVenta, std::ref(pv1));
	std::this_thread::sleep_for (std::chrono::milliseconds(100));

	InfoPuntoVenta pv2={2,8,8,8};
	std::thread PuntoVenta2(PuntoVenta, std::ref(pv2));
	std::this_thread::sleep_for (std::chrono::milliseconds(100));

	InfoPuntoVenta pv3={3,7,7,7};
	std::thread PuntoVenta3(PuntoVenta, std::ref(pv3));
	std::thread Reponedor(reponer);

	for(i=0;i<num_hilos;i++)
		vector_clientes.push_back(Cliente(i+1, (rand() % 5)+1, (rand() % 5)+1, (rand() % 5)+1));

	for(i=0;i<num_hilos;i++){
		cola_hilos_tickets.push(std::thread(&Cliente::comportamientoCliente, &vector_clientes[i]));
	 	std::this_thread::sleep_for (std::chrono::milliseconds(100));
	}

	while(asientos_sala != 0){
		if(!cola_hilos_tickets.empty()){
			turno_tickets += 1;
			cv_tickets.notify_all();
			s_manager_tickets.lock();
		}
		if(!cola_hilos_palomitas.empty() || asientos_suficientes){
			turno_palomitas += 1;
			cv_palomitas.notify_all();
			s_manager_palomitas.lock();
			sala.push_back(std::move(cola_hilos_palomitas.front()));
			cola_hilos_palomitas.pop();
		}
		if(!asientos_suficientes){
			turno_palomitas+=1;
		}
		if(((rand()%10)+1) > 7){
			vector_clientes.push_back(Cliente(num_hilos+1, (rand() % 5)+1, (rand() % 5)+1, (rand() % 5)+1));
			cola_hilos_tickets.push(std::thread(&Cliente::comportamientoCliente, &vector_clientes[num_hilos]));
	 		std::this_thread::sleep_for (std::chrono::milliseconds(100));
			num_hilos += 1;
		}
	}

	//Taquilla.detach();
	//Pagar.detach();
	//PuntoVenta1.detach();
	//PuntoVenta2.detach();
	//PuntoVenta3.detach();
	//Reponedor.detach();

	Pagar.join();
	return 0;
}