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
 *										manejador de interrupción Ctrl + C
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

/*Estructura con la información del Punto de Venta*/
struct InfoPuntoVenta {
	int id;
	int cantidad_palomitas;
	int cantidad_bebidas;
	int cantidad_reponer;
};

/*Estructura para realizar los pagos*/
struct SolicitudPago {
	int num_cliente;
	int tipo_pago;
};

/*Variables Globales*/
int 								num_hilos 			= 		10;		/*Número de clientes que van a entrar al cine al principio*/
int 								asientos_libres 	= 		72;		/*Número de asientos libres de la sala*/
int 								turno_tickets 		= 		0;		/*Turno para la cola de los tickets*/
int 								turno_palomitas 	= 		0;		/*Turno para la cola de las palomitas y bebidas*/
bool 								asientos_suficientes 		{true};	/*Variable para saber si quedan asientos suficientes para asignar*/

std::mutex 							sem_tickets;						/*Semáforo para la cola de los tickets*/
std::mutex 							sem_taquilla;						/*Semáforo para la taquilla*/
std::mutex 							sem_pagar;							/*Semáforo para el sistema de pago*/
std::mutex 							sem_palomitas;						/*Semáforo para la cola de las palomitas y bebidas*/
std::mutex 							sem_puntoventa;						/*Semáforo para los puntos de venta*/
std::mutex 							sem_reponedor;						/*Semáforo para el reponedor*/
std::mutex 							s_manager_tickets;					/*Semáforo para los turnos de la cola de los tickets*/
std::mutex 							s_manager_palomitas;				/*Semáforo para los turnos de la cola de las palomitas y bebidas*/

std::condition_variable 			cv_puntos_venta;					/*Variable de condición para los accesos a los puntos de venta*/
std::condition_variable 			cv_tickets;							/*Variable de condición para los turnos de la cola de los tickets*/
std::condition_variable 			cv_palomitas;						/*Variable de condición para los turnos de la cola de las palomitas y bebidas*/

std::vector <Cliente> 				vector_clientes;					/*Vector para almacenar las instancias de la clase Cliente*/
std::vector <std::thread> 			sala_cine;							/*Vector para simular los clientes dentro de la sala*/
std::vector <std::thread> 			clientes_no_aceptados;				/*Vector para simular los clientes que se marchan por no poder darles los tickets que solicitan*/

std::queue 	<std::thread> 			cola_hilos_tickets;					/*Cola para pedir los tickets*/
std::queue 	<std::thread> 			cola_hilos_palomitas;				/*Cola para pedir las palomitas y bebidas*/
std::queue 	<PeticionTicket> 		cola_peticiones_taquilla;			/*Cola para realizar las peticiones de tickets*/
std::queue 	<PeticionComidaBebida> 	cola_peticiones_palomitas;			/*Cola para realizar las peticiones de palomitas y bebidas*/
std::queue 	<InfoPuntoVenta*> 		cola_peticiones_reponer;			/*Cola para solicitar reponer palomitas y bebidas de un punto de venta*/
std::queue 	<SolicitudPago> 		cola_pagos;							/*Cola para realizar los pagos*/

/*Se encarga de recargar las palomitas y bebidas de los 3 puntos de venta*/
void reponer(){
	while(1){
		sem_reponedor.lock();
		/*Se saca la primera petición*/
		InfoPuntoVenta *info_reponer = cola_peticiones_reponer.front();
		cola_peticiones_reponer.pop();

		std::cout <<RED<< "[REPONEDOR]\t Solicitud de reponer" <<RESET<<std::endl;

		info_reponer->cantidad_bebidas = info_reponer->cantidad_reponer;
		info_reponer->cantidad_palomitas = info_reponer->cantidad_reponer;
		std::this_thread::sleep_for (std::chrono::milliseconds(1500));

		std::cout << RED<<"[REPONEDOR]\t Repuestos " << info_reponer->cantidad_palomitas <<" palomitas y " << info_reponer->cantidad_bebidas <<" bebidas de Punto de Venta "<<info_reponer->id << RESET <<std::endl;		
		std::this_thread::sleep_for (std::chrono::milliseconds(500));		
		sem_puntoventa.unlock();
	}
}
/*Se encarga de realizar los pagos tanto de tickets (1) como de comida y bebida (2)*/
void pagarTarjeta(){
	while(1){
		sem_pagar.lock();
		/*Se saca la primera petición*/
		SolicitudPago sp = cola_pagos.front();
		cola_pagos.pop();

		if(sp.tipo_pago == 1){
			std::cout << BOLDYELLOW <<"[REALIZAR_PAGO]\t Hilo "<< sp.num_cliente << " paga sus tickets"<< RESET <<std::endl;
		}
		else{
			std::cout << BOLDYELLOW <<"[REALIZAR_PAGO]\t Hilo "<< sp.num_cliente << " paga sus palomitas y bebidas"<< RESET <<std::endl;
		}
	}
}
/*Se encarga de suministrar las palomitas y las bebidas a los clientes*/
void PuntoVenta(InfoPuntoVenta& pv){
	std::cout << BOLDBLUE<<"[PUNTO_VENTA_" << pv.id << "]\t Abre Punto de venta "<< pv.id<<" con " << pv.cantidad_palomitas << " palomitas y " << pv.cantidad_bebidas << " bebidas" <<RESET<< std::endl;
	while(1){
		std::unique_lock<std::mutex> lk_puntoventa(sem_puntoventa);
		cv_puntos_venta.wait(lk_puntoventa, []{return !cola_peticiones_palomitas.empty();});
		/*Se saca la primera petición*/
		PeticionComidaBebida pcb = cola_peticiones_palomitas.front();
		cola_peticiones_palomitas.pop();
		std::this_thread::sleep_for (std::chrono::milliseconds(1000));	

		if(pv.cantidad_bebidas - pcb.getNumBebidas() <0 || pv.cantidad_palomitas - pcb.getNumPalomitas()<0){
			cola_peticiones_reponer.push(&pv);

			sem_reponedor.unlock();
			sem_puntoventa.lock();
			std::cout << CYAN << "[PUNTO_VENTA_" << pv.id << "]\t Reposición realizada"<< RESET <<std::endl;
			pv.cantidad_bebidas -= pcb.getNumBebidas();
			pv.cantidad_palomitas -= pcb.getNumPalomitas();
			std::cout << BLUE << "[PUNTO_VENTA_" << pv.id << "]\t Punto de venta " << pv.id << " Quedan " << pv.cantidad_palomitas << " palomitas y " << pv.cantidad_bebidas << " bebidas" << RESET <<std::endl;

		}else{
			pv.cantidad_bebidas -= pcb.getNumBebidas();
			pv.cantidad_palomitas -= pcb.getNumPalomitas();
			std::cout << BLUE << "[PUNTO_VENTA_" << pv.id << "]\t Punto de venta " << pv.id << " Quedan " << pv.cantidad_palomitas << " palomitas y " << pv.cantidad_bebidas << " bebidas" << RESET <<std::endl;
		}
		lk_puntoventa.unlock();
		sem_palomitas.unlock();
	}
}
/*Se encarga de dar a los clientes los tickets que desean si hay hueco suficiente en la sala*/
void servicioTaquilla(){
	std::cout << YELLOW <<"\n[TAQUILLA]\t Abre la taquilla"<<RESET<< std::endl;
	while(1){
		sem_taquilla.lock();
		/*Se saca la primera petición*/
		PeticionTicket p = cola_peticiones_taquilla.front();
		cola_peticiones_taquilla.pop();
		std::this_thread::sleep_for (std::chrono::milliseconds(250));		

		if((asientos_libres - p.getNumTickets()) >= 0) {
			asientos_libres -= p.getNumTickets();
			std::cout << YELLOW << "[TAQUILLA]\t Hilo " <<p.getId()<< " ha comprado " << p.getNumTickets()<<" tickets"<< RESET <<std::endl;
			/*En caso de que llegue un cliente que quiera justo los asientos que
			quedan libres cuando ha llegado antes otro que quería más*/
			if(!asientos_suficientes)
				asientos_suficientes = true;
			
		} else {
			std::cout  << BOLDRED<< "[C_TICKETS]\t Hilo " << p.getId() << " quiere más asientos de los que hay libres" << RESET<<std::endl;
			asientos_suficientes = false;
		}
		sem_tickets.unlock();
	}
}
/*Es el comportamiento que tiene el cliente cuando llega al cine*/
void Cliente::comportamientoCliente(){
	Cliente::comprarTickets(this->id);

	if(asientos_suficientes){
		/*El cliente se cambia a la cola de palomitas y bebidas*/
		cola_hilos_palomitas.push(std::move(cola_hilos_tickets.front()));
		cola_hilos_tickets.pop();

		s_manager_tickets.unlock();

		Cliente::comprarPalomitas(this->id);
		std::this_thread::sleep_for (std::chrono::milliseconds(500));

		s_manager_palomitas.unlock();

		std::cout << BOLDMAGENTA << "[GENERAL]\t Hilo " << this->id << " entra a la sala a ver DEADPOOL" << RESET <<std::endl;
	}else{
		/*El cliente se va del cine y sale de la cola de los tickets*/
		clientes_no_aceptados.push_back(std::move(cola_hilos_tickets.front()));
		cola_hilos_tickets.pop();
		std::cout << BOLDMAGENTA << "[GENERAL]\t Hilo " << this->id << " se va del cine" << RESET <<std::endl;
	}
}
/*Conjunto de acciones que realiza el cliente cuando va a solicitar los tickets*/
void Cliente::comprarTickets(int const id){
	std::cout << "[C_TICKETS]\t Hilo " << this->id << " llega a la cola de los tickets" << std::endl;
	std::unique_lock<std::mutex> lk_tickets(sem_tickets);

	cv_tickets.wait(lk_tickets, [id]{return(turno_tickets==id);});

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
/*Conjunto de acciones que realiza el cliente cuando va a solicitar las palomitas y la bebida*/
void Cliente::comprarPalomitas(int const id){
	std::cout  << GREEN << "[C_PALOMITAS]\t Hilo " << this->id << " llega a la cola de las palomitas" << RESET<<std::endl;
	std::unique_lock<std::mutex> lk_palomitas(sem_palomitas);

	cv_palomitas.wait(lk_palomitas, [id]{return(turno_palomitas==id);});

	std::this_thread::sleep_for (std::chrono::milliseconds(1000));
	std::cout << "[C_PALOMITAS]\t Hilo " << this->id << " va a comprar " << this->num_palomitas << " palomitas y " <<this->num_bebidas << " bebidas" << std::endl;

	cola_peticiones_palomitas.push(PeticionComidaBebida(this->id, this->num_palomitas, this->num_bebidas));
	cola_pagos.push(SolicitudPago{this->id,2});

	cv_puntos_venta.notify_one();
	sem_palomitas.lock();
	sem_pagar.unlock();
}
/*Manejador de la señal de interrupción Ctrl+C*/
void manejador(int sig){
    signal(sig, manejador);
    std::cout << BOLDRED << "\n[MANEJADOR]\t Señal Ctrl+C recibida. Programa finalizado" << RESET << std::endl;
    kill(getpid(), SIGKILL);
    exit(EXIT_SUCCESS);
}

/*Método principal donde se crean los clientes, los hilos y donde se generan los turnos
tanto para la cola de los tickets como para la cola de las palomitas y la bebida*/
int main(int argc, char *argv[]) {
	int i;

	if(signal(SIGINT, manejador) == SIG_ERR) {
    	std::cout << BOLDRED <<"\n[GENERAL]\t ERROR, fallo en la captura de la señal Ctrl+C\n" << RESET << std::endl;
    	exit(EXIT_FAILURE);
    }

	sem_taquilla.		lock();
	sem_pagar.			lock();
	sem_reponedor.		lock();
	s_manager_tickets.	lock();
	s_manager_palomitas.lock();

	std::thread Taquilla(servicioTaquilla);
	std::this_thread::sleep_for (std::chrono::milliseconds(100));
	std::thread Pagar(pagarTarjeta);

	/*id, num_palomitas, num_bebida, cantidad_reponer*/
	InfoPuntoVenta pv1={1,	10,	10,	10};
	InfoPuntoVenta pv2={2,	8,	8,	8};
	InfoPuntoVenta pv3={3,	7,	7,	7};

	std::thread PuntoVenta1(PuntoVenta, std::ref(pv1));
	std::this_thread::sleep_for (std::chrono::milliseconds(100));

	std::thread PuntoVenta2(PuntoVenta, std::ref(pv2));
	std::this_thread::sleep_for (std::chrono::milliseconds(100));

	std::thread PuntoVenta3(PuntoVenta, std::ref(pv3));
	std::this_thread::sleep_for (std::chrono::milliseconds(100));

	std::thread Reponedor(reponer);

	/*Se crean las primeros instancias de la clase Cliente*/
	for(i=0;i<num_hilos;i++)
		vector_clientes.push_back(Cliente(i+1, (rand() % 5)+1, (rand() % 5)+1, (rand() % 5)+1));

	/*Se crean los primeros hilos para simular la llegada de clientes*/
	for(i=0;i<num_hilos;i++){
		cola_hilos_tickets.push(std::thread(&Cliente::comportamientoCliente, &vector_clientes[i]));
	 	std::this_thread::sleep_for (std::chrono::milliseconds(100));
	}

	while(asientos_libres != 0){
		if(!cola_hilos_tickets.empty()){
			turno_tickets++;
			std::cout <<BOLDWHITE <<"\n[C_TICKETS]\t ** Turno " << turno_tickets << " para tickets **\n"<< RESET<<std::endl;
			cv_tickets.notify_all();
			s_manager_tickets.lock();
		}
		if(!cola_hilos_palomitas.empty()){
			turno_palomitas++;
			std::cout <<BOLDWHITE <<"\n[C_PALOMITAS]\t -- Turno " << turno_palomitas << " para palomitas y bebidas --\n"<< RESET<<std::endl;
			cv_palomitas.notify_all();
			s_manager_palomitas.lock();
			sala_cine.push_back(std::move(cola_hilos_palomitas.front()));
			cola_hilos_palomitas.pop();
		}
		/*Se simula la llegada nuevos clientes*/
		if(((rand()%10)+1) > 6){
			vector_clientes.push_back(Cliente(num_hilos+1, (rand() % 5)+1, (rand() % 5)+1, (rand() % 5)+1));
			cola_hilos_tickets.push(std::thread(&Cliente::comportamientoCliente, &vector_clientes[num_hilos]));
	 		std::this_thread::sleep_for (std::chrono::milliseconds(100));
			num_hilos += 1;
		}
		/*En caso de que el cliente se haya ido sin entrar al cine*/
		if(!asientos_suficientes && !cola_hilos_tickets.empty()){
			turno_palomitas++;
		}
	}

	std::this_thread::sleep_for (std::chrono::milliseconds(1000));
	std::cout << BOLDBLUE << "\n[GENERAL]\t Todos los asientos están acupados y la película va a comenzar "<< RESET <<std::endl;

	Pagar.join();
	return 0;
}