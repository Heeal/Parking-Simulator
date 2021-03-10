#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <fcntl.h>
#include <ctype.h>

#include "parking.h"
#include "struct.h"
#include "config.h"



char key_pressed(){
	struct termios oldterm, newterm;
	int oldfd; char c, result = 0;
	tcgetattr (STDIN_FILENO, &oldterm);
	newterm = oldterm; newterm.c_lflag &= ~(ICANON | ECHO);
	tcsetattr (STDIN_FILENO, TCSANOW, &newterm);
	oldfd = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl (STDIN_FILENO, F_SETFL, oldfd | O_NONBLOCK);
	c = getchar();
	tcsetattr (STDIN_FILENO, TCSANOW, &oldterm);
	fcntl (STDIN_FILENO, F_SETFL, oldfd);
	if (c != EOF) {ungetc(c, stdin); result = getchar();}
	return result;
}


char* getColor(int color){ //Retourne le code couleur associé
	if(color == 0){ //Red
		return "\033[31m";
	}
	else if(color == 1){ //White
		return "\033[0m";
	}
	else if(color == 2){ //Green
		return "\033[32m";
	}
	else if(color == 3){ //Blue
		return "\033[34m";
	}
	else if(color == 4){ //Yellow
		return "\033[33m";
	}
	else if(color == 5){ //Gray
		return "\033[37m";
	}
	else if(color == 6){ //Black
		return "\033[30m";
	}
	else if(color == 7){ //Cyan
		return "\033[36m";
	}
	return "\033[0m";
}

void asciiTextMenu(char* path){ //Affichage des menus : Ouverture et affichage de fichier .txt
	FILE* f = fopen(path, "r");
	
	char c = fgetc(f);	
	while(c!=EOF){
		printf("%c", c);
		c = fgetc(f);
	}	
	fclose(f);
}

int mainMenu(){ //Menu Principal
	
	int choix = -1;
	asciiTextMenu("files/menu/menu.txt");
	
	while(choix != 1 || choix != 2){ //Tant que le choix n'est pas 1 ou 2, on redemande à l'utilisateur de choisir
		printf("\n\n\n\n\n\n\n\n\n\t\t\t\t\t\tYour choice : ");
		if(scanf("%d", &choix)==1){
			if(choix == 1) return SPAWNRATE_EASY; //On retourne le spawnRate du config.h du mode choisi
			if(choix == 2) return SPAWNRATE_HARD;
			if(choix == 3) return -1;
		}else{ //En cas d'erreur sur le scanf on ferme le programme
			system("kill `pidof play` 2> /dev/null");
			exit(-1);
		}
	}
	return -1;
}


void printUnicode(char c){ //Conversion ascii vers Unicode
	switch(c){
			case '-':
				printf("━");
				break;
			case'_':
				printf("─");
				break;
			case ' ':
				printf(" ");
				break;
			case '=':
				printf("─");
				break;
			case ':':
				printf("┨");
				break;
			case '*':
				printf("┠");
				break;
			case 'o':
				printf("┬");
				break;
			case 'k':
				printf("┴");
				break;
			case 'g':
				printf("┖");
				break;
			case 'b':
				printf("┎");
				break;
			case 'h':
				printf("┒");
				break;
			case 'x':
				printf("┚");
				break;
			case'/':
				printf("┼");
				break;
			case';':
				printf("│");
				break;
			case '1':
				printf("┠");
				break;
			case 'p':
				printf("┏");
				break;
			case 'j':
				printf("┓");
				break;
			case 'l':
				printf("┗");
				break;
			case 't':
				printf("┛");
				break;
			case '(':
				printf("┃");
				break;
			case '>':
				printf("↟");
				break;
			case '<':
				printf("↡");
				break;
			case '0':
				printf("░");
				break;
			case '2':
				printf("▒");
				break;
			case '3':
				printf("╊");
				break;
			case '4':
				printf("▄");
				break;
			case '5':
				printf("▀");
				break;
			case '6':
				printf("█");
				break;
			case '7':
				printf("╇━╇");
				break;
			case 'w':
				printf("╈━╈");
				break;
			case '8':
				printf("┷");
				break;
			case '$':
				printf("┯");
				break;
			case '9':
				printf("▐");
				break;
			case '@':
				printf("▌");
				break;
			case '!':
				printf("╉");
				break;
			case 'm':
				printf("╺╇╸");
				break;
			case '}':
				printf("╺┷╸");
				break;
			case '~':
				printf("╺╈╸");
				break;
			case '{':
				printf("╺┯╸");
				break;
			case ']':
				printf("⌾");
				break;
			case '|':
				printf("⌤");
				break;
			case 'u':
				printf("├");
				break;	
			case '.':
				printf("\n");
				break;
			case '#':
				printf("\033[32m◉\033[00m"); //lumière gauche green
				break;
			case '?':
				printf("\033[32m◉\033[00m");  //lumière droite green
				break;
			case 'r':
				printf("\033[31m◉\033[00m");  //lumière gauche red
				break;
			case '%':
				printf("\033[31m◉\033[00m");  //lumière droite red
				break;
			case '&':
				printf("┤");
				break;
			case 'v':
				printf("⊖");
				break;	
			case '+':
				printf("↰");
				break;
			case 'i':
				printf("⭡");
				break;
			case 'y':
				printf("⭣");
				break;
			case 'e':
				printf("⮦");
				break;
			case 'z':
				printf("⭠");
				break;
			case 'a':
				printf("⮣");
				break;
	 		case 'q':
				printf("⮧");
				break;
			case 'd':
				printf("⮡");
				break;
			case 'n':
				printf("⭢");
				break;
			case 'f':
				printf("⮥");
				break;
	 }
}


void setConsoleCursor(int x, int y){ //Deplacement du curseur dans le terminal
	printf("\033[%d;%dH",x,y);

}

void clearConsole(){ //Clear du terminal
	printf ("\33[H\33[2J");
}
