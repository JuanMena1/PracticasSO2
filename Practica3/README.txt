Para compilar y ejecutar el programa, disponemos de 2 opciones:

ESCRIBIENDO LOS COMANDOS CORRESPONDIENTES

Compilar el programa:
	-Para poder compilar el programa tenemos que ejecutar el comando:

		"g++ Cine.cpp -o Cine -pthread -std=c++11"

	Con ello se le indica la utilización de hilos con "-pthread" y la versión de C++ que usamos
	"std=c++11" que en este caso es la versión 11.

Ejecutar el programa:
	-Una vez compilado el programa con el comando antes mencionado, la ejecución se realiza con el siguiente comando:

		"./Cine"

USANDO MAKEFILE

Compilar el programa:
	-Para compilar usando el Makefile basta con ejecutar el comando:
		"make compile"

Ejecutar el programa:
	-Para ejecutar usando el Makefile debemos usar el comando:
		"make run"

Al ejecutar el programa en cualquiera de los 2 casos, se crearán 10 clientes en un principio y después se
simulará la llegada de más.