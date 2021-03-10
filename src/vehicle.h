#include "config.h"

list* init_list(list* list);
Vehicule* init_v(Vehicule* v);
void addEndList(Vehicule* actuel, Vehicule* v);
Vehicule* newVehicule(list* l, char type, int x, int y, int color, char direction, int x_size, int y_size, char vehiculeBoard[x_size][y_size], int step, list_p* p);
void showList(Vehicule* actuel);
char *pathConstruct(Vehicule* v);
void forward(Vehicule* v, int x_size, int y_size, char parkingBoard[x_size][y_size], char vehiculeBoard[x_size][y_size], list* l, float *last_pay,unsigned int *last_pay_dir,unsigned long long int step);
void backward(Vehicule* v, int x_size, int y_size, char parkingBoard[x_size][y_size], char vehiculeBoard[x_size][y_size], list* l,unsigned long long int step);
void turnVehicule(Vehicule* v, char direction,int x,int y,char vehiculeBoard[x][y]);
void carLogic(Vehicule* v,int x, int y, char array[x][y],int step,char vehiculeArray[x][y], list* l, list_p* l_car,list_p* l_truck,list_p* l_moto,float *last_pay,unsigned int *last_pay_dir);
void printVehicules(Vehicule* v);
void showVehiculeArray(int x_size,int y_size,char vehiculeBoard[x_size][y_size]);
void updateVehiculeArray(Vehicule* v, int x_size, int y_size, char vehiculeBoard[x_size][y_size]);
void updateArray(Vehicule *premier, int x_size, int y_size, char vehiculeBoard[x_size][y_size]);
void updateLong(Vehicule* v);
int deleteFromList(Vehicule* current, Vehicule *v);
int deleteLastFromList(Vehicule* current, Vehicule *v);
void deleteFirstFromList(list* l, Vehicule *v);
void deleteVehicule(list *l, Vehicule *v);
void refreshScreen(char parkingBoard[PARKING_X_SIZE][PARKING_Y_SIZE], char vehiculeBoard[PARKING_X_SIZE][PARKING_Y_SIZE], list* l,float*last_pay,unsigned int *last_pay_dir);
void vehiculeCrash(Vehicule* v, list* l, int  x, int y, unsigned long long int step);
int deleteCrashedVehicules(list* l, Vehicule* v, list_p* l_car, list_p* l_truck, list_p* l_moto, unsigned long long int step);

