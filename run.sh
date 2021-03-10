#!/bin/bash

#Compilation
make
#Suppression des fichiers temporaires
rm bin/parking.o bin/console.o bin/vehicle.o bin/random.o bin/place.o
#Execution
./bin/EXEC


