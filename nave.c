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
int sem_shmnave, sem_shmporto;
int sem_porto;
int msg_richiesta;
int msg_offerta;
snave* shmnavi; sporto*shmporti;smerce* shmmerci; int* shmgiorno;
int SO_LOADSPEED;
list lista_carico = NULL;

void cerca_rotta(carico c);

void navigazione(double x, double y);
void scadenza(int signum);

/*HANDLER PER GESTIRE IL SEGNALE DI TERMINAZIONE DEL PADRE*/
void handle_signal(int signum){
    printf("\033[0;31m");
    printf("uccisa nave[%d]\n", getpid());
    printf("\033[0m");

    exit(EXIT_SUCCESS);
}


int main(int argc, char** argv){
    /*DICHIARAZIONE DELLE VARIABILI*/
    int i;
    carico temp_merce;
    int sem_avvio;
    struct sigaction sa;
    struct sembuf sops;
    int temp;
    bzero(&sa, sizeof(sa));
    sa.sa_handler = handle_signal;
    sigaction(SIGINT,&sa,NULL);
    bzero(&sa, sizeof(sa));
    sa.sa_handler = scadenza;
    sigaction(SIGUSR1,&sa,NULL);
    srand(time(NULL));
    capacita = atoi(argv[4]);
    velocita = atoi(argv[5]);
    shmporti = shmat(atoi(argv[2]), NULL, 0);
    shmnavi = shmat(atoi(argv[3]), NULL, 0);
    sem_shmnave = atoi(argv[13]);
    sem_porto = atoi(argv[7]);
    sem_shmporto = atoi(argv[1]);
    id = atoi(argv[6]);
    msg_richiesta = atoi(argv[8]);
    msg_offerta = atoi(argv[9]);
    shmgiorno = shmat(atoi(argv[10]),NULL,0);
    shmmerci = shmat(atoi(argv[12]),NULL,0);
    SO_LOADSPEED = atoi(argv[11]);
    sem_avvio = atoi(argv[14]);
    TEST_ERROR;

    
    
    sem_accesso(sem_shmnave,id);
    xnave = shmnavi[id].x;
    ynave = shmnavi[id].y;
    sem_uscita(sem_shmnave,id);

    while(1){
        cerca_rotta(temp_merce);
    }


    

    /*prova carico*/



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
    shmnavi[id].stato_nave = 1;
    my_time.tv_sec = (time_t)tempo;
    my_time.tv_nsec = (short)((tempo-(int)tempo) * 10000);
    /*my_time.tv_nsec = 0;*/
    nanosleep(&my_time, NULL);
    sem_accesso(sem_shmnave, id);
    shmnavi[id].x = x;
    shmnavi[id].y = y;
    sem_uscita(sem_shmnave, id);
    shmnavi[id].stato_nave = 0;
}

void cerca_rotta(carico c){
    int differenza;
    int id_porto;
    msg_lettura(msg_richiesta, &c);
    if(differenza = c.qmerce - list_sum_merce(lista_carico, shmmerci, c.idmerce)){   /*la nave non contiene merci*/
        id_porto = pid_to_id_porto(c.pid, shmporti);
        navigazione(shmporti[id_porto].x, shmporti[id_porto].y);      /*arriviamo al porto*/
        sem_accesso(sem_porto, id_porto);              /*siamo entrati in una banchina*/
        shmnavi[id].stato_nave = 0;
        sem_accesso(sem_shmporto, id_porto);                         /*accediamo alla shmporto*/
        lista_carico = carico_nave(shmporti[id_porto].offerta, lista_carico, SO_LOADSPEED, shmmerci, shmnavi[id]);
        lista_carico = list_controllo_scadenza(lista_carico, shmmerci, *shmgiorno);
        TEST_ERROR;
        shmnavi[id].stato_nave = 0;
        /*bisogna mandare un segnale al porto per dirgli di aggiornare la sua offerta*/
        msg_invio(msg_richiesta, c);               /*rimando la richiesta in coda in quanto non potevo soddisfare la richiesta*/
        bzero(&c, sizeof(c));    /*azzero temp_merci*/
        sem_uscita(sem_shmporto, id_porto);  
        sem_uscita(sem_porto, id_porto);
        sem_accesso(sem_shmnave, id);
        shmnavi[id].carico_tot = list_sum(lista_carico, shmmerci);
        shmnavi[id].stato_nave = 1;
        sem_uscita(sem_shmnave, id);
        TEST_ERROR;
    }
    else{
        id_porto = pid_to_id_porto(c.pid, shmporti);
        navigazione(shmporti[id_porto].x, shmporti[id_porto].y);      /*arriviamo al porto*/
        sem_accesso(sem_porto, id_porto);              /*siamo entrati in una banchina*/
        shmnavi[id].stato_nave = 0;
        sem_accesso(sem_shmporto, id_porto);                         /*accediamo alla shmporto*/
        lista_carico = list_rimuovi_richiesta(lista_carico, c);
        lista_carico = list_controllo_scadenza(lista_carico, shmmerci, *shmgiorno);
        TEST_ERROR;
        /*kill(c.pid, SIGUSR2);*/       
        bzero(&c, sizeof(c));    /*azzero temp_merci*/
        sem_uscita(sem_shmporto, id_porto);  
        sem_uscita(sem_porto, id_porto);
        sem_accesso(sem_shmnave, id);
        shmnavi[id].carico_tot = list_sum(lista_carico, shmmerci);
        shmnavi[id].stato_nave = 1;
        sem_uscita(sem_shmnave, id);
        TEST_ERROR;
    }
}

void scadenza(int signum){
    if(lista_carico!=NULL)
        lista_carico = list_controllo_scadenza(lista_carico, shmmerci, *shmgiorno);
}