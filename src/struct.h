
/* Liste chainée de vehicules */
typedef struct vehicule Vehicule;
struct vehicule{
	char direction; // N-E-S-O
	int x;
	int y;
	int long_x;
	int long_y;
	int speed;
	char type; // v,c,m
	int color; //0-> white, 1-> red
	int state; //0 -> arriving, 1-> stopped, 2->go out of the parking
	int randposx;
	int randposy;
	int truck;
	int park;
	int collision;
	int timecollision;
	int xcollision;
	int ycollision;
	int var;
	unsigned long long int crashtime;
	unsigned long long int starttime;
	unsigned long long int stoptime;
	int back;// 0-> forward, 1->car goes back

	Vehicule* next;
};
typedef struct Liste list;
struct Liste{
	Vehicule* premier;
};


/* Liste chainée de place */
typedef struct place Place;
struct place{
	int x;
	int y;
	Place* next;
};


typedef struct ListePlace list_p;
struct ListePlace{
	Place* premier;
};
