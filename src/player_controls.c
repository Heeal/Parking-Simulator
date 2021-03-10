#include <stdio.h>

#include "struct.h"
#include "console.h"
#include "vehicle.h"

//Deplacer la voiture du joueur selon le caractère 'key' appuyé
void moveVehicule(char key, Vehicule* v, int x_size, int y_size,char parkingBoard[x_size][y_size], char vehiculeBoard[x_size][y_size], list* l, float *last_pay, unsigned int *last_pay_dir, unsigned long long int step){
    if(key == MOVE_LEFT){ //Si on appuie sur avancer à gauche 
        if(v->direction == 'O'){ //Si le vehicule est déjà vers la gauche : On avance
            forward(v, x_size, y_size,parkingBoard, vehiculeBoard, l, last_pay, last_pay_dir,step);
        }else if(v->direction == 'E'){ //Si le vehicule est vers la droite : On recule
            backward(v, x_size, y_size,parkingBoard, vehiculeBoard, l,step);
        }else{ //Sinon on tourne
            turnVehicule(v,'O',x_size,y_size,vehiculeBoard);
        }
    }

     if(key == MOVE_UP){  //Si on appuie sur avancer vers le haut
        if(v->direction == 'N'){ //Si le vehicule est déjà vers le haut : On avance
            forward(v, x_size, y_size,parkingBoard, vehiculeBoard, l, last_pay, last_pay_dir,step);
        }else if(v->direction == 'S'){ //Si le vehicule est vers le bas : On recule
            backward(v, x_size, y_size,parkingBoard, vehiculeBoard, l,step);
        }else{ //Sinon on tourne
            turnVehicule(v,'N',x_size,y_size,vehiculeBoard);
        }
    }

     if(key == MOVE_RIGHT){  //Si on appuie sur avancer à droite
        if(v->direction == 'E'){ //Si le vehicule est déjà vers la droite : On avance
            forward(v, x_size, y_size,parkingBoard, vehiculeBoard, l, last_pay, last_pay_dir,step);
        }else if(v->direction == 'O'){ //Si le vehicule est vers la gauche : On recule
            backward(v, x_size, y_size,parkingBoard, vehiculeBoard, l,step);
        }else{ //Sinon on tourne
            turnVehicule(v,'E',x_size,y_size,vehiculeBoard);
        }
    }

     if(key == MOVE_DOWN){ //Si on appuie sur avancer vers le bas
        if(v->direction == 'S'){ //Si le vehicule est déjà vers le bas : On avance
            forward(v, x_size, y_size,parkingBoard, vehiculeBoard, l, last_pay, last_pay_dir,step);
        }else if(v->direction == 'N'){ //Si le vehicule est vers le haut : On recule
            backward(v, x_size, y_size,parkingBoard, vehiculeBoard, l,step);
        }else{ //Sinon on tourne
            turnVehicule(v,'S',x_size,y_size,vehiculeBoard);
        }
    }
}