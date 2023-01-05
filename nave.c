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


/*int id_algo;*/
int n_algo = 0;
int capacita, velocita;
double xnave, ynave;
int id, id_dest = -1, id_merce, indirizzo_merce = -1, numero_porti_ricerca;
int sem_shmnave, sem_shmporto, sem_porto;
snave* shmnavi; sporto*shmporti; smerce* shmmerci; int* shmgiorno;
int SO_LOADSPEED, SO_PORTI, SO_CAPACITY, SO_STORM_DURATION, SO_SWELL_DURATION;
list lista_carico = NULL;
struct timespec rimanente;  /*SI USA NEL CASO IN CUI UNA NANOSLEEP VENGA BLOCCATA DA UN SEGNALE*/
struct timespec now;
int* array_porti;


/*FUNZIONI PER LA RIUSCITA DELLA SIMULAZIONE*/
void cerca_rotta();
void navigazione(double x, double y);
int cerca_richiesta();
void carica_offerta(int id_porto);
void portiOrdianti();
int numInserito(int n);
/*int closestPort();
int closestAvailablePort();
int algoritmoAleV1();*/

/*FUNZIONI PER LA GESTIONE DEI DISASTRI*/
void mareggiata(int signum);
void tempesta(int signum);

/*HANDLER PER GESTIRE IL SEGNALE DI TERMINAZIONE DEL PADRE*/
void handle_signal(int signum) {
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
    bzero(&now, sizeof(now));
    bzero(&rimanente, sizeof(rimanente));
    bzero(&sa, sizeof(sa));
    sa.sa_handler = handle_signal;
    sigaction(SIGINT, &sa, NULL);
    bzero(&sa, sizeof(sa));
    sa.sa_handler = mareggiata;
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
    SO_PORTI = atoi(argv[9]);
    shmgiorno = shmat(atoi(argv[10]), NULL, 0);
    shmmerci = shmat(atoi(argv[12]), NULL, 0);
    SO_LOADSPEED = atoi(argv[11]);
    sem_avvio = atoi(argv[14]);
    SO_STORM_DURATION = atoi(argv[15]);
    SO_SWELL_DURATION = atoi(argv[16]);
    if(SO_PORTI<=10)
        numero_porti_ricerca = SO_PORTI;
    else if(SO_PORTI <= 100)
        numero_porti_ricerca = 10;
    else if(SO_PORTI <= 1000)
        numero_porti_ricerca = 50;
    else
        numero_porti_ricerca = 100;     
    array_porti = calloc(numero_porti_ricerca, 4);
    TEST_ERROR;
    /*SEMAFORO PER AVVISARE IL PADRE MASTER CHE LA NAVE E' PRONTA*/
    sem_uscita(sem_avvio, 0);
    /*SEMAFORO CON CUI IL PADRE DA' IL VIA ALLA SIMULAZIONE*/
    sem_accesso(sem_avvio, 1);

    sem_accesso(sem_shmnave, id);
    xnave = shmnavi[id].x;
    ynave = shmnavi[id].y;
    sem_uscita(sem_shmnave, id);

    while (1) {
        cerca_rotta();
        lista_carico = list_controllo_scadenza(lista_carico, shmmerci, *shmgiorno, &capacita);
    }
    exit(0);
}

/*LA SEGUENTE FUNZIONE SERVE PER FAR SPOSTARE LA NAVE IN UN ALTRO PORTO.
    È STATA FATTA IN MODO CHE, DOPO UNA SOLA NANOSLEEP, LA NAVE ARRIVI A DESTINAZIONE.*/
void navigazione(double x, double y) {
    double dist;
    double tempo;
    struct timespec my_time;
    dist = sqrt(pow((y - ynave), 2) + pow((x - xnave), 2));
    tempo = dist / velocita;
    shmnavi[id].stato_nave = 1;
    my_time.tv_sec = (time_t)tempo;
    my_time.tv_nsec = (short)((tempo - (int)tempo) * 1000000000);
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
/*LA SEGUENTE E' UNA FUNZIONE CHE PERMETTE DI GESTIRE LO SPOSTAMENTO E LO SCARICO DELLE NAVI*/
void cerca_rotta() {
    int temp, i;
    struct timespec now;
    double tempo;
    int quantita = 0;
    id_dest = cerca_richiesta();
    navigazione(shmporti[id_dest].x, shmporti[id_dest].y);
    sem_accesso(sem_porto, id_dest);    /*siamo entrati in una banchina*/
    shmnavi[id].stato_nave = 0;
    if(shmnavi[id].carico_tot>0){
        quantita = list_sum_merce(lista_carico, shmmerci, shmporti[id_dest].richiesta.idmerce);
        if(quantita < shmporti[id_dest].richiesta.qmerce)
            tempo = (quantita * shmmerci[shmporti[id_dest].richiesta.idmerce].dimensione) / SO_LOADSPEED;
        else
            tempo = (shmporti[id_dest].richiesta.qmerce * shmmerci[shmporti[id_dest].richiesta.idmerce].dimensione) / SO_LOADSPEED;
        now.tv_sec = (time_t)tempo;
        now.tv_nsec = (long)(tempo - (int)tempo) * 10000;
        if (id_merce >= 0) {
            shmnavi[id].stato_nave = 2;
            sem_accesso(sem_shmporto, id_dest);
            sem_accesso(sem_shmnave, id);
            temp = shmporti[id_dest].richiesta.qmerce - list_sum_merce(lista_carico, shmmerci, shmporti[id_dest].richiesta.idmerce);
            lista_carico = list_rimuovi_richiesta(lista_carico, shmporti[id_dest].richiesta, shmporti, id_dest);
            capacita = SO_CAPACITY - list_sum(lista_carico, shmmerci);
            shmnavi[id].carico_tot = list_sum(lista_carico, shmmerci);
            if (temp < 0) {
                shmporti[id_dest].richiesta.qmerce = 0;
                shmporti[id_dest].richiesta_soddisfatta = 1;
            }
            sem_uscita(sem_shmnave, id);
            sem_uscita(sem_shmporto, id_dest);
            nanosleep(&now, &rimanente);
            bzero(&rimanente, sizeof(rimanente));
            shmnavi[id].stato_nave = 0;
        }
    }
    if (capacita > 0) {
        carica_offerta(id_dest);
    }
    sem_uscita(sem_porto, id_dest);
    shmnavi[id].stato_nave = 1;
}
/*LA SEGUENTE FUNZIONE PERMETTE DI GESTIRE IL CARICO DELL'OFFERTA DA PARTE DELLE NAVI*/
void carica_offerta(int id_porto) {
    carico c;
    struct timespec now;
    int temp = 0;
    double tempo;
    shmnavi[id].stato_nave = 3;
    sem_accesso(sem_shmporto, id_dest);
    tempo = (list_sum_merce(lista_carico, shmmerci, shmporti[id_dest].offerta.idmerce) * shmmerci[shmporti[id_dest].offerta.idmerce].dimensione) / SO_LOADSPEED;
    now.tv_sec = (time_t)tempo;
    now.tv_nsec = (long)(tempo - (int)tempo) * 10000;
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
    }

    sem_uscita(sem_shmporto, id_dest);
    shmnavi[id].carico_tot = SO_CAPACITY - capacita;
    nanosleep(&now, &rimanente);
    bzero(&rimanente, sizeof(rimanente));
    shmnavi[id].stato_nave = 0;
}
/*CON QUESTA FUNZIONE ANDIAMO A CERCARE LA DESTINAZIONE IN BASE AI PORTI PIU' VICINI ALLA NAVE E ALLE LORO RICHIESTE*/
int cerca_richiesta() {
    int i, id_temp;
    struct timespec now; 

    portiOrdianti();

    while (1) {
        for (i = 0; i < numero_porti_ricerca; i++) {
            id_temp = array_porti[i];
            if (shmporti[id_temp].richiesta_soddisfatta == 0 && list_sum_merce(lista_carico, shmmerci, shmporti[id_temp].richiesta.idmerce) > 0) {
                if(semctl(sem_porto, id_temp, GETVAL))
                    return id_temp;
            }
            /*if (shmporti[i].richiesta_soddisfatta == 0 && list_sum_merce(lista_carico, shmmerci, shmporti[i].richiesta.idmerce) > 0) {
                if(semctl(sem_porto, i, GETVAL))
                    return i;
            }*/
        }
        clock_gettime(CLOCK_REALTIME, &now);
        id_temp = now.tv_nsec % numero_porti_ricerca;
        return array_porti[id_temp];
    }
}
/*SIGUSR1*/
void mareggiata(int signum) {
    double t1 = (shmgiorno[1] * SO_SWELL_DURATION / 24.0);
    double t = (shmgiorno[1] * SO_SWELL_DURATION / 24);
    shmnavi[id].stato_nave = 5;
    now.tv_sec = rimanente.tv_sec + (time_t)t;
    now.tv_nsec = rimanente.tv_nsec + (long)((t1-t)*1000000000);
    nanosleep(&now, &rimanente);
    shmnavi[id].stato_nave = 1;
}
/*SIGUSR2*/
void tempesta(int signum) {
    struct timespec now;
    double t1 = (shmgiorno[1] * SO_STORM_DURATION / 24.0);
    double t = (shmgiorno[1] * SO_STORM_DURATION / 24);
    shmnavi[id].stato_nave = 4;
    now.tv_sec = rimanente.tv_sec + (time_t)t;
    now.tv_nsec = rimanente.tv_nsec + (long)((t1-t)*1000000000);
    nanosleep(&now, NULL);
    shmnavi[id].stato_nave = 1;
}
/*FUNZIONE CHE RITORNA GLI numero_porti_richiesta PIU' VICINI ALLA NAVE, ESCLUSO IL PORTO DI PARTENZA*/
void portiOrdianti(){
    int i, j, id_temp;
    double min = -1, distanza;
    for(i = 0; i<numero_porti_ricerca; i++){
        array_porti[i] = -1;
    }
    for(i = 0; i<numero_porti_ricerca; i++){
        for(j = 0; j<SO_PORTI; j++){
            distanza = dist(shmnavi[id].x, shmnavi[id].y, shmporti[j].x, shmporti[j].y);
            if(!numInserito(j) && (distanza < min || min == -1) && j != id_dest){
                min = distanza;
                id_temp = j;
            }
        }
        min = -1;
        array_porti[i] = id_temp;
    }
}

int numInserito(int n){
    int i;
    for(i = 0; i<SO_PORTI; i++){
        if(array_porti[i] == n)
            return 1;
    }
    return 0;
}
/*RITORNA ID PORTO MENO DISTANTE DA NAVE
int closestPort() {
    double min = dist(xnave, ynave, shmporti[0].x, shmporti[0].y); /*distanza min
    double d;
    int i = 1;
    int return_id = 0;
    for (; i < SO_PORTI; i++) {
        d = dist(xnave, shmporti[i].x, ynave, shmporti[i].y);
        if (id == 2)
            printf("pid:%d, id:%d, min:%f, d:%f\n", getpid(), return_id, min, d);
        if (d < min) {
            min = d;
            return_id = i;
        }
    }
    return return_id;
}

/*RITORNA ID PORTO MENO DISTANTE DA NAVE CON BANCHINE LIBERE
int closestAvailablePort() {
    double min;
    double d;
    int i = 0;
    int id = 0;

    for (; semctl(sem_porto, i, GETVAL) == 0 && i < SO_PORTI && dist(xnave, shmporti[i].x, ynave, shmporti[i].y) != 0; i++);
    id = i;

    for (min = dist(xnave, shmporti[i].x, ynave, shmporti[i].y); i < SO_PORTI; i++) {
        d = dist(xnave, shmporti[i].x, ynave, shmporti[i].y);
        if (d < min && semctl(sem_porto, i, GETVAL) > 0 && d != 0) {
            min = d;
            id = i;
        }
    }
    return id;
}

int algoritmoAleV001() {
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    n_algo++;
    if (n_algo % 2) {
        id_algo = now.tv_nsec % SO_PORTI;
    } else {
        id_algo = (id_algo + 1) % SO_PORTI;
    }
    return id_algo;
}

/*ALGORITMO SUPREMO
int algoritmoAleV1() {
    struct timespec now;
    int id_temp;
    clock_gettime(CLOCK_REALTIME, &now);
    n_algo++;
    if (n_algo % 2) {
        id_algo = now.tv_nsec % SO_PORTI;
    } else {
        id_algo = closestAvailablePort();
    }
    return id_algo;
}
*/