#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "struct.h"
#include "parking.h"
#include "console.h"
#include "vehicle.h"
#include "random.h"
#include "config.h"
#include "place.h"
#include "player_controls.h"

int main(){
	srand(time(NULL));

	char parkingBoard[PARKING_X_SIZE][PARKING_Y_SIZE]; //Matrice du parking
	char vehiculeBoard[PARKING_X_SIZE][PARKING_Y_SIZE]; //Matrice de vehicules

	list* l = malloc(1*sizeof(list)); //Liste chaînée des vehicules
	l = init_list(l);

	list_p* l_car = NULL; //Listes chaînées des places disponibles pour les voitures/camions/moto
	list_p* l_truck = NULL;
	list_p* l_moto = NULL;

	l_car = init_placeList(l_car);
	l_truck = init_placeList(l_truck);
	l_moto = init_placeList(l_moto);

	int nb_crash = 0; //Nombre de crash
	unsigned long long int step = 0;
	int spawnRate; //Taux de spawn de vehicules
	unsigned int last_pay_dir = 0; //0->voiture est partie à gauche, 1->voiture est partie à droite
	float last_pay = 0; //Dernier payement
	unsigned long long int barrierStep[5]= {0}; //Tableau d'ouverture des barrières
	int tick_rate = TICK_RATE;
	unsigned int game_over = 0;
	Vehicule* v_player = NULL;
	v_player = init_v(v_player);
	if(ACTIVATE_MUSIC == 1){
		system("play -q files/music/music.mp3 &");
	}
	clearConsole();
	spawnRate = mainMenu(); //Affichage du menu de jeu et récupèration du spawnrate selon le mode de jeu choisi
	clearConsole();

	initParking(PARKING_X_SIZE, PARKING_Y_SIZE, parkingBoard);  //Initialisation du tableau du parking 
	initVehicule(PARKING_X_SIZE, PARKING_Y_SIZE, vehiculeBoard); //Initialisation du tableau de vehicules
	fillPlaceList(l_car,l_truck,l_moto); //Ajout de toutes les places du parking dans la liste chaînée
	loadParking(PARKING_X_SIZE, PARKING_Y_SIZE, parkingBoard); //Remplissage de la matrice du parking depuis le fichier source parking.txt
	

	while(key_pressed() != QUIT_BUTTON){ //Tant que le joueur n'a pas quitté
		if(spawnRate == -1 && l->premier == NULL){
			v_player = newVehicule(l,'v',135,20,2,'O',PARKING_X_SIZE, PARKING_Y_SIZE, vehiculeBoard, step, l_car);
			v_player->speed = 1;
			tick_rate = 60000;
		}
		if(l->premier != NULL && game_over == 0){
			nb_crash += deleteCrashedVehicules(l,l->premier, l_car,l_truck,l_moto,step); //On compte le nombre de crash et on détruit les vehicules crash
		}
		if(step%spawnRate == 10 && spawnRate != -1){ //Tous les step multiple du spawnrate : Ajout d'un vehicule aléatoire sur le parking
			newRandVehicle(l, PARKING_X_SIZE, PARKING_Y_SIZE, vehiculeBoard,step,l_car,l_truck,l_moto);
		}

		if(l->premier != NULL && spawnRate != -1){ //S'il y a au moins un vehicule : Appel de l'algorithme des vehicules
			carLogic(l->premier,PARKING_X_SIZE,PARKING_Y_SIZE,parkingBoard,step,vehiculeBoard, l,l_car,l_truck,l_moto,&last_pay,&last_pay_dir);
		}
		if(spawnRate == -1 && l->premier != NULL && game_over == 0){
			moveVehicule(key_pressed(),v_player,PARKING_X_SIZE, PARKING_Y_SIZE,parkingBoard,vehiculeBoard,l,&last_pay,&last_pay_dir,step);
		}
		//Appel de l'algorithme des barrières
		barrierLogic(23,31,PARKING_X_SIZE, PARKING_Y_SIZE,vehiculeBoard, parkingBoard,step, &barrierStep[0]);
		barrierLogic(23,17,PARKING_X_SIZE, PARKING_Y_SIZE,vehiculeBoard, parkingBoard,step, &barrierStep[1]);
		barrierLogic(119,16,PARKING_X_SIZE, PARKING_Y_SIZE,vehiculeBoard, parkingBoard,step, &barrierStep[2]);
		barrierLogic(118,27,PARKING_X_SIZE, PARKING_Y_SIZE,vehiculeBoard, parkingBoard,step, &barrierStep[4]);

		//On raffraichit l'écran
		if(game_over == 0){
			refreshScreen(parkingBoard, vehiculeBoard, l,&last_pay,&last_pay_dir);
		}

		if(spawnRate == -1){
			if(nb_crash != 0){
				game_over = 1;
			}
		}
		//On affiche les infos principales du parking en bas de l'écran
		if(game_over == 1){
			setConsoleCursor(21,70);
			printf("G A M E   O V E R\n");
		}

		if(game_over == 0){
			setConsoleCursor(55,0);	
			printf("step: \t\t\t%lld\n",step);
			printf("number of crash:\t%d\n",nb_crash);
			printf("\nPress %c for exit...\n", QUIT_BUTTON);
			fflush(stdout);
			step++; //Incrémentation du step
		}
		usleep(tick_rate); //On attend la durée d'un tick
	} 
	//Fin du jeu :
	clearConsole();
	asciiTextMenu("files/menu/endMenu.txt"); //Affichage du menu de fin et des credits
	if(ACTIVATE_MUSIC == 1){
		system("kill `pidof play` 2> /dev/null"); //On kill le processus lié à la musique (redirect error on /dev/null)
	}
	exit(-1);
	return 0;
}
