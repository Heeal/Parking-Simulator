#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "console.h"
#include "config.h"

FILE* openFile(char* path){ 
	FILE* f = NULL;
	f = fopen(path, "r");
	return f;
}

void showParking(int x, int y, char array[x][y]){ //Affichage du parking
	int i;
	int j;
	setConsoleCursor(0,1);
	for(j=0;j<y;j++){
		printf(" ");	
		for(i=0;i<x;i++){
			printUnicode(array[i][j]); //On affiche les caractères Unicode de la matrice de parking
		}
		printf("\n");	
	}
}

void showParkingRaw(int x, int y, char array[x][y]){ //[DEBUG]: Affiche la matrice de parking en ASCII
	int i;
	int j;
	for(j=0;j<y;j++){
		for(i=0;i<x;i++){
			printf("%c", array[i][j]);
		}
		printf("\n");	
	}
}
void initParking(int x, int y, char array[x][y]){ //Initialisation de la matrice de parking
	int i,j;

	for(i=0;i<x;i++){
		for(j=0;j<y;j++){
			array[j][i] = ' ';
		}
	}	
}


void initVehicule(int x, int y, char array[x][y]){ //Initialisation de la matrice de vehicules
	int i,j;
	for(i=0;i<y;i++){
		for(j=0;j<x;j++){
			array[j][i] = '0';
		}
	}
}
void barrierOpen(int x, int y, int x_size, int y_size, char parkingArray[x_size][y_size]){ //Ouverture d'une barrière de position (x,y)
	int i;
	for(i=-3;i<4;i++){
		parkingArray[x+i][y] = ' ';
	}
}
void barrierClose(int x, int y, int x_size, int y_size, char parkingArray[x_size][y_size]){ //Fermeture d'une barrière de position (x,y)
	int i;
	for(i=-3;i<4;i++){
		parkingArray[x+i][y] = '-';
	}
}

//Algorithme des barrières
void barrierLogic(int x, int y, int x_size, int y_size, char vehiculeArray[x_size][y_size],char parkingArray[x_size][y_size], unsigned long long int step, unsigned long long int *tmp_step){
	if( (y==31 && x==23)||(y==17 && x==23) ){ //Pour les barrières en bas à gauche et en haut à gauche
		//On verifie s'il n'y a pas de vehicules en haut de la barrière :
		if(vehiculeArray[x+1][y] == '0' && vehiculeArray[x][y] == '0' && vehiculeArray[x-1][y] == '0' && vehiculeArray[x+1][y-1] == '0' && vehiculeArray[x][y-1] == '0' && vehiculeArray[x-1][y-1] == '0' ){
			//S'il n'y a personne devant : On ferme la barrière et on réinitialise son temps d'ouverture tmp_step
			*tmp_step = 0;
			barrierClose(x,y,PARKING_X_SIZE, PARKING_Y_SIZE, parkingArray);
		}
		if(vehiculeArray[x+1][y-1] != '0' || vehiculeArray[x][y-1] != '0' || vehiculeArray[x-1][y-1] != '0'){
			//s'il y a un vehicule devant la barrière
			if((*tmp_step != 0 && step >= *tmp_step + FENCE_OPEN_TIME)){ //Si la barrière n'a pas commencé l'ouverture : on attend que le temps d'ouverture se finisse
				barrierOpen(x,y,PARKING_X_SIZE, PARKING_Y_SIZE, parkingArray); //On ouvre la barrière
			}else if(*tmp_step == 0){ //Si la barrière n'a pas commencé à s'ouvrir
				*tmp_step = step; //On met à jour le debut de l'ouverture au step actuel
			}
		}
	}else{
		//Pour les barrières en haut à droite et en bas à droite : Même algorithme mais les vehicules arrivent par le bas
		if(vehiculeArray[x+1][y] == '0' && vehiculeArray[x][y] == '0' && vehiculeArray[x-1][y] == '0' && vehiculeArray[x+1][y+1] == '0' && vehiculeArray[x][y+1] == '0' && vehiculeArray[x-1][y+1] == '0' ){
			*tmp_step = 0;
			barrierClose(x,y,PARKING_X_SIZE, PARKING_Y_SIZE, parkingArray);
		}
		if(vehiculeArray[x+1][y+1] != '0' || vehiculeArray[x][y+1] != '0' || vehiculeArray[x-1][y+1] != '0'){
			if((*tmp_step != 0 && step >= *tmp_step + FENCE_OPEN_TIME)){
				barrierOpen(x,y,PARKING_X_SIZE, PARKING_Y_SIZE, parkingArray);
			}else if(*tmp_step == 0){
				*tmp_step = step;
			}
		} 
	}
}

void loadParking(int x, int y, char array[x][y], char array2[x][y]){ //Ouverture du fichier parking.txt et on l'enregistre dans la matrice de parking
	FILE* f = openFile(P_PATH);
	char c = fgetc(f);
	int i = 0;
	int j = 0;
	while(c != EOF){
		if(c != '\n'){
			array[i][j] = c;
			i++;
		}else{
			j++;
			i = 0;
		}
		c = fgetc(f);
	}
	fclose(f);
}

