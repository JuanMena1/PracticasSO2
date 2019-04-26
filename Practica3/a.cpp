#include <iostream>
#include <queue>
#include <thread>
#include <future>

std::queue <int> cola;

void HiloA(std::future<int>& futA){
	int x = 0;
	
	x = futA.get();
	std::cout << "El valor de x es " << x << std::endl;
}

int main(int argc, char const *argv[])
{
	std::promise <int> promA;
	std::future <int> futA = promA.get_future();

	std::thread A(HiloA, std::ref(futA));
	promA.set_value(7);

	A.join();
	return 0;
}