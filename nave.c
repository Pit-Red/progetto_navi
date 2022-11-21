#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <string.h>

#define SO_CAPACITY rand()%massimo + 50

/*HANDLER PER GESTIRE IL SEGNALE DI TERMINAZIONE DEL PADRE*/
void handle_signal(int signum){
    printf("E' stata uccisa la nave");
    exit(EXIT_SUCCESS);
}

int main(int argc, char** argv){
    /*DICHIARAZIONE DELLE VARIABILI*/
    int velocita = atoi(argv[1]);
    struct sigaction sa;
    double ordinata = atof(argv[2]), ascissa = atof(argv[3]), distanza;
    bzero(&sa, sizeof(sa));
    sa.sa_handler = handle_signal;
    sigaction(SIGINT,&sa,NULL);
    srand(time(NULL));

    /*ENTRA IN UN CICLO INFINITO PER ATTENDERE LA TERMINAZIONE DEL PADRE.
    VA POI MODIFICATO PER ESEGUIRE LE OPERAZIONI NECESSARIE.*/
    for(;;){}
    exit(0);
}