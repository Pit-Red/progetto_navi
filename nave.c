#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <math.h>
#include <sys/sem.h>
#include "utilities.h"


int capacita, velocita;
double xdest,ydest;
double xnave,ynave;
int id;
int sem_id;
int sem_porto;
int msg_richiesta;
int msg_offerta;
snave* shmnavi; sporto*shmporti;



void navigazione(double x, double y);

/*HANDLER PER GESTIRE IL SEGNALE DI TERMINAZIONE DEL PADRE*/
void handle_signal(int signum){
    printf("\033[0;31m");
    printf("uccisa nave[%d]\n", getpid());
    printf("\033[0m");

    exit(EXIT_SUCCESS);
}


int main(int argc, char** argv){
    /*DICHIARAZIONE DELLE VARIABILI*/
    carico temp_merce;
    struct sigaction sa;
    int temp;
    bzero(&sa, sizeof(sa));
    sa.sa_handler = handle_signal;
    sigaction(SIGINT,&sa,NULL);
    srand(time(NULL));
    capacita = atoi(argv[4]);
    velocita = atoi(argv[5]);
    shmporti = shmat(atoi(argv[2]), NULL, 0);
    shmnavi = shmat(atoi(argv[3]), NULL, 0);
    sem_id = atoi(argv[1]);
    sem_porto = atoi(argv[7]);
    id = atoi(argv[6]);
    msg_richiesta = atoi(argv[8]);
    msg_offerta = atoi(argv[9]);
    TEST_ERROR;

    sem_accesso(sem_id,1);/*sem[0]=>shmporti, sem[1]=>shmnavi*/
    xnave = shmnavi[id].x;
    ynave = shmnavi[id].y;
    sem_uscita(sem_id,1);

    /*prova carico*/
    if(id==0){
        temp_merce.pid = getpid();
        temp_merce.idmerce = 0;
        temp_merce.qmerce = 10;
        temp_merce.scadenza = 19;
        sem_accesso(sem_id,1);
        shmnavi[id].lista_merci = list_insert_head(shmnavi[id].lista_merci, temp_merce);
        sem_uscita(sem_id,1);
    }



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
    dist = sqrt(pow((y-ynave),2)+pow((x-xnave),2));
    tempo = dist/velocita;
    my_time.tv_sec = (int)tempo;
    my_time.tv_nsec = (tempo-(int)tempo) * 10000;
    nanosleep(&my_time, NULL);
    sem_accesso(sem_id,1);
    shmnavi[id].x = x;
    shmnavi[id].y = y;
    sem_uscita(sem_id,1);
}
