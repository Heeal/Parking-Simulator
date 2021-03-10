#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "struct.h"
#include "vehicle.h"

// génère un int de manière aléatoire dans un interval
int randInt(int min, int max){
    return rand()%(max-min +1) + min;
}

// génère aléatoiremet un type de vehicule v c ou m
char randType(){
    int rand = randInt(0,100);
    if(rand < SPAWRATE_CAR){
        return 'v';
    }
    else if(rand < SPAWRATE_CAR+SPAWRATE_MOTO){
        return 'm';
    }else{
        return 'c';
    }
}

// génère une direction aléatoire lors du spawn de vehicule
char randDirection(){
    switch(randInt(0,1)){
        case 0:
            return 'E';
            break;
        case 1:
            return 'O';
            break;
    }
    return 'E';

}
// génère aléatoirement un vehicule v c ou m arrivant de l'ouest ou de l'est de la carte
void newRandVehicle(list* l, int x, int y, char vehiculeBoard[x][y],int step, list_p* l_c, list_p* l_t, list_p* l_m){
    char dir = randDirection();
    char rand = randType();
    int color = randInt(0,7);
    printf("%d", color);
    if(dir == 'O'){
        if(rand == 'c'){
            newVehicule(l,rand,141,20,color,dir,PARKING_X_SIZE,PARKING_Y_SIZE,vehiculeBoard,step,l_t);
        }else if(rand == 'm'){
            newVehicule(l,rand,141,20,color,dir,PARKING_X_SIZE,PARKING_Y_SIZE,vehiculeBoard,step,l_m);
        }else{
            newVehicule(l,rand,140,19,color,dir,PARKING_X_SIZE,PARKING_Y_SIZE,vehiculeBoard,step,l_c);
        }
    }else{
        if(rand == 'v'){
            newVehicule(l,rand,9,24,color,dir,PARKING_X_SIZE,PARKING_Y_SIZE,vehiculeBoard,step,l_c);
        }else if(rand == 'c'){
            newVehicule(l,rand,9,24,color,dir,PARKING_X_SIZE,PARKING_Y_SIZE,vehiculeBoard,step,l_t);
        }else{
            newVehicule(l,rand,9,23,color,dir,PARKING_X_SIZE,PARKING_Y_SIZE,vehiculeBoard,step,l_m);
        }
    }
}
int genrandTemp(char type){ //Génère une durée aléatoire de temps restés à la place du parking
    switch(type){
        case 'v': //On génère un entier entre MIN/MAX du fichier config.h selon le type de vehicules
            return randInt(MIN_DURATION_PARK_CAR,MAX_DURATION_PARK_CAR);
            break;
        case 'c':
            return randInt(MIN_DURATION_PARK_TRUCK,MAX_DURATION_PARK_TRUCK);   
            break;
        case 'm':
            return randInt(MIN_DURATION_PARK_MOTO,MAX_DURATION_PARK_MOTO);   
            break;
    }
    return randInt(MIN_DURATION_PARK_CAR,MAX_DURATION_PARK_CAR);
}
