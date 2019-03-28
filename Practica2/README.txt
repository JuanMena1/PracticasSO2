Para compilar y ejecutar el programa, disponemos de 2 opciones:
ESCRIBIENDO LOS COMANDOS CORRESPONDIENTES

Compilar el programa:
	-Para poder compilar el programa tenemos que ejecutar el comando:

		"g++ SSOOIIGLE.cpp -o SSOOIIGLE -pthread -std=c++11"

	Con ello se le indica la utilización de hilos con "-pthread" y la versión de C++ que usamos
	"std=c++11" que en este caso es la versión 11.

Ejecutar el programa:
	-Una vez compilado el programa con el comando antes mencionado, la ejecución se realiza con el siguiente comando:

		"./SSOOIIGLE <nombre_fichero> <palabra> <num_hilos>"

	En este caso <nombre_fichero> es el nombre del fichero en el que queremos buscar la palabra
	<palabra> es la palabra que queremos buscar dentro del fichero y <num_hilos> son los hilos
	que queremos que el programa cree para ello.

USANDO MAKEFILE

Compilar el programa:
	-Para compilar usando el Makefile basta con ejecutar el comando:
		"make compile"

Ejecutar el programa:
	-Para ejecutar usando el Makefile antes debemos abrir dicho Makefile para indicarle 
	el fichero donde queremos buscar, la palabra que queremos buscar y el número de hilos que queremos usar.
	Una vez hecho esto debemos usar el comando:
		"make run"

	Por defecto si queremos realizar una prueba de la ejecución del programa en un fichero de prueba, el comando que debemos utilizar es:
		"make test"

	Este comando hará una búsqueda de la palabra hola en el archivo "prueba.txt" usando
	para ello 3 hilos.