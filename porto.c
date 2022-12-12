#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <math.h>
#include "utilities.h"
#define REQUEST_MAX_SIZE 128



struct sembuf my_op;
int sem_porto;
int id;
int sem_shmporto, sem_shmnave;
int msg_richiesta;
int msg_offerta;
sporto* shmporti; smerce* shmmerci;
int* shmgiorno;
int SO_MERCI;

/*HANDLER PER GESTIRE IL SEGNAÒLE DI TERMINAZIONE DEL PADRE*/
void handle_signal(int signum) {
    printf("\033[0;31m");
    printf("ucciso porto[%d]\n", getpid());
    printf("\033[0m");
    exit(0);
}

carico creazione_offerta();

carico creazione_richiesta();

void creazione_random();

void nuova_offerta(int signum);

void nuova_richiesta(int signum);


int main(int argc, char** argv) {
    /*DICHIARAZIONE DELLE VARIABILI*/
    /*DEFINIZIONE VAR CODA MEX*/
    int tmerce, qmerce; /*tmerce = tipo merce, qmerce = quantita merce*/
    int sem_avvio;
    size_t msgsize_user, msgesize_max;
    int status, num_bytes;
    struct sembuf sops;
    struct sigaction sa;
    struct timespec now;
    bzero(&sa, sizeof(sa));
    sa.sa_handler = handle_signal;
    sigaction(SIGINT, &sa, NULL);
    bzero(&sa, sizeof(sa));
    sa.sa_handler = nuova_offerta;
    sigaction(SIGUSR1, &sa, NULL);
    bzero(&sa, sizeof(sa));
    sa.sa_handler = nuova_richiesta;
    sigaction(SIGUSR2, &sa, NULL);
    TEST_ERROR;
    shmporti = shmat(atoi(argv[2]), NULL, 0);
    sem_shmporto = atoi(argv[1]);
    sem_shmnave = atoi(argv[10]);
    sem_porto = atoi(argv[5]);
    id = atoi(argv[4]);
    msg_richiesta = atoi(argv[6]);
    SO_MERCI = atoi(argv[7]);
    shmmerci = shmat(atoi(argv[8]), NULL, 0);
    shmgiorno = shmat(atoi(argv[9]), NULL, 0);
    sem_avvio = atoi(argv[11]);




    sem_accesso(sem_shmporto , id);
    shmporti[id].offerta = creazione_offerta();
    msg_invio(msg_richiesta, shmporti[id].offerta);
    TEST_ERROR;
    sem_uscita(sem_shmporto, id);
    TEST_ERROR;

    /*num_bytes = sprintf(mybuf.mtext,"porto[%5d]: %dx%d\n", getpid(), tmerce, qmerce);
    num_bytes++;
    mybuf.mtype = 1;


    msg_send(q_id, &mybuf, num_bytes);*/



    /*ENTRA IN UN CICLO INFINITO PER ATTENDERE LA TERMINAZIONE DEL PADRE.
    VA POI MODIFICATO PER ESEGUIRE LE OPERAZIONI NECESSARIE.*/
    for (;;) {}
    exit(0);
}

carico creazione_offerta(int qmerce) {
    carico c;
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    c.idmerce = now.tv_nsec % SO_MERCI;
    c.pid = getpid();
    clock_gettime(CLOCK_REALTIME, &now);
    c.qmerce = qmerce;
    c.scadenza = shmmerci[c.idmerce].scadenza + *shmgiorno;
    return c;
}

carico creazione_richiesta(int qmerce) {
    carico c;
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    do {
        c.idmerce = now.tv_nsec % SO_MERCI;
    } while (c.idmerce == shmporti[id].offerta.idmerce);
    clock_gettime(CLOCK_REALTIME, &now);
    c.qmerce = qmerce;
    c.scadenza = shmmerci[c.idmerce].scadenza + *shmgiorno;
    return c;
}

void nuova_offerta(int signum) {    /*SIGUSR1*/
    sem_accesso(sem_shmporto, id);
    shmporti[id].offerta = creazione_offerta();
    sem_uscita(sem_shmporto, id);
}

void nuova_richiesta(int signum) {  /*SIGUSR2*/
    msg_invio(msg_richiesta, creazione_offerta());
}


void creazione_random() {
    /*shmfill[0] = MEDIA;
    shmfill[1] = RANGE;
    shmfill[2] = SO_FILL OFFERTA;
    shmfill[3] = SO_FILL RICH;
    shmfill[4] = SO_PORTI;*/
    int offerta, richiesta;
    struct timespec now;

    if (idshmfill[4] != 1) {
        /*gen offerta*/
        clock_gettime(CLOCK_REALTIME, &now);
        offerta = now.tv_nsec % shmfill[1];
        shmfill[2] -= offerta;
        creazione_offerta(offerta);

        /*gen richiesta*/
        clock_gettime(CLOCK_REALTIME, &now);
        richiesta = now.tv_nsec % shmfill[1];
        shmfill[3] -= richiesta;
        creazione_richiesta(richiesta);


        idshmfill[4]--;
    } else {
        offerta = shmfill[2];
        richiesta = shmfill[3];
        creazione_offerta(offerta);
        creazione_richiesta(richiesta);


    }

}