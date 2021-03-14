all: random.c console.c parking.c vehicle.c place.c player_controls.c
	gcc -Wall src/main.c -o bin/EXEC bin/parking.o bin/console.o bin/vehicle.o bin/random.o bin/place.o bin/player_controls.o
parking.c: 
	gcc -Wall -c src/parking.c -o bin/parking.o
console.c:
	gcc -Wall -c src/console.c -o bin/console.o
vehicle.c:
	gcc -Wall -c src/vehicle.c -o bin/vehicle.o
random.c:
	gcc -Wall -c src/random.c -o bin/random.o
place.c:
	gcc -Wall -c src/place.c -o bin/place.o
player_controls.c:
	gcc -Wall -c src/player_controls.c -o bin/player_controls.o
