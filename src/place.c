#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include "struct.h"
#include "place.h"
#include "config.h"
#include "random.h"

void colorPlace(int x, int y, char parkingArray[x][y], char vehiculeArray[x][y]){
    int i, j = 0;
    for(j=0;j<y;j++){
        for(i=0;i<x;i++){ //On parcourt le parking
                if(parkingArray[i][j] == '#' || parkingArray[i][j] == '%'){ //Pour chaque lumières qui doivent vérifier un vehicule à leur gauche
                    //S'il y a un vehicule devant
                    if(vehiculeArray[i-1][j+1] != '0' || vehiculeArray[i-1][j-1] != '0' || vehiculeArray[i-1][j] != '0' || vehiculeArray[i-2][j+1] != '0' || vehiculeArray[i-2][j-1] != '0' || vehiculeArray[i-2][j] != '0'){
                        parkingArray[i][j] = '%'; //On met le code ASCII d'une lumière rouche
                    }else{
                        parkingArray[i][j] = '#'; //Sinon on met le code ASCII d'une lumière verte
                    }
                }else if(parkingArray[i][j] == '?' || parkingArray[i][j] == 'r'){ //Même algorithme mais les lumières verifient les vehicules à leur droite
                    if(vehiculeArray[i+2][j-1] != '0' || vehiculeArray[i+2][j] != '0' || vehiculeArray[i+2][j+1] != '0' || vehiculeArray[i+3][j-1] != '0' || vehiculeArray[i+3][j] != '0' || vehiculeArray[i+3][j+1] != '0'){
                        parkingArray[i][j] = 'r';
                    }else{
                        parkingArray[i][j] = '?';
                    }
                }
        }
    }
}


void addEndPlaceList(Place* actuel, Place* p){ //Ajout d'une place : On ajoute à la fin de la liste chaînée
	if(actuel->x != p->x || actuel->y != p->y){ //Verification que la place n'est pas déjà dans la liste (éviter les doublons)
        if(actuel->next == NULL){
            actuel->next = p;
        }
        else{
            addEndPlaceList(actuel->next, p);
        }
    }
}

Place* newPlace(int x, int y){ //Création d'une place de coordonées (x,y)
    Place* p = NULL;
    p = init_place(p);
    p->x = x;
    p->y = y;
    p->next= NULL;
    return p;
}

list_p* init_placeList(list_p* list){ //Allocation de la liste chaînée
	list = malloc(sizeof(list_p));
	list->premier=NULL;
	return list;
}


Place* init_place(Place* p){ //Allocation de la place
	p = malloc(sizeof(Place));
    return p;
}

void fillPlaceList(list_p* l_car, list_p* l_truck, list_p* l_moto){ //Ajout de toutes les places (x,y) du parking dans les listes chaînées associées
    //Car
    l_car->premier = newPlace(119,4);
    addEndPlaceList(l_car->premier, newPlace(119,7));
    addEndPlaceList(l_car->premier, newPlace(119,7));
    addEndPlaceList(l_car->premier, newPlace(119,10));
    addEndPlaceList(l_car->premier, newPlace(116,4));
    addEndPlaceList(l_car->premier, newPlace(116,7));
    addEndPlaceList(l_car->premier, newPlace(116,10));
    addEndPlaceList(l_car->premier, newPlace(97,4));
    addEndPlaceList(l_car->premier, newPlace(97,7));
    addEndPlaceList(l_car->premier, newPlace(97,10));
    addEndPlaceList(l_car->premier, newPlace(94,4));
    addEndPlaceList(l_car->premier, newPlace(94,7));
    addEndPlaceList(l_car->premier, newPlace(94,10));
    addEndPlaceList(l_car->premier, newPlace(75,4));
    addEndPlaceList(l_car->premier, newPlace(75,7));
    addEndPlaceList(l_car->premier, newPlace(75,10));
    addEndPlaceList(l_car->premier, newPlace(72,4));
    addEndPlaceList(l_car->premier, newPlace(72,7));
    addEndPlaceList(l_car->premier, newPlace(72,10));
    addEndPlaceList(l_car->premier, newPlace(53,4));
    addEndPlaceList(l_car->premier, newPlace(53,7));
    addEndPlaceList(l_car->premier, newPlace(53,10));
    addEndPlaceList(l_car->premier, newPlace(50,4));
    addEndPlaceList(l_car->premier, newPlace(50,7));
    addEndPlaceList(l_car->premier, newPlace(50,10));
    addEndPlaceList(l_car->premier, newPlace(31,4));
    addEndPlaceList(l_car->premier, newPlace(31,7));
    addEndPlaceList(l_car->premier, newPlace(31,10));
    addEndPlaceList(l_car->premier, newPlace(28,4));
    addEndPlaceList(l_car->premier, newPlace(28,7));
    addEndPlaceList(l_car->premier, newPlace(28,10)); 

    //Truck
    l_truck->premier = newPlace(4,34);
    addEndPlaceList(l_truck->premier, newPlace(142,34));
    addEndPlaceList(l_truck->premier, newPlace(42,39));
    addEndPlaceList(l_truck->premier, newPlace(45,39));
    addEndPlaceList(l_truck->premier, newPlace(72,39));
    addEndPlaceList(l_truck->premier, newPlace(75,39));
    addEndPlaceList(l_truck->premier, newPlace(101,39));
    addEndPlaceList(l_truck->premier, newPlace(104,39));
    addEndPlaceList(l_truck->premier, newPlace(42,44));
    addEndPlaceList(l_truck->premier, newPlace(45,44));
    addEndPlaceList(l_truck->premier, newPlace(72,44));
    addEndPlaceList(l_truck->premier, newPlace(75,44));
    addEndPlaceList(l_truck->premier, newPlace(101,44));
    addEndPlaceList(l_truck->premier, newPlace(104,44));
    addEndPlaceList(l_truck->premier, newPlace(4,49));
    addEndPlaceList(l_truck->premier, newPlace(142,49));
    addEndPlaceList(l_truck->premier, newPlace(4,44));
    addEndPlaceList(l_truck->premier, newPlace(4,39));
    addEndPlaceList(l_truck->premier, newPlace(4,34));   
    addEndPlaceList(l_truck->premier, newPlace(142,44));
    addEndPlaceList(l_truck->premier, newPlace(142,39));

    //Moto
    l_moto->premier = newPlace(4,2);
    addEndPlaceList(l_moto->premier, newPlace(4,4));
    addEndPlaceList(l_moto->premier, newPlace(4,6));
    addEndPlaceList(l_moto->premier, newPlace(4,8));
    addEndPlaceList(l_moto->premier, newPlace(4,10));
    addEndPlaceList(l_moto->premier, newPlace(4,12));
    addEndPlaceList(l_moto->premier, newPlace(4,14));
    addEndPlaceList(l_moto->premier, newPlace(142,4));
    addEndPlaceList(l_moto->premier, newPlace(142,6));
    addEndPlaceList(l_moto->premier, newPlace(142,8));
    addEndPlaceList(l_moto->premier, newPlace(142,10));
    addEndPlaceList(l_moto->premier, newPlace(142,12));
    addEndPlaceList(l_moto->premier, newPlace(142,14)); 
}


int getSizeOf(Place* p){ //Retourne la taille de la liste chaînée
    int r = 1;
    if(p->next != NULL){
        r += getSizeOf(p->next);
    }
    return r;
}

Place* getPlaceByIndex(Place* p, int index, int acc){ //Retourne une place selon son index dans la liste
    Place* p_temp = NULL;
    p_temp = malloc(sizeof(Place));

    if(acc+1==index){
        return p;
    }
    if(p->next != NULL){
        p_temp = getPlaceByIndex(p->next, index, acc+1);
    }
    return p_temp;
}



int deleteFromPlaceList(Place* current, Place *p){ //Supprime une place de position quelconque
	if(current->next == p){
		current->next = p->next;
		free(p);
		return 1;
	}
	deleteFromPlaceList(current->next, p);
	return 0;
}

void deleteFirstFromPlaceList(list_p *l, Place *p){ //Supprime une place en début de liste
	if(p->next != NULL){
		l->premier = p->next;
	}else{
		l->premier = NULL;
	}
	free(p);
}

int deleteLastFromPlaceList(Place* current, Place* p){ //Supprime une place en fin de liste
	if(current->next == p){
		current->next = NULL;
		free(p);
		return 1;
	}
	deleteLastFromPlaceList(current->next, p);
	return 0;
}

void deletePlace(list_p *l, Place *p){ //Supprime une place dans la liste chaînée
	if(l->premier == p){ //Cas 1 : La place est au début de la liste
		deleteFirstFromPlaceList(l, p);
	}else if(p->next == NULL){ //Cas 2 : La place est à la fin de la liste
		deleteLastFromPlaceList(l->premier, p);
	}else{ //Cas 3 : La place est à une position quelconque 
		deleteFromPlaceList(l->premier, p);
	}
}

void showPlace(Place* p){ //[DEBUG] : Affiche la liste chaînée des places disponibles
    if(p->next != NULL){
        showPlace(p->next);
    }
    printf("Place: (%d,%d)\n", p->x, p->y);
}

Place* findPlace(Vehicule* v, list_p* p){ //Permet à un vehicule de trouver une place
    int rand;
    if(p->premier != NULL){ //S'il reste au moins une place
        rand = randInt(1, getSizeOf(p->premier)); //On choisit un random dans [1, sizeOf(liste)]
        Place* pl = getPlaceByIndex(p->premier,rand,0); //On récupère la place d'index du random généré
        return pl; //On retourne la place
    }else{ //S'il n'y a PAS de places disponible
        Place* noPlace = NULL;
        noPlace = init_place(noPlace);
        noPlace->x = -1;
        noPlace->y = -1;
        return noPlace; //On retourne une place de coordonnées (-1,-1) = Le vehicule n'a pas de place
    }
    return p->premier;

}