#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "struct.h"
#include "console.h"
#include "parking.h"
#include "vehicle.h"
#include "random.h"
#include "place.h"

void refreshScreen(char parkingBoard[PARKING_X_SIZE][PARKING_Y_SIZE], char vehiculeBoard[PARKING_X_SIZE][PARKING_Y_SIZE], list* l, float *last_pay,unsigned int *last_pay_dir){
		clearConsole(); //Clear le terminal
		showParking(PARKING_X_SIZE, PARKING_Y_SIZE, parkingBoard); //On affiche le parking
		if(l->premier != NULL){ //S'il y a au moins un vehicule
			updateArray(l->premier, PARKING_X_SIZE, PARKING_Y_SIZE, vehiculeBoard); //On met à jour la matrice de vehicules
			printVehicules(l->premier); //On affiche les vehicules
		}
		colorPlace(PARKING_X_SIZE, PARKING_Y_SIZE, parkingBoard, vehiculeBoard); //On met à jour les LED des places

		if(*last_pay != 0){ //Affichage des payements
			if(*last_pay_dir == 0){ //Changement de curseur selon la position du payement
				setConsoleCursor(17,8);
			}else{
				setConsoleCursor(29,104);
			}
			printf("%s+%.2f $%s",getColor(2),*last_pay,getColor(1)); //On affiche le payement en vert
		}
}
// initialise la liste
list* init_list(list* list){
	list = malloc(sizeof(list));
	list->premier=NULL;
	return list;
}

// aloue dynamiquement de la mémoire pour la liste de vehicule
Vehicule* init_v(Vehicule* v){
	v = malloc(sizeof(Vehicule));
	return v;
}

// retourne le prix à payé pour un vehicule donné
float getPrice(Vehicule* v){
	if(v->stoptime == 0){
		return 0;
	}
	switch(v->type){
		case 'v':
			return (PRICE_PER_STEP_CAR*(v->stoptime-v->starttime));// multiplie une constante définissant le prix par step par le temps resté sur le parking
		case 'm':
			return (PRICE_PER_STEP_MOTO*(v->stoptime-v->starttime));
		case 'c':
			return (PRICE_PER_STEP_TRUCK*(v->stoptime-v->starttime));
	}	
	return (PRICE_PER_STEP_CAR*(v->stoptime-v->starttime));
}
// ajoute le vehicule actuel à la fin de la liste des vehicules
void addEndList(Vehicule* actuel, Vehicule* v){	
	if(actuel->next == NULL){
		actuel->next = v;
	}
	else{
		addEndList(actuel->next, v);
	}
}
// montre la matrice des vehicules 
void showVehiculeArray(int x, int y, char array[x][y]){
	int i,j;
	for(i=0;i<y;i++){
		printf(" ");
		for(j=0;j<x;j++){
			printf("%c", array[j][i]);
		}
	printf("\n");
	}
}

void updateVehiculeArray(Vehicule* v, int x_size, int y_size, char vehiculeBoard[x_size][y_size]){ //Mise à jour de la matrice de vehicule
	int i;
	int j;
	for(j=0;j < v->long_y;j++){ 
		for(i=0;i < v->long_x;i++){ 
			if(v->direction == 'O'){
				j *= -1;
			}else if(v->direction == 'S'){
				i *= -1;
				j *= -1;
			}else if(v->direction == 'E'){
				i *= -1;
			}
			vehiculeBoard[v->x + i][v->y + j] = v->type;
			if(v->direction == 'O'){
				j *= -1;
			}else if(v->direction == 'S'){
				i *= -1;
				j *= -1;
			}else if(v->direction == 'E'){
				i *= -1;
			}	
		}
	}

	if(v->next != NULL){
		updateVehiculeArray(v->next, x_size, y_size, vehiculeBoard);
	}
}



void updateLong(Vehicule* v){ //Update des longueurs des vehicules (Les vehicules n'ont pas les mêmes dimensions lorsqu'ils tournent)
	switch(v->type){
		case 'c':
			if(v->direction == 'N' || v->direction == 'S'){
				v->long_x = 3;
				v->long_y = 6;
			}else{
				v->long_x = 9;
				v->long_y = 2;
			}
			break;
		case 'v':
			if(v->direction == 'N' || v->direction == 'S'){
				v->long_x = 3;
				v->long_y = 3;
			}else{
				v->long_x = 4;
				v->long_y = 1;
			}
			break;
		case 'm':
			if(v->direction == 'N' || v->direction == 'S'){
				v->long_x = 1;
				v->long_y = 3;
			}else{
				v->long_x = 4;
				v->long_y = 1;
			}
			break;
	}
}

void updateArray(Vehicule *premier, int x_size, int y_size, char vehiculeBoard[x_size][y_size]){
	initVehicule(x_size, y_size, vehiculeBoard);

	updateVehiculeArray(premier, x_size, y_size, vehiculeBoard);

}


int isPlaceAvailable(Vehicule *v, int x, int y){
	int f = 0;
	if(v->randposx == x && v->randposy == y){
		return 1;
	}
	if(v->next != NULL){
		f = isPlaceAvailable(v->next, x, y);	
	}
	return f;
}

Vehicule* newVehicule(list* l, char type, int x, int y, int color, char direction, int x_size, int y_size, char vehiculeBoard[x_size][y_size], int step, list_p* l_p){ //Add a vehicule
	
	Vehicule* v = NULL;
	v = init_v(v);
	if(vehiculeBoard[x][y] != 0 && vehiculeBoard[x][y+1]&& vehiculeBoard[x][y-1]){
		Place* p = NULL;
		v = init_v(v);
		p = init_place(p);
		v->type = type; // type du vehicules ( c: camion, v: voiture, m : moto)
		v->x = x; // position en x
		v->y = y; // position en y 
		v->state = 1; // état du vehicule ( 0: à l'arrêt, 1 : vehicule arrive dans le parking, 2: vehicule sortant du parking, -1: Vehicule crashé pas encore traité par la procédure de crash, -2: Vehicule crashé et déjà traité)
		v->starttime=step; // permet de savoir combien de temps il reste dans le vehicule
		v->crashtime = 0; // gestion du temps de crash
		vehiculeBoard[x][y] = v->type; // permet de mettre le type de vehicule dans la matrice des vehicules v, c ou m
		v->direction = direction; // mets à jour la direction du vehicule quand on décide de la modifier
		v->next = NULL; // gestion de liste
		v->color = color; // couleur du vehicule
		v->stoptime = 0; // permet la gestion du temps d'arrête sur une place
		v->back = 0; // à 0 : le vehicule ne recule pas, à 1 il recule
		v->truck = 0; // permet d'enregistrer un état (1 ou 0) pour rentrer dans certaine boucle ou pas (gestion déplacement vehicules)
		v->park = 0; // 0 vehicule non garé, 1 il est garé
		p = findPlace(v,l_p); // cherche une place disponible	
		v->randposx = p->x; // enregistre coordonnées de la place disponible dans les caractéristiques du véhicule pour qu'il s'y rende (voir carlogic)
		v->randposy = p->y;

		v->collision = 0; // si il y'a un blocage entre deux véhicules, fait avancer le vehicule en état 1 
		v->xcollision = 0; // retiens la position du vehicule à l'arrêt, si il dépasse le temps défini à cette même place on considère que deux vehicules sont bloqués donc on les fait avancer
		v->ycollision = 0;
		v->var = 0; // empeche de retourner dans la boucle pour sauvegarder le temps d'arrêt (voir backward)
		
		if(p->x != -1){
			deletePlace(l_p, p);
		} 


		switch(type){
			case 'c':
				v->speed = 1;
				if(v->direction == 'N' || v->direction == 'S'){
					v->long_x = 3;
					v->long_y = 6;
				}else{
					v->long_x = 9;
					v->long_y = 2;
				}
				break;
			case 'v':
				v->speed = 1;
				if(v->direction == 'N' || v->direction == 'S'){
					v->long_x = 3;
					v->long_y = 3;
				}else{
					v->long_x = 4;
					v->long_y = 1;
				}
				break;
			case 'm':
				v->speed = 2;
				if(v->direction == 'N' || v->direction == 'S'){
					v->long_x = 1;
					v->long_y = 3;
				}else{
					v->long_x = 4;
					v->long_y = 1;
				}
				break;
		}

		if(l->premier == NULL){
			l->premier = v;
		}else{
			addEndList(l->premier, v);
		}
	}
	return v;
}

void showList(Vehicule* actuel){ //DEBUG: Liste les véhicules
	if (actuel == NULL){
		printf("Fin de la liste\n");
	}else{
		printf("------------\n");
		printf("Type: %c \n", actuel->type);
		printf("Position: [%d;%d] \n", actuel->x, actuel->y);
		printf("Status: %d \n", actuel->state);
		printf("Crashedtime: %lld \n", actuel->crashtime);
		printf("------------\n");
		if(actuel->next != NULL){
			showList(actuel->next);
		}
	}
}

char *pathConstruct(Vehicule* v){ //Construis le chemin d'accès au fichier source des vehicuels
	char *path = (char *) malloc(31*sizeof(char));
	strcpy(path, "");	
	strcat(path, "files/vehicles/");
	switch(v->type){ //Concatenation de chaine de caractères selon le type et la direction des vehicules
		case 'c':
			strcat(path, "truck/");
			break;
		case 'm':
			strcat(path, "moto/");
			break;
		case 'v':
			strcat(path, "car/");
			break;
	}
	switch(v->direction){
		case 'N':
			strcat(path, "up.txt");
			break;
		case 'E':
			strcat(path, "right.txt");
			break;
		case 'S':
			strcat(path, "down.txt");
			break;
		case 'O':
			strcat(path, "left.txt");
			break;
	}
	return path;

}
// Retire un vehicule de la liste
int deleteFromList(Vehicule* current, Vehicule *v){
	if(current->next == v){
		current->next = v->next;
		free(v);
		return 1;
	}
	deleteFromList(current->next, v);
	return 0;
}
// Retire le premier vehicule de la liste
void deleteFirstFromList(list *l, Vehicule *v){
	if(v->next != NULL){
		l->premier = v->next;
	}else{
		l->premier = NULL;
	}
	free(v);
}
// Retire le dernier vehicule de la liste
int deleteLastFromList(Vehicule* current, Vehicule *v){
	if(current->next == v){
		current->next = NULL;
		free(v);
		return 1;
	}
	deleteLastFromList(current->next, v);
	return 0;
}
// Retire un vehicule de la liste	
void deleteVehicule(list *l, Vehicule *v){
	
	if(l->premier == v){
		deleteFirstFromList(l, v);
	}else if(v->next == NULL){
		deleteLastFromList(l->premier, v);
	}else{
		deleteFromList(l->premier, v);
	}
}


int findVehiculeCrashed(int x, int y,Vehicule* v, Vehicule* current, unsigned long long int step){ //Trouve le vehicule crashé dans la liste
	int i,j = 0;
	int flag = 0;
	if(v != current){
		for(i=0;i<current->long_x;i++){ //On parcourt toutes les cases de chaque vehicule
			for(j=0;j<current->long_y;j++){

				if(v->direction == 'O'){ //Changement de signe selon l'orientation du vehicule
					j *= -1; 
				}else if(v->direction == 'S'){
					i *= -1;
					j *= -1;
				}else if(v->direction == 'E'){
					i *= -1;
				}

				if(current->x+i==x && current->y+j == y){ //Si un vehicule est dans la zone du crash : on le detruit
					current->crashtime = step;	
					current->state = -1;				
					return 1;
				}

				if(v->direction == 'O'){ //Reinitialisation du signe i/j pour incrémentation
					j *= -1;
				}else if(v->direction == 'S'){
					i *= -1;
					j *= -1;
				}else if(v->direction == 'E'){
					i *= -1;
				}

			}
		}
	}
	if(current->next != NULL){
		flag = findVehiculeCrashed(x,y,v,current->next, step);
	}
	return flag;
}

void vehiculeCrash(Vehicule* v, list* l, int  x, int y, unsigned long long int step){ //Procédure lors d'un crash : on trouve le vehicule collisioné et on détruit les deux vehicules
	findVehiculeCrashed(x,y,v,l->premier,step);
	v->state = -1;
	v->crashtime = step;
}

int deleteCrashedVehicules(list* l, Vehicule* v, list_p* l_car, list_p* l_truck, list_p* l_moto, unsigned long long int step){ //Destruction des vehicules
	int n = 0;
	if(v == NULL){
		return 0;
	}
	if(v->crashtime != 0 && step == v->crashtime+CRASH_TIME){ //On attend la durée d'un crash
		deleteVehicule(l,v);
	}
	n += deleteCrashedVehicules(l, v->next, l_car,l_truck,l_moto,step); //On compte le nombre de véhicule crashés
	if(v->state == -1 || v->state == -2){ //Pour chaque vehicule du parking -> s'il est crashé
		if(v->state == -1){ //Si on ne l'a pas compté 
			if(v->type=='c' && v->randposx != -1){
				if(l_truck->premier != NULL){ //On ajoute dans la liste de place disponible sa place occupée
					addEndPlaceList(l_truck->premier, newPlace(v->randposx,v->randposy));
				}else{
					l_truck->premier = newPlace(v->randposx,v->randposy);
				}
			}else if(v->type == 'v' && v->randposx != -1){
				if(l_car->premier != NULL){
					addEndPlaceList(l_car->premier, newPlace(v->randposx,v->randposy));
				}else{
					l_car->premier = newPlace(v->randposx,v->randposy);
				}
			}else if(v->randposx != -1){ 
				if(l_moto->premier != NULL){
					addEndPlaceList(l_moto->premier, newPlace(v->randposx,v->randposy));
				}else{
					l_moto->premier = newPlace(v->randposx,v->randposy);
				}
			}
			v->color = 0; //On le met en rouge
			v->state = -2; //On compte le vehicule crashé
			return 1;
		}
		return 0;
	}
	return n;
}

// Fonction qui permet de faire avancer un vehicule tout en empechant les collisions entre les vehicules
void forward(Vehicule* v, int x_size, int y_size, char parkingBoard[x_size][y_size], char vehiculeBoard[x_size][y_size], list* l, float *last_pay, unsigned int *last_pay_dir, unsigned long long int step){ //Move vehicule forward
	int i = 0;

	if(v->direction == 'N'){// si le vehicule se deplace vers le nord
		//Pas de colision en ligne
		if((vehiculeBoard[v->x][v->y-2] != '0') || (vehiculeBoard[v->x-1][v->y-2] != '0') || (vehiculeBoard[v->x+1][v->y-2] != '0')) {
			if(v->state >= 0){
				v->state = 0;// si le vehicule va entrer en collision on l'arrête sinon on le fait avancer dans le else
			}
		}else{
			if(v->park == 0 && v->state != -1 && v->state != -2){
				if((v->x != 117 || v->y != 28)&&(v->x != 117 || v->y != 17)&&(v->x != 118 || v->y != 17)){ // pour que le vehicule ne continue pas d'avancer face à une barrière
					if(v->state >= 0){
						v->state = 1;// sinon on le fait avancer
					}
				}
			}
		}
	}

	if(v->direction == 'S'){ // vers le sud
		//Pas de colision en ligne
		if((vehiculeBoard[v->x][v->y+2] != '0') || (vehiculeBoard[v->x-1][v->y+2] != '0') || (vehiculeBoard[v->x+1][v->y+2] != '0')){
			if(v->state >= 0){
				v->state = 0;
			}
		}else{
			if(v->park == 0 && v->state != -1 && v->state != -2){
				if((v->x != 24 || v->y != 30)&&(v->x != 24 || v->y != 16)&&(v->x != 22 || v->y != 16)){  // pour que le vehicule ne continue pas d'avancer face à une barrière
					if(v->state >= 0){
						if(v->state >= 0){
							v->state = 1;
						}
					}
				}
			}
		}
	}

	if(v->direction == 'O'){  // vers l'ouest
		//Pas de colision en ligne
		if((vehiculeBoard[v->x-3][v->y] != '0') || (vehiculeBoard[v->x-3][v->y-1] != '0') || (vehiculeBoard[v->x-3][v->y+1] != '0')){
			if(v->state >= 0){
				v->state = 0;
			}
		}else{
			if(v->park == 0 && v->state != -1 && v->state != -2){
				if(v->state >= 0){
					v->state = 1;
				}
			}
		}
	}
	if(v->direction == 'E'){// vers l'est
		//Pas de colision en ligne
		if((vehiculeBoard[v->x+3][v->y] != '0') || (vehiculeBoard[v->x+3][v->y-1] != '0') || (vehiculeBoard[v->x+3][v->y+1] != '0')){
			if((v->x == 17||v->x == 18||v->x == 19||v->x == 20 ||v->x == 21 || v->x == 22 || v->x == 23) && v->y == 12 && v->state == 0){// gestion blocage voiture en bas à gauche du parking voiture, permet de la faire reculer quand deux voitures veulent sortir en même temps
				if(v->type == 'v'){
					v->back = 1;
					v->truck = 1;
				}
			}else{
				if(v->state >= 0){
					v->state = 0;
				}
			}	
		}else{
			if(v->park == 0 && v->state != -1 && v->state != -2){
				if(v->state >= 0){
					v->state = 1;
				}
			}
		}
	}
		
	if(v->state > 0){
		for(i=0;i<v->speed;i++){
			switch(v->direction){
				case 'N': 
					if(v->y-1 >= 0){
						//On crash le vehicule s'il y a un mur ou une voiture devant
						if(vehiculeBoard[v->x][v->y -1] != '0' || parkingBoard[v->x-1][v->y-1] == '-' || parkingBoard[v->x-1][v->y-1] == 't' || parkingBoard[v->x-1][v->y-1] == 'l' || parkingBoard[v->x-1][v->y-1] == 'p' || parkingBoard[v->x-1][v->y-1] == 'j' || parkingBoard[v->x-1][v->y-1] == ':'|| parkingBoard[v->x-1][v->y-1] == '6'|| parkingBoard[v->x-1][v->y-1] == '('){
							vehiculeCrash(v,l,v->x,v->y-1,step);
							v->y -= 1;
						}else if(v->type != 'm' && vehiculeBoard[v->x +1][v->y -1] != '0'){ //On vérifie aussi en diagonale dans le cas ou ce n'est pas une moto (car la moto n'a qu'un caractère le de largeur)
							vehiculeCrash(v,l,v->x+1,v->y-1, step);
						}else{ //Si pas de problème : le vehicule peut avancer peut avancer
							v->y -= 1;
						}
					}else{
						//Si le parking sort du parking, on le détruit directement
						deleteVehicule(l,v);
					}
					break;
				case 'E':  //Même principe sur les autres directions
					if(v->x+1 < 142){
						if(vehiculeBoard[v->x+1][v->y] != '0' || parkingBoard[v->x][v->y] == ':' || parkingBoard[v->x][v->y] == 'j' || parkingBoard[v->x][v->y] == 'p' || parkingBoard[v->x][v->y] == 't' || parkingBoard[v->x][v->y] == 'l'|| parkingBoard[v->x][v->y] == '-'|| parkingBoard[v->x][v->y] == '6'|| parkingBoard[v->x][v->y] == '('){
							vehiculeCrash(v,l,v->x+1,v->y,step);
						}else if (v->type != 'm' && vehiculeBoard[v->x +1][v->y +1] != '0'){
							vehiculeCrash(v,l,v->x+1,v->y+1,step);
						}else{
							v->x += 1;
						}
					}else{
						deleteVehicule(l,v);
					}
					break;
				case 'S': 
					if(v->y+1 < 135){
						if(vehiculeBoard[v->x][v->y+1] != '0'|| parkingBoard[v->x-1][v->y] == '-'|| parkingBoard[v->x-1][v->y] == 'j'|| parkingBoard[v->x-1][v->y] == 'p'|| parkingBoard[v->x-1][v->y] == 't'|| parkingBoard[v->x][v->y] == 'l'|| parkingBoard[v->x][v->y] == ':' || parkingBoard[v->x][v->y] == '6' || parkingBoard[v->x][v->y] == '('){
							vehiculeCrash(v,l,v->x,v->y+1,step);
						}else if (v->type != 'm' && vehiculeBoard[v->x - 1][v->y +1] != '0'){
							vehiculeCrash(v,l,v->x-1,v->y+1,step);
						}else{
							v->y += 1;
						}
					}else{
						deleteVehicule(l,v);
					}
					break;
				case 'O': 
					if(v->x-3 > 0 || v->randposy == 44 || v->randposy == 39 || v->randposy == 34){
						if(vehiculeBoard[v->x-1][v->y] != '0'|| parkingBoard[v->x-1][v->y] == ':'|| parkingBoard[v->x][v->y] == 'j'|| parkingBoard[v->x][v->y] == 'p'|| parkingBoard[v->x][v->y] == 't'|| parkingBoard[v->x][v->y] == 'l'|| parkingBoard[v->x][v->y] == ':'|| parkingBoard[v->x][v->y] == '6'|| parkingBoard[v->x][v->y] == '('){
							vehiculeCrash(v,l,v->x-1,v->y,step);
						}else if(v->type != 'm' && vehiculeBoard[v->x - 1][v->y -1] != '0'){
							vehiculeCrash(v,l,v->x-1,v->y-1,step);
						}else{
							v->x -= 1;
						}
					}else{
						if(v->y > 15 || v->y < 25){
							deleteVehicule(l,v);
						}
					}
					break;
			}
		}
		if(l->premier != NULL){
			updateArray(l->premier, PARKING_X_SIZE, PARKING_Y_SIZE, vehiculeBoard);	
		}else{
			initVehicule(PARKING_X_SIZE, PARKING_Y_SIZE, vehiculeBoard);
		}
	}
}
// fait reculer un vehicule et empeche la collision avec un vehicule 
void backward(Vehicule* v, int x_size, int y_size, char parkingBoard[x_size][y_size], char vehiculeBoard[x_size][y_size], list* l, unsigned long long int step){ //Move vehicule forward
	int i = 0;

	if(v->type == 'c'){// si c'est un camion
		if(v->direction == 'E'){// recule vers l'est
			//Pas de colision en ligne
			if((vehiculeBoard[v->x-9][v->y] != '0') || (vehiculeBoard[v->x-9][v->y+1] != '0')|| (vehiculeBoard[v->x-9][v->y-1] != '0')){
			// condition qui permet d'empecher tous les blocages possibles quand un vehicule est bloqué avec un autre en backward
				if(v->var != 1){// permet de ne pas rentrer dans la boucle pour sauvegarder le temps d'arrêt
					v->timecollision = step + 20; // on sauvegarde quand le vehicule s'arrête plus un temps, dans carlogic si ce temps est dépassé et que le véhicule est toujours à la même position, on le fait reculer
					v->xcollision = v->x; // on sauvegarde sa position
					v->ycollision = v->y;
					if(v->state >= 0){
						v->state = 0;
					}
					v->var = 1;
				}
			}else{ // si pas de collision on le fait reculer
				if(v->park == 0 && v->state != -1 && v->state != -2){
					if(v->state >= 0){
						v->state = 1;
					}
				}
			}
		}
		if(v->direction == 'O'){ //  recule vers l'ouest
			//Pas de colision en ligne
			if((vehiculeBoard[v->x+9][v->y] != '0') || (vehiculeBoard[v->x+9][v->y+1] != '0')|| (vehiculeBoard[v->x+9][v->y-1] != '0')){
				if(v->var != 1){
					v->timecollision = step + 20;
					v->xcollision = v->x;
					v->ycollision = v->y;
					if(v->state >= 0){
						v->state = 0;
					}
					v->var = 1;
				}
			}else{
				if(v->park == 0 && v->state != -1 && v->state != -2){
					if(v->state >= 0){
						v->state = 1;
					}
				}
			}
		}
	}

	if(v->type == 'v'){ // si c'est une voiture
		if(v->direction == 'E'){ // recule vers l'est
			//Pas de colision en ligne
			if((vehiculeBoard[v->x-4][v->y] != '0') || (vehiculeBoard[v->x-4][v->y+1] != '0')|| (vehiculeBoard[v->x-4][v->y-1] != '0')){
				if(v->var != 1){
					v->timecollision = step + 20;
					v->xcollision = v->x;
					v->ycollision = v->y;
					if(v->state >= 0){
						v->state = 0;
					}
					v->var = 1;
				}
			}else{
				if(v->park == 0 && v->state != -1 && v->state != -2){
					if(v->state >= 0){
						v->state = 1;
					}
				}
			}
		}
		if(v->direction == 'O'){ // recule vers l'ouest
			//Pas de colision en ligne
			if((vehiculeBoard[v->x+4][v->y] != '0') || (vehiculeBoard[v->x+4][v->y+1] != '0')|| (vehiculeBoard[v->x+4][v->y-1] != '0')){
				if(v->var != 1){
					v->timecollision = step + 20;
					v->xcollision = v->x;
					v->ycollision = v->y;
					if(v->state >= 0){
						v->state = 0;
					}
					v->var = 1;
				}
			}else{
				if(v->park == 0 && v->state != -1 && v->state != -2){
					if(v->state >= 0){
						v->state = 1;
					}
				}
			}

		}
	}

	if(v->type == 'm'){ // si c'est une moto
		if(v->direction == 'E'){ // recule  vers l'est
			if((vehiculeBoard[v->x-4][v->y] != '0') || (vehiculeBoard[v->x-4][v->y-1] != '0') || (vehiculeBoard[v->x-4][v->y+1] != '0')){
				if(v->var != 1){
					v->timecollision = step + 20;
					v->xcollision = v->x;
					v->ycollision = v->y;
					if(v->state >= 0){
						v->state = 0;
					}
					v->var = 1;
				}
			}else{
				if(v->park == 0 && v->state != -1 && v->state != -2){
					if(v->state >= 0){
						v->state = 1;
					}
				}
			}
		}
		if(v->direction == 'O'){ // recule vers l'ouest
			if((vehiculeBoard[v->x+4][v->y] != '0') || (vehiculeBoard[v->x+4][v->y-1] != '0') || (vehiculeBoard[v->x+4][v->y+1] != '0')){
				if(v->var != 1){
					v->timecollision = step + 20;
					v->xcollision = v->x;
					v->ycollision = v->y;
					if(v->state >= 0){
						v->state = 0;
					}
					v->var = 1;
				}
			}else{
				if(v->park == 0 && v->state != -1 && v->state != -2){
					if(v->state >= 0){
						v->state = 1;
					}
				}
			}
		}
	}

	if(v->state > 0){
	for(i=0;i<v->speed;i++){
		switch(v->direction){
			case 'N': 
				if(v->y+1 < 135){
					if(vehiculeBoard[v->x][v->y + v->long_y] != '0' || parkingBoard[v->x][v->y+ v->long_y] == '-' || parkingBoard[v->x][v->y+ v->long_y] == 't' || parkingBoard[v->x][v->y+ v->long_y] == 'l' || parkingBoard[v->x][v->y+ v->long_y] == 'p' || parkingBoard[v->x][v->y+ v->long_y] == 'j' || parkingBoard[v->x][v->y+ v->long_y] == ':'|| parkingBoard[v->x][v->y+ v->long_y] == '6'|| parkingBoard[v->x][v->y+ v->long_y] == '('){
							vehiculeCrash(v,l,v->x,v->y+ v->long_y,step);
						}else if(v->type != 'm' && vehiculeBoard[v->x +1][v->y+ v->long_y] != '0'){
							vehiculeCrash(v,l,v->x+1,v->y+ v->long_y,step);
						}else{
						v->y += 1;
					}
				}
				break;
			case 'E': 
				if(v->x-1 > 0){
					if(vehiculeBoard[v->x - v->long_x][v->y] != '0' || parkingBoard[v->x- v->long_x][v->y] == '-' || parkingBoard[v->x- v->long_x][v->y] == 't' || parkingBoard[v->x- v->long_x][v->y] == 'l' || parkingBoard[v->x- v->long_x][v->y] == 'p' || parkingBoard[v->x- v->long_x][v->y] == 'j' || parkingBoard[v->x- v->long_x][v->y] == ':'|| parkingBoard[v->x- v->long_x][v->y] == '6'|| parkingBoard[v->x- v->long_x][v->y] == '('){
							vehiculeCrash(v,l,v->x - v->long_x,v->y,step);
							exit(-1);
						}else if(v->type != 'm' && vehiculeBoard[v->x - v->long_x][v->y+1] != '0'){
							vehiculeCrash(v,l,v->x- v->long_x,v->y+1,step);
							exit(-1);
						}else{
						v->x -= 1;
					}
				}
				break;
			case 'S': 
				if(v->y-1 > 0){
					if(vehiculeBoard[v->x][v->y- v->long_y] != '0' || parkingBoard[v->x][v->y- v->long_y] == '-' || parkingBoard[v->x][v->y- v->long_y] == 't' || parkingBoard[v->x][v->y- v->long_y] == 'l' || parkingBoard[v->x][v->y- v->long_y] == 'p' || parkingBoard[v->x][v->y- v->long_y] == 'j' || parkingBoard[v->x][v->y- v->long_y] == ':'|| parkingBoard[v->x][v->y- v->long_y] == '6'|| parkingBoard[v->x][v->y- v->long_y] == '('){
							vehiculeCrash(v,l,v->x,v->y-v->long_y,step);
						}else if(v->type != 'm' && vehiculeBoard[v->x-1][v->y-v->long_y] != '0'){
							vehiculeCrash(v,l,v->x- 1,v->y-v->long_y,step);
						}else{
						v->y -= 1;
					}
				}
				break;
			case 'O': 
				if(v->x+1 < 135){
					if(vehiculeBoard[v->x - v->long_x][v->y] != '0'	 || parkingBoard[v->x + v->long_x][v->y] == '-' || parkingBoard[v->x + v->long_x][v->y] == 't' || parkingBoard[v->x + v->long_x][v->y] == 'l' || parkingBoard[v->x + v->long_x][v->y] == 'p' || parkingBoard[v->x + v->long_x][v->y] == 'j' || parkingBoard[v->x + v->long_x][v->y] == ':'|| parkingBoard[v->x + v->long_x][v->y] == '6'|| parkingBoard[v->x+ v->long_x][v->y] == '('){
							vehiculeCrash(v,l,v->x+v->long_x,v->y,step);					
						}else if(v->type != 'm' && vehiculeBoard[v->x+v->long_x][v->y-1] != '0'){
							vehiculeCrash(v,l,v->x+v->long_x,v->y-1,step);
						}else{
						v->x += 1;
					}
				}
				break;
		}
		}
	}
}
// fonction qui tourne le vehicule dans une certaine direction donnée en paramètre (nord, sud, ouest, est) selon sa direction actuelle (v->direction)
void turnVehicule(Vehicule* v, char direction,int x,int y,char vehiculeBoard[x][y]){

    if(v->type == 'v'){// turn pour les voitures
        if(direction == 'N'){
                v->x -= 2;
                v->y -= 2;
        }

        if(v->direction == 'O'){
            if(direction == 'S'){
                v->x += 1;
                v->y += 2;
            }
        }

        if(v->direction == 'S'){
            if(direction == 'E'){
                v->x += 2;
                v->y -= 1;
            }
            if(direction == 'O'){
                v->x -= 3;
			}
        }
        if(v->direction == 'N'){
            if(direction == 'E'){
                v->x += 5;
                v->y += 1;
            }
            if(direction == 'O'){
                v->x -=1;
            }
        }
    }

    if(v->type == 'c'){ // turn pour les camions

        if(v->direction == 'E'){
            if(direction == 'S'){
                v->y += 6;
            }
            if(direction == 'N'){
                v->y -= 4;
                v->x -= 2;
            }
        }

        if(v->direction == 'O'){
            if(direction == 'S'){
                v->y += 4;
                v->x += 2;
            }
        }
        if(v->direction == 'S'){
            if(direction == 'E'){
                v->y -= 3;
                v->x += 5;
            }
            if(direction == 'O'){
                //v->y -= 3;
                v->x -= 9;
            }
        }
        if(v->direction == 'N'){
            if(direction == 'E'){
                v->y += 1;
                v->x += 9;
            }
            if(direction == 'O'){
                v->y += 3;
                v->x -= 9;
            }
        }
    }

	if(v->type == 'm'){// turn pour les motos

        if(v->direction == 'E'){
            if(direction == 'N'){
                v->x -= 1;
                v->y -= 2;
            }
            if(direction == 'S'){
                v->y += 3;
            }
        }

        if(v->direction == 'N'){
            if(direction == 'E'){
                v->y += 1;
                v->x += 3;
            }
            if(direction == 'O'){
                v->x -= 2;
            }
        }

        if(v->direction == 'O'){
            if(direction == 'S'){
                v->y += 2;
            }
            if(direction == 'N'){
                v->x -=1;
                v->y -=3;
            }
        }

        if(v->direction == 'S'){
            if(direction == 'E'){
                v->x += 2;
            }
            if(direction == 'O'){
                v->y -= 3;
                v->x -= 2;
            }
        }
    }

    v->direction = direction; // passe la direction du vehicule à sa nouvelle direction
    updateLong(v);
}


//Fonction qui gère le déplacement d'un véhicule (camion,voiture,moto), il choisi une place libre et il s'y rend pour ensuite sortir du parking
void carLogic(Vehicule* v,int x, int y, char array[x][y],int step,char vehiculeArray[x][y], list* l, list_p* l_car,list_p* l_truck,list_p* l_moto,float *last_pay, unsigned int *last_pay_dir){
	
	if(v->next != NULL){// on appelle la fonction pour chaque vehicule
		carLogic(v->next,x,y,array,step,vehiculeArray,l,l_car,l_truck,l_moto,last_pay,last_pay_dir);
	}
	if(v->state >= 0){// si le vehicule n'est pas en collision 
		if(v->type == 'v'){// si c'est une voiture

			if(((v->direction == 'N' && array[v->x-1][v->y-1] == '-') || (v->direction == 'S' && array[v->x-1][v->y+1] == '-')) && v->y!=49){ //Si devant moi = barrière
				v->state = 0;
			}else if((v->x == 24 && v->y == 16) || (v->x == 117 && v->y == 17)){ 							
				v->state = 1;
			}
			if(v->back == 0){
				forward(v, x, y,array, vehiculeArray, l,last_pay,last_pay_dir, step); // fait avancer la voiture
			}
			if(v->back == 1){
				backward(v, x, y,array, vehiculeArray, l, step); // fait reculer la voiture
			}
			// dirige la voiture jusqu'à la ligne en haut du parking
			if(v->x == 119 && (v->y == 24 || v->y == 19)){
				turnVehicule(v,'N',x,y,vehiculeArray);
			}
			if(v->x == 117 && v->y == 12 && v->state != 3){
				turnVehicule(v,'E',x,y,vehiculeArray);
			}
			if(v->x == 129 && v->y == 13){
				turnVehicule(v,'N',x,y,vehiculeArray);
			}
			if(v->x == 127 && v->y == 1){
				turnVehicule(v,'O',x,y,vehiculeArray);
			}
			// fait tourner la voiture si elle se trouve au niveau de sa place sur la première rangée à droite
			if(v->randposx == 119 && v->state != 3 && v->truck == 0){
				if(v->y == v->randposy){
					turnVehicule(v,'O',x,y,vehiculeArray);
				}
			}
			// fait tourner la voiture si elle se trouve au niveau de la rangée où elle doit se garer
			if((v->randposx == 116 || v->randposx == 97)&& v->state != 3){
				if(v->x == 104 && v->y == 1){
					turnVehicule(v,'S',x,y,vehiculeArray);
				}
			}

			if((v->randposx == 94 || v->randposx == 75)&& v->state != 3){
				if(v->x == 82 && v->y == 1){
					turnVehicule(v,'S',x,y,vehiculeArray);
				}
			}

			if((v->randposx == 72 || v->randposx == 53)&& v->state != 3){
				if(v->x == 60 && v->y == 1){
					turnVehicule(v,'S',x,y,vehiculeArray);
				}
			}

			if((v->randposx == 50 || v->randposx == 31)&& v->state != 3){
				if(v->x == 38 && v->y == 1){
					turnVehicule(v,'S',x,y,vehiculeArray);
				}
			}
			// dirige la voiture vers la sortie
			if(v->y == 13 && (v->x == 105 || v->x == 83 || v->x == 61 || v->x == 39)){
				turnVehicule(v,'O',x,y,vehiculeArray);
			}

			if(v->y == 13 && v->x == 17){
				turnVehicule(v,'E',x,y,vehiculeArray);
			}

			if(v->y == 12 && v->x == 24 && v->back == 0){
				turnVehicule(v,'S',x,y,vehiculeArray);
				v->y +=2;
			}

			if(v->y == 13 && v->x == 23){
				turnVehicule(v,'S',x,y,vehiculeArray);
			}
			// remet le vehicule sur la route
			if(v->y == 19 && v->x == 24){
				turnVehicule(v,'O',x,y,vehiculeArray);
				if(l_car->premier != NULL){
					addEndPlaceList(l_car->premier, newPlace(v->randposx,v->randposy));
				}else{
					l_car->premier = newPlace(v->randposx,v->randposy);
				}
				*last_pay_dir = 0;
				*last_pay = getPrice(v);
			}
			// si le vehicule est en face de la place où il doit se garer, il tourne pour se garer
			if((v->x + 11 == v->randposx)&&(v->y -1 == v->randposy)){
				turnVehicule(v,'E',x,y,vehiculeArray);
			}

			if((v->x - 8 == v->randposx)&&(v->y == v->randposy)){
				turnVehicule(v,'O',x,y,vehiculeArray);
			}

	 		// arrête le vehicule quand il arrive sur la place
	 		if(v->randposx == 97 || v->randposx == 75 || v->randposx == 53 || v->randposx == 31){
				if((v->x == 96 || v->x == 74 ||v->x == 52 ||v->x == 30) && v->y == v->randposy && v->state != 0){
					v->state = 0;
					v->park = 1;
					v->stoptime = step + genrandTemp(v->type); // génére une temps aléatoire d'arrêt du vehicule
				}
				if(v->back == 1 && (v->x == 104 || v->x == 82 || v->x == 60 || v->x == 38 || v->x == 16)){// tourne le vehicule vers le bas quand il est sorti de sa place
					turnVehicule(v,'S',x,y,vehiculeArray);
					//v->y += 1;
					v->back = 0;
				}
			}

			if(v->randposx == 116 || v->randposx == 94 || v->randposx == 72 || v->randposx == 50 || v->randposx == 28){
				if((v->x == 114 || v->x == 92 ||v->x == 70 ||v->x == 48 || v->x == 26) && v->y-1 == v->randposy-1 && v->state != 0){
					v->state = 0;
					v->park = 1;
					v->stoptime = step + genrandTemp(v->type);
				}
				if(v->back == 1 && (v->x == 105 || v->x == 83 || v->x == 61 || v->x == 39 || v->x == 17) && v->y != 12){
					turnVehicule(v,'S',x,y,vehiculeArray);
					v->y += 2;
					v->back = 0;
				}
			}

			if(v->randposx == 119){
				if(v->x == 118 && v->y == v->randposy&& v->state != 0){
					v->state = 0;
					v->park = 1;
					v->truck = 1;
					v->stoptime = step + genrandTemp(v->type);
				}
				if(v->back == 1 && v->x == 129){
					turnVehicule(v,'N',x,y,vehiculeArray);
					v->back = 0;
				}
			}
			if(v->y == 1 && v->x == 16){
					turnVehicule(v,'S',x,y,vehiculeArray);
			}

			if(step == v->stoptime && step != 0){// si la voiture dépasse le temps random fixé où elle doit être garé, on la fait reculer pour qu'elle sorte
				v->back = 1;
				v->park = 0;
				v->state = 3;
			}
			if(v->truck == 1 && v->x == 13){//debug sortie parking
				v->back = 0;
				v->truck = 0;
			}
			// Permet à la voiture d'avancer si elle est bloquée en backard avec une autre voiture
			if(v->timecollision == step && v->state == 0 && v->x == v->xcollision && v->y == v->ycollision){
				v->collision = 1;
			}
			//debug place haut droite
			if(v->collision == 1){
				v->back = 0;
				v->state = 1;
				v->collision = 0;
				v->var = 0;
			} 
		}

		if(v->type =='c'){ // si le vehicule est une camion 

			if(((v->direction == 'N' && array[v->x-1][v->y-1] == '-') || (v->direction == 'S' && array[v->x-1][v->y+1] == '-')) && v->y!=49){ //Si devant moi = barrière
				v->state = 0;
			}else if((v->x == 24 && v->y == 30) || (v->x == 117 && v->y == 28)){ 							
				v->state = 1;
			}

			if(v->back == 0){
				forward(v, x, y,array, vehiculeArray, l,last_pay,last_pay_dir,step);
			}
			if(v->back == 1){
				backward(v, x, y,array, vehiculeArray, l,step);
			}
            // fait rentrer le camion dans le parking du bas
			if(v->x == 24 && v->y == 24){
				turnVehicule(v,'S',x,y,vehiculeArray);	
			}
			if(v->x == 22 && v->y == 20){
				turnVehicule(v,'S',x,y,vehiculeArray);	
			}
			if(v->x == 24 && v->y == 50){
				turnVehicule(v,'E',x,y,vehiculeArray);
			}
			if(v->randposx == 142 && v->randposy == 49){ // permet au camion de ne pas tourner au nord en bas à droite si il doit se garer à la position bas droite
				v->truck = 1;
			}
			if((v->y == 47 && v->x == 119) && (v->truck == 0 || v->truck == 2)){// direction dans parking
				turnVehicule(v,'N',x,y,vehiculeArray);
			}
			if(v->randposx ==  4 && v->randposy == 49 && v->state != 3){ // permet de garer le camion en bas à gauche en backward
				if(v->x == 32 && v->y == 47 && v->truck == 0){
				 	v->back = 1;
				}
				if(v->x == 10 && v->y == 47 && v->state != 0){ // arrête le camion à sa place
					v->back = 0;
					v->state = 0;
					v->park = 1;
					v->truck = 2;
					v->stoptime = step + genrandTemp(v->type);
				}
			}

			// fait tourner le camion quand il arrive au niveau d'une rangée où il doit se garer
			if(v->randposx == 45 || v->randposx == 72){
				if(v->x == 57 && v->y == 47){
					turnVehicule(v,'N',x,y,vehiculeArray);
				}
			}
			if(v->randposx == 75 || v->randposx == 101){
				if(v->x == 87 && v->y == 47){
					turnVehicule(v,'N',x,y,vehiculeArray);
				}
			}
			// fait tourner le camion quand il arrive au niveau de la place où il doit se garer
			if((v->x + 16 == v->randposx)&&(v->y == v->randposy-3)){
				turnVehicule(v,'E',x,y,vehiculeArray);
			}
			if((v->x - 10 == v->randposx)&&(v->y == v->randposy-4)){
				turnVehicule(v,'O',x,y,vehiculeArray);
			}
			if(v->x + 17 == v->randposx && v->y == v->randposy-3 && v->randposx == 72){
				turnVehicule(v,'E',x,y,vehiculeArray);
			}
			// fait tourner le camion pour les deux première rangés
			if((v->x == 24) &&(v->randposx == 42 || v->randposx == 4) && v->y != 48){
				if((v->randposx == 42)&& (v->y == v->randposy+1)){
					turnVehicule(v,'E',x,y,vehiculeArray);
				}
				if((v->randposx == 4)&& (v->y == v->randposy-1) && v->state != 3){
					turnVehicule(v,'O',x,y,vehiculeArray);
				}
			}
			// fait tourner le camion pour les deux dernière rangés
			if((v->x == 117) &&(v->randposx == 104 || v->randposx == 142)){ // VOIR ICI
				if((v->randposx == 104)&& (v->y == v->randposy-4)){
					turnVehicule(v,'O',x,y,vehiculeArray);
				}
				if((v->randposx == 142)&& (v->y == v->randposy-3)){
					turnVehicule(v,'E',x,y,vehiculeArray);
				}
			}
			if(v->randposx == 142 && v->randposy == 49&& v->state != 0){// cas de la place en bas à droite
				if(v->x == 139 && v->y == 47){
					v->state = 0;
					v->park = 1;
					v->stoptime = step + genrandTemp(v->type); 
				}
			}

			if((v->x == 2 || v->x == 43 || v->x == 73 || v->x == 102)&& v->y == v->randposy-1 && v->state != 0){ //arrête la voiture quand elle sur sa place (place gauche)
				v->state = 0;
				v->park = 1;
				v->stoptime = step + genrandTemp(v->type);
			}

			if(v->back == 1 && v->x == 24 && v->randposy != 49){// cas bas gauche
					turnVehicule(v,'S',x,y,vehiculeArray);
					//v->y += 1;
					v->back = 0;
			}
			// dirige le camion vers le nord pour le faire sortir après qu'il se soit garé
			if(v->randposx == 4 || v->randposx == 45 || v->randposx == 75 || v->randposx == 104 || v->randposx == 142){// le 142 à test apès debug delete
				if(v->back == 1 && (v->x == 52 || v->x == 114)&& v->randposx != 142){
					turnVehicule(v,'N',x,y,vehiculeArray);
					v->x += 3;
					v->y -= 4;
					v->back = 0;
				}
				if(v->back == 1 && v->x == 81 && v->randposx != 142){
					turnVehicule(v,'N',x,y,vehiculeArray);
					v->x += 4;
					v->y -= 4;
					v->back = 0;
				}
				if(v->back == 1 && v->randposx == 142 && v->x == 120){//LAAAAAAAA
					turnVehicule(v,'N',x,y,vehiculeArray);
					v->x -= 1;
					v->back = 0;
				}
				if(v->back == 1 && v->randposx == 4 && v->x == 22 && v->randposy != 49){
					turnVehicule(v,'S',x,y,vehiculeArray);
					//v->x += 1;
					v->back = 0;
				}	
			}
			if(v->randposx == 72 || v->randposx == 101){// le 142 à test apès debug delete
				if(v->back == 1 && (v->x == 60 || v->x == 90)){
					turnVehicule(v,'N',x,y,vehiculeArray);
					v->x -= 3;
					v->y -= 1;
					v->back = 0;
				}	
			}
			
			if((v->x == 39 || v->x == 69 ||v->x == 98 ||v->x == 139) && v->y-1 == v->randposy-3 && v->y != 47 && v->state != 0){//arrête la voiture quand elle sur sa place (place droite) 
					v->state = 0;
					v->park = 1;
					v->stoptime = step + genrandTemp(v->type);
			}

			if((v->x == 55 || v->x == 85) && (v->y== 31)){
				turnVehicule(v,'E',x,y,vehiculeArray);
			}


			if((v->y == 39 || v->y == 32) && v->x == 119){
				turnVehicule(v,'N',x,y,vehiculeArray);
				v->y +=1;
			}

			if(v->y == 23 && v->x == 117){
				turnVehicule(v,'E',x,y,vehiculeArray);
				if(l_truck->premier != NULL){
					addEndPlaceList(l_truck->premier, newPlace(v->randposx,v->randposy));
				}else{
					l_truck->premier = newPlace(v->randposx,v->randposy);
				}
				*last_pay_dir = 1;
				*last_pay = getPrice(v);
			}
			// si le camion dépasse le temps random fixé où il doit être garé, on le fait reculer pour qu'il sorte
			if(step == v->stoptime && step != 0){
				v->back = 1;
				v->park = 0;
				v->state = 3;
			}
			if(v->x == 10 && v->y == 47){
				if(step == v->stoptime && step != 0){
					v->back = 0;
					v->park = 0;
					v->state = 3;
				}
			}
			// permet au camion d'avancer si il est bloqué en backard avec un autre camion
			if(v->timecollision == step && v->state == 0 && v->x == v->xcollision && v->y == v->ycollision){
				v->collision = 1;
			}
			if(v->collision == 1){
				v->back = 0;
				v->state = 1;
				v->collision = 0;
				v->var = 0;
			}
		}

		if(v->type == 'm'){// si le vehicule est une moto

			if(((v->direction == 'N' && array[v->x-1][v->y-1] == '-') || (v->direction == 'S' && array[v->x-1][v->y+1] == '-')) && v->y!=49){ //Si devant moi = barrière
				v->state = 0;
			}else if((v->x == 22 && v->y == 16) || (v->x == 118 && v->y == 17)){ 					
				v->state = 1;
			}

			if(v->back == 0){
				forward(v, x, y,array, vehiculeArray, l,last_pay,last_pay_dir,step);// fait avancer la moto
			}
			if(v->back == 1){
				backward(v, x, y,array, vehiculeArray, l,step);// fait reculer la moto
			}
			// fait rentrer la moto dans le parking et l'emmène jusqu'en bas à gauche
			if(v->x == 119 && (v->y == 23 || v->y == 20)){
				turnVehicule(v,'N',x,y,vehiculeArray);
				v->speed = 1;// réduis le vitesse de la moto quand elle arrive à proximité des places
			}
			if(v->x == 118 && v->y == 12){
				turnVehicule(v,'E',x,y,vehiculeArray);
			}
			if(v->randposx == 142 && v->randposy == 14){
				v->truck = 1;// empeche la moto d'aller vers le sud si elle doit aller se garer en bas à droite
			}
			if(v->x == 133 && v->y == 13 && v->truck == 0){
				turnVehicule(v,'N',x,y,vehiculeArray);
				v->back = 0;
			}
			if(v->x == 132 && v->y == 1){
				turnVehicule(v,'O',x,y,vehiculeArray);
				v->speed = 2;
			}
			if(v->x == 10 && v->y == 1 && v->randposy != 2 && v->back == 0){
				turnVehicule(v,'S',x,y,vehiculeArray);
				v->speed = 1;
			}
			if(v->x == 10 && v->y == 1){
				v->speed = 1;
			}
			if(v->randposx == 5 && v->randposy == 2){ // arrête la moto si elle arrive sur sa place
				if(v->x == 3 && v->y == 1 && v->state != 0){
					v->state = 0;
					v->park = 1;
					v->stoptime = step + genrandTemp(v->type);
				} 
			}

			if(v->x == 10 && v->y == 13){
				turnVehicule(v,'E',x,y,vehiculeArray);
			}

			if(v->randposx == 4 && v->randposy == 14){
				if(v->x == 15 && v->y == 13 && v->truck == 0){// fait rentrer la moto en backward si elle doit se garer en bas à gauche
					v->back = 1;
				}
				if(v->x == 6 && v->y == 13 && v->state != 0){
					v->state = 0;
					v->park = 1;
					v->back = 0;
					v->stoptime = step + genrandTemp(v->type);
					v->truck = 1;
				}
			}
			// fait sortir la moto du parking et la remet sur la route
			if(v->x == 21 && v->y == 13){
				turnVehicule(v,'S',x,y,vehiculeArray);
				v->x += 1;
			}
			if(v->x == 22 && v->y == 21){
				turnVehicule(v,'O',x,y,vehiculeArray);
				v->y += 1;
				v->speed = 2;

				if(l_moto->premier != NULL){
					addEndPlaceList(l_moto->premier, newPlace(v->randposx,v->randposy));
				}else{
					l_moto->premier = newPlace(v->randposx,v->randposy);
				}
				*last_pay_dir = 0;
				*last_pay = getPrice(v);
			}
			// fait tourner la moto quand elle arrive en face de sa place (à droite) puis l'arrête
			if((v->randposx == 142) && (v->randposy == 14 || v->randposy == 12 || v->randposy == 10 || v->randposy == 8 ||v->randposy == 6 ||v->randposy == 4)){
				if(v->x == 132 && (v->y == v->randposy-2)){
					turnVehicule(v,'E',x,y,vehiculeArray);
				}
				if(v->x == 140 && v->y == v->randposy-1 && v->state != 0){
					v->state = 0;
					v->park = 1;
					v->stoptime = step + genrandTemp(v->type);
				}
			}
			if(v->back == 1 && v->x == 133){ // dirige la moto vers le nord après qu'elle ai quitté sa place de droite
					turnVehicule(v,'N',x,y,vehiculeArray);
					v->back = 0;
			}
			// fait tourner la moto quand elle arrive en face de sa place (à gauche) puis l'arrête
			if((v->randposx == 4) && (v->randposy == 14 || v->randposy == 12 || v->randposy == 10 || v->randposy == 8 ||v->randposy == 6 ||v->randposy == 4 || v->randposy == 2)){
				if(v->x == 10 && (v->y == v->randposy+2) && v->state != 3 && v->truck == 0){
					turnVehicule(v,'O',x,y,vehiculeArray);
				}
				if(v->x == 3 && v->y == v->randposy-1 && v->state != 0){
					v->state = 0;
					v->park = 1;
					v->stoptime = step + genrandTemp(v->type);
					v->truck = 1;
				}
			}
			// dirige la moto vers le sud après qu'elle ai quitté sa place de gauche
			if(v->randposy != 14){
				if(v->back == 1 && v->x == 10){
						turnVehicule(v,'S',x,y,vehiculeArray);
						v->back = 0;
				}
			}

			if(step == v->stoptime && step != 0){ // lorsque la moto est restée garé  le temps aléatoire qu'on lui a fixé, elle recule
				v->back = 1;
				v->park = 0;
				v->state = 3;
			}

			if(v->x == 6 && v->y == 13){
				if(step == v->stoptime && step != 0){// ici on la fait avancer quand le temps est dépassé car c'est le cas en bas à gauche
					v->back = 0;
					v->park = 0;
					v->state = 3;
				}
			}
			// permet à la moto d'avancer si elle est bloquée en backard avec une autre moto
			if(v->timecollision == step && v->state == 0 && v->x == v->xcollision && v->y == v->ycollision){
				v->collision = 1;
			}
			//debug place haut droite
			if(v->collision == 1){
				v->back = 0;
				v->state = 1;
				v->collision = 0;
				v->var = 0;
			} 

		}
	}
}




void printVehicules(Vehicule* v){ //Affichage des vehicules
	if(v->next != NULL){
		printVehicules(v->next); //Pour tous les vehicules
	}
	
	char* path = pathConstruct(v);	
	FILE* f = NULL;
	f = fopen(path, "r");
	if(f==NULL){
		perror("fopen");
		system("kill `pidof play` 2> /dev/null");
		exit(-1);
	}
	char c = fgetc(f);	
	int i,j;
	int x = v->x; //0
	int y = v->y; //0
	i = 0;
	j = 0;

	while(c != EOF){ //On affiche caractère par caractère
		if(c == '\n'){
			j++;
			i=0;
		}
		switch(v->direction){ //Décalage du curseur selon la direction du vehicule : Point de référence des vehicules = devant à gauche pour toutes les directions
			case 'N':
				setConsoleCursor(j+y+1, i+x+1);
				break;
			case 'E':
				setConsoleCursor(j+y+1, i+x-v->long_x+2);
				break;
			case 'S':
				setConsoleCursor(j+y+2-v->long_y, i+x-v->long_x+2); 
				break;
			case 'O':
				setConsoleCursor(j+y+2-v->long_y, i+x+1);
				break;
		}
		if(v->state >= 0){ //Si le vehicule n'est pas crashé, on affiche sa couleur sur certains caractères
			if(v->type == 'c'){
				if(c == '2'){
					printf("%s", getColor(v->color));
				}else{
					printf("%s", getColor(1));
				}
			}
			else if(v->type == 'v'){
				if(c == '6' || c == '9' || c == '@'){
					printf("%s", getColor(v->color));
				}else{
					printf("%s", getColor(1));
				}
			}
			else{
				if(c == '[' || c == '|' || c == 'u'){
					printf("%s", getColor(v->color));
				}else{
					printf("%s", getColor(1));
				}
			}
		}else{ //Si le vehicule est crashé, on affiche entièrement sa couleur (rouge)
			printf("%s", getColor(v->color));
		}
		printUnicode(c);
		printf("%s", getColor(1));
		c = fgetc(f);
		i++;
	}
	setConsoleCursor(50,0);
	fclose(f);
}

