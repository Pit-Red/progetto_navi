
#include <stdio.h>
#include <unistd.h>
#include <time.h>
int i, j;

void rmLinesTerminal(int n) {
	int i;
	for (i = 0; i < n - 1; i++) {
		printf("\33[2K\r");
		printf("\033[A\r");
	}
	printf("\33[2K\r");
}

void barraRichieste(int rich_sodd, int rich_tot) {
	int nbarra;
	nbarra = (rich_tot / rich_sodd) / 3;
	char barra[33];
	printf(" 					  ]\n");
	for (i = 0; i < nbarra; i++ ) {
		barra[i] = '|';
	}
	barra[i + 1] = '\0';
	printf("\033[\r");
	printf("[%s", barra);

}

int main() {
	char space[60];






	barraRichieste(10,30);







	/*printf("\033[0;40m");
	printf("\n 				BARCHETTA\n\n\n");
	printf("\n\n");


	printf("\n\n");
	printf("              |    |    |                 \n");
	printf("             )_)  )_)  )_)              \n");
	printf("            )___))___))___)              \n");
	printf("           )____)____)_____)		\n");
	printf("         _____|____|____|____			\n");
	printf("---------\\                   /---------\n");
	printf("  ^^^^^ ^^^^^^^^^^^^^^^^^^^^^\n");
	printf("    ^^^^      ^^^^     ^^^    ^^\n");
	printf("         ^^^^      ^^^\n");
	printf("\n");
	printf("\n\n");

	printf("                __/___\n");
	printf("          _____/______|\n");
	printf("   ______/_____\\_______\\_____\n");
	printf("   \\              < < <      |\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\n");



	for (i = 0; i < 5; i++) {

		printf("\033[0;40m		%d\n",i);
		printf("%s            __\\____						\n", space);
		printf("%s         ___|______\\__						\n", space);
		printf("%s  ______/_____\\_______\\____				\n", space);
		printf("%s  \\     > > >             /					", space);
		printf("\033[0;44m\033[5m\n~    ~~~ ~~ ~~  ~    ~~ ~ ~~   ~~~    ~~     ~~  ~  ~~~ \n");
		printf("~~~~  ~~ 	~~~	 ~~  ~~~  ~~   ~~     ~   ~~  ~~\n");
		printf("~ ~  ~~~    ~~  ~~  ~  ~~ ~  ~~ ~~~~    ~~ ~  ~~    ~~~\n");
		for (j = 0; j < 7; j++) {
			printf("\033[A\r");
		}
		space[i] = ' ';
		sleep(1);
	}
	printf("\033[0;39m");
	printf("\033[0;49m");
	printf("\n\n\n\n\n\n\n\n");
	rmLinesTerminal(8);*/




	printf("\n\n\n");




	/*printf("_________________________\n");
		printf("\33[2K\r");
	printf("|_______________________|\n");*/

/*
	printf("[								]\n");
	printf("\033[0;33m");
	printf("\033[A\r");

	for (i = 0; i < 48; i++) {
		printf("|");
	}
	printf("\033[0;39m");
	printf("\n");
*/

	//printf("\n\n");


	//printf("\n\n\n\n\n");
	/*printf("\033[0;34m██\n\033[0;39m");*/
	/*printf("\033[4m\ntesto lineuzza\033[0;39m");
	printf("\033[5m\ntesto blinkoso\033[0;39m");
	printf("\033[47m\ntesto babushka\n\033[0;39m");
	/*printf("\033[0;31m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\033[0m");
	printf("\033[0;32m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\033[0m");
	printf("\033[0;33m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\033[0m");
	printf("\033[0;34m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\033[0m");
	printf("\033[0;35m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\033[0m");
	printf("\033[0;36m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\033[0m");
	printf("\033[0;37m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\033[0m");*/

}