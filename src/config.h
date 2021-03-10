        /* USER SETTINGS */
//Music
#define ACTIVATE_MUSIC 1             //0: false, 1: true
//Game Settings
#define TICK_RATE 75000             //Duration of a tick in nanoseconds (default : 75000)
#define QUIT_BUTTON 120             //ASCII Code for the quit button (default: 120 = 'x')
//Controls in Play Mode
#define MOVE_UP 122                  //default : 122 = 'z'
#define MOVE_LEFT 113                //default : 113 = 'q'
#define MOVE_DOWN 115                //default : 115 = 's'
#define MOVE_RIGHT 100               //default : 100 = 'd'


        /* PARKING SETTINGS */

//Spawnrates : Spawn a vehicule every X ticks
#define SPAWNRATE_EASY 50           //In tick (default: 50)
#define SPAWNRATE_HARD 14           //In tick (default: 14)

//Spawnrates for vehicule (in pourcent
// /!\ The sum of Spawnrate_car and Spawnrate_moto MUST be less or equals than 100 !!!
#define SPAWRATE_CAR 50             //default: 50
#define SPAWRATE_MOTO 25            //default: 25
//Spawnrate for trucks will be : 100-(SPAWNRATE_CAR + SPAWNRATE_TRUCK) %

//Price per step for vehicules
#define PRICE_PER_STEP_CAR 0.05      //In $
#define PRICE_PER_STEP_MOTO 0.10     //In $
#define PRICE_PER_STEP_TRUCK 0.20    //In $

//Duration of parking time for vehicules will be a random int, in the range between the MIN and the MAX
//MIN
#define MIN_DURATION_PARK_CAR 300    //In tick
#define MIN_DURATION_PARK_MOTO 300   //In tick
#define MIN_DURATION_PARK_TRUCK 300  //In tick
//MAX
#define MAX_DURATION_PARK_CAR 700    //In tick
#define MAX_DURATION_PARK_MOTO 700   //In tick
#define MAX_DURATION_PARK_TRUCK 700  //In tick

//Fence : Duration (in step) that a fence need in order to open
#define FENCE_OPEN_TIME 10           //In step (default: 10)

//Crash
#define CRASH_TIME 30                //Duration of a crash in step (default: 70)













/* DO NOT MODIFY UNDER THOSES LINES*/
#define P_PATH "files/map/parking.txt"
#define PARKING_X_SIZE 146
#define PARKING_Y_SIZE 51
