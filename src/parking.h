void showParking(int x, int y, char array[x][y]);
void loadParking(int x, int y, char array[x][y]);
void initParking(int x, int y, char parkingBoard[x][y]);
void initVehicule(int x, int y, char array[x][y]);
FILE* openFile(char* path);
void showParkingRaw(int x, int y, char array[x][y]);
void barrierOpen(int x, int y, int x_size, int y_size, char parkingArray[x_size][y_size]);
void barrierClose(int x, int y, int x_size, int y_size, char parkingArray[x_size][y_size]);
void barrierLogic(int x, int y, int x_size, int y_size, char vehiculeArray[x_size][y_size],char parkingArray[x_size][y_size], unsigned long long int step, unsigned long long *tmp_step);