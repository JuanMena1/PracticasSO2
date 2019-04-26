#include <iostream>
#include <mutex>
#include <thread>
#include <chrono>


class SemaforoContador{
private:
	int valor;
	std::mutex mutex_;
	std::mutex mutex_variable;

	void block();
	void unblock();
public:
	SemaforoContador(int valor);
	void wait();
	void signal();
};

SemaforoContador::SemaforoContador(int v): valor(v){};

void SemaforoContador::block() {mutex_variable.lock();}
void SemaforoContador::unblock() {mutex_variable.unlock();}

void SemaforoContador::wait() {

	mutex_.lock();
	if(--valor<=0){
		mutex_.unlock();
		block();
		mutex_.lock();
	}
	mutex_.unlock();
}

void SemaforoContador::signal() {
	
	mutex_.lock();
	if(++valor<=0){
		unblock();
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}
	mutex_.unlock();
}