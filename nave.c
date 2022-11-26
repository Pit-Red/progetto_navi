#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>



int capacita, velocita;
double ord,asc;

/*HANDLER PER GESTIRE IL SEGNALE DI TERMINAZIONE DEL PADRE*/
void handle_signal(int signum){
    printf("uccisa nave [%d]\n", getpid());
    exit(EXIT_SUCCESS);
}

int main(int argc, char** argv){
    /*DICHIARAZIONE DELLE VARIABILI*/
    struct sigaction sa;
    bzero(&sa, sizeof(sa));
    sa.sa_handler = handle_signal;
    sigaction(SIGINT,&sa,NULL);
    srand(time(NULL));
    capacita = atoi(argv[1]);
    velocita = atoi(argv[2]);
    ord = atoi(argv[3]);
    asc = atoi(argv[4]);
    
    printf("E' stata creata la nave, l'id della memoria condivisa e': %d\n", atoi(argv[5]));
    /*ENTRA IN UN CICLO INFINITO PER ATTENDERE LA TERMINAZIONE DEL PADRE.
    VA POI MODIFICATO PER ESEGUIRE LE OPERAZIONI NECESSARIE.*/
    for(;;){}
    exit(0);
}

/*LA SEGUENTE FUNZIONE SERVE PER FAR SPOSTARE LA NAVE IN UN ALTRO PORTO.
    È STATA FATTA IN MODO CHE, DOPO UNA SOLA NANOSLEEP, LA NAVE ARRIVI A DESTINAZIONE.
    BISOGNA POI AGGIUNGERE L'IDENTIFICATORE DI NAVE IN PORTO\NAVE IN MARE*/
void navigazione(double x, double y){
    double dist;
    double tempo;
    struct timespec my_time;
    dist = sqrt(pow((y-ord),2)+pow((x-asc),2));
    tempo = dist/velocita;
    my_time.tv_sec = (int)tempo;
    my_time.tv_nsec = tempo-(int)tempo;
    nanosleep(&my_time, NULL);
    ord = y;
    asc = x;
}
