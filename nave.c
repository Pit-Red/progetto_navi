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
double xdest, ydest;
double xnave, ynave;
int id, id_dest = -1, id_merce, indirizzo_merce = -1;
int sem_shmnave, sem_shmporto;
int sem_porto;
int msg_richiesta;
int msg_offerta;
snave* shmnavi; sporto*shmporti; smerce* shmmerci; int* shmgiorno;
int SO_LOADSPEED, SO_PORTI, SO_CAPACITY, SO_STORM_DURATION;
list lista_carico = NULL;
struct timespec rimanente;

void cerca_rotta(carico c);

void navigazione(double x, double y);
void scadenza(int signum);
int cerca_richiesta();
void carica_offerta(int id_porto, double tempo);

int controllo(carico c);
int closestPort();
int closestAvailablePort();


void tempesta(int signum) {
    struct timespec now;
    int t = (shmgiorno[1] * SO_STORM_DURATION) / 24;
    shmnavi[id].stato_nave = 3;
    now.tv_sec = rimanente.tv_sec + (time_t)t;
    now.tv_nsec = rimanente.tv_nsec;
    printf("sono stata colpita da una tempesta [%d]\n\n", getpid());
    nanosleep(&now, NULL);
    TEST_ERROR;
    shmnavi[id].stato_nave = 1;
}

/*HANDLER PER GESTIRE IL SEGNALE DI TERMINAZIONE DEL PADRE*/
void handle_signal(int signum) {
    printf("\033[0;31m");
    printf("uccisa nave[%d]\n", id);
    printf("\033[0m");

    exit(EXIT_SUCCESS);
}


int main(int argc, char** argv) {
    /*DICHIARAZIONE DELLE VARIABILI*/
    int i;
    carico temp_merce;
    int sem_avvio;
    struct sigaction sa;
    struct sembuf sops;
    int temp;
    bzero(&sa, sizeof(sa));
    sa.sa_handler = handle_signal;
    sigaction(SIGINT, &sa, NULL);
    bzero(&sa, sizeof(sa));
    sa.sa_handler = scadenza;
    sigaction(SIGUSR1, &sa, NULL);
    bzero(&sa, sizeof(sa));
    sa.sa_handler = tempesta;
    sigaction(SIGUSR2, &sa, NULL);
    srand(time(NULL));
    capacita = atoi(argv[4]);
    SO_CAPACITY = atoi(argv[4]);
    velocita = atoi(argv[5]);
    shmporti = shmat(atoi(argv[2]), NULL, 0);
    shmnavi = shmat(atoi(argv[3]), NULL, 0);
    sem_shmnave = atoi(argv[13]);
    sem_porto = atoi(argv[7]);
    sem_shmporto = atoi(argv[1]);
    id = atoi(argv[6]);
    msg_richiesta = atoi(argv[8]);
    SO_PORTI = atoi(argv[9]);
    shmgiorno = shmat(atoi(argv[10]), NULL, 0);
    shmmerci = shmat(atoi(argv[12]), NULL, 0);
    SO_LOADSPEED = atoi(argv[11]);
    sem_avvio = atoi(argv[14]);
    SO_STORM_DURATION = atoi(argv[15]);
    TEST_ERROR;



    sem_accesso(sem_shmnave, id);
    xnave = shmnavi[id].x;
    ynave = shmnavi[id].y;
    sem_uscita(sem_shmnave, id);

    while (1) {
        cerca_rotta(temp_merce);
        lista_carico = list_controllo_scadenza(lista_carico, shmmerci, *shmgiorno, &capacita);
    }
    exit(0);
}

/*LA SEGUENTE FUNZIONE SERVE PER FAR SPOSTARE LA NAVE IN UN ALTRO PORTO.
    È STATA FATTA IN MODO CHE, DOPO UNA SOLA NANOSLEEP, LA NAVE ARRIVI A DESTINAZIONE.
    BISOGNA POI AGGIUNGERE L'IDENTIFICATORE DI NAVE IN PORTO\NAVE IN MARE*/
void navigazione(double x, double y) {
    double dist;
    double tempo;
    struct timespec my_time;
    dist = sqrt(pow((y - ynave), 2) + pow((x - xnave), 2));
    tempo = dist / velocita;
    shmnavi[id].stato_nave = 1;
    my_time.tv_sec = (time_t)tempo;
    my_time.tv_nsec = (short)((tempo - (int)tempo) * 10000);
    /*my_time.tv_nsec = 0;*/
    nanosleep(&my_time, &rimanente);
    bzero(&rimanente, sizeof(rimanente));
    if (errno = 4)
        errno = 0;
    TEST_ERROR;
    sem_accesso(sem_shmnave, id);
    shmnavi[id].x = x;
    shmnavi[id].y = y;
    sem_uscita(sem_shmnave, id);
}

void cerca_rotta(carico c) {
    int temp;
    struct timespec now;
    double tempo;
    id_dest = cerca_richiesta();
    navigazione(shmporti[id_dest].x, shmporti[id_dest].y);
    sem_accesso(sem_porto, id_dest);    /*siamo entrati in una banchina*/
    shmnavi[id].stato_nave = 0;
    tempo = (list_sum_merce(lista_carico, shmmerci, shmporti[id_dest].richiesta.idmerce) * shmmerci[shmporti[id_dest].richiesta.idmerce].dimensione) / SO_LOADSPEED;
    now.tv_sec = (time_t)tempo;
    now.tv_nsec = (long)(tempo - (int)tempo) * 10000;
    shmnavi[id].stato_nave = 2;
    if (id_merce >= 0) { 
        nanosleep(&now, NULL);
        sem_accesso(sem_shmporto, id_dest);
        sem_accesso(sem_shmnave, id);
        temp = shmporti[id_dest].richiesta.qmerce - list_sum_merce(lista_carico, shmmerci, shmporti[id_dest].richiesta.idmerce);
        lista_carico = list_rimuovi_richiesta(lista_carico, shmporti[id_dest].richiesta);
        capacita = SO_CAPACITY - list_sum(lista_carico, shmmerci);
        shmnavi[id].carico_tot = list_sum(lista_carico, shmmerci);
        if (temp < 0) {
            shmporti[id_dest].richiesta.qmerce = 0;
            shmporti[id_dest].richiesta_soddisfatta = 1;
        }
        else
            shmporti[id_dest].richiesta.qmerce = temp;
        sem_uscita(sem_shmnave, id);
        sem_uscita(sem_shmporto, id_dest);
    }
    if (capacita > 0) {
        carica_offerta(id_dest, tempo);
    }
    shmnavi[id].stato_nave = 0;
    sem_uscita(sem_porto, id_dest);
}

void carica_offerta(int id_porto, double tempo) {
    carico c;
    struct timespec now;
    int temp = 0;
    now.tv_sec = (time_t)tempo;
    now.tv_nsec = (long)(tempo - (int)tempo) * 10000;
    shmnavi[id].stato_nave = 3;
    nanosleep(&now, &rimanente);
    bzero(&rimanente, sizeof(rimanente));
    sem_accesso(sem_shmporto, id_dest);
    c.idmerce = shmporti[id_dest].offerta.idmerce;
    c.qmerce = shmporti[id_dest].offerta.qmerce;
    c.pid = shmporti[id_dest].offerta.pid;
    c.scadenza = shmporti[id_dest].offerta.scadenza;
    if ((capacita - (c.qmerce * shmmerci[c.idmerce].dimensione)) < 0) {
        /*SE NON C'E' ABBASTANZA SPAZIO SULLA NAVE*/
        c.qmerce = capacita / shmmerci[c.idmerce].dimensione;
        lista_carico = list_insert_head(lista_carico, c);
        shmporti[id_dest].offerta.qmerce -= c.qmerce;
        capacita = 0;
    }
    else {
        /*SE C'E' ABBASTANZA SPAZIO SULLA NAVE*/
        lista_carico = list_insert_head(lista_carico, c);
        capacita -= c.qmerce * shmmerci[c.idmerce].dimensione;
        shmporti[id_dest].offerta.qmerce = 0;
        if (capacita < 0) {
            printf("diocaneeeeeee\n\n");
        }
    }
    shmnavi[id].carico_tot = SO_CAPACITY - capacita;
    shmnavi[id].stato_nave = 0;
    sem_uscita(sem_shmporto, id_dest);
}


int cerca_richiesta() {
    int i;
    struct timespec now;
    while (1) {
        for (i = 0; i < SO_PORTI; i++) {
            if (shmporti[i].richiesta_soddisfatta == 0 && list_sum_merce(lista_carico, shmmerci, shmporti[i].richiesta.idmerce) > 0) {
                return i;
            }
        }
        clock_gettime(CLOCK_REALTIME, &now);
        return now.tv_nsec % SO_PORTI;
        /*return closestAvailablePort();*/
    }
}

void scadenza(int signum) {
    if (lista_carico != NULL)
        lista_carico = list_controllo_scadenza(lista_carico, shmmerci, *shmgiorno, &capacita);
}

/*RITORNA ID PORTO MENO DISTANTE DA NAVE*/
int closestPort() {
    double min = dist(xnave, ynave, shmporti[0].x, shmporti[0].y); /*distanza min*/
    double d;
    int i = 1;
    int return_id = 0;
    for (; i < SO_PORTI; i++) {
        d = dist(xnave, shmporti[i].x, ynave, shmporti[i].y);
        if(id==2)
            printf("pid:%d, id:%d, min:%f, d:%f\n",getpid(), return_id, min, d);
        if (d < min) {
            min = d;
            return_id = i;
        }
    }
    return return_id;
}

/*RITORNA ID PORTO MENO DISTANTE DA NAVE CON BANCHINE LIBERE*/
int closestAvailablePort() {
    double min;
    double d;
    int i = 0;
    int id = 0;

    for (; semctl(sem_porto, i, GETVAL) == 0 && i < SO_PORTI; i++);
    id = i;

    for (; i < SO_PORTI; i++) {
        d = dist(xnave, shmporti[i].x, ynave, shmporti[i].y);
        if (d < min && semctl(sem_porto, i, GETVAL) > 0) {
            min = d;
            id = i;
        }
    }
    return id;
}
