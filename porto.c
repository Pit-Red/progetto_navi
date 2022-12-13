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
int* shmfill;
int sem_fill;
int id_merce_richiesta;

/*HANDLER PER GESTIRE IL SEGNAÒLE DI TERMINAZIONE DEL PADRE*/
void handle_signal(int signum) {
    printf("\033[0;31m");
    printf("ucciso porto[%d]\n", getpid());
    printf("\033[0m");
    exit(0);
}

void nuova_richiesta();

void nuova_offerta();

carico creazione_offerta(int qmerce);

carico creazione_richiesta(int qmerce);

void creazione_random();

void nuova_offerta_handler(int signum);



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
    sa.sa_handler = nuova_offerta_handler;
    sigaction(SIGUSR1, &sa, NULL);
    /*bzero(&sa, sizeof(sa));
    sa.sa_handler = nuova_richiesta;
    sigaction(SIGUSR2, &sa, NULL);*/
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
    shmfill = shmat(atoi(argv[12]), NULL, 0);
    sem_fill = atoi(argv[13]);

    nuova_richiesta();
    nuova_offerta();


    my_op.sem_num = 1;
    my_op.sem_op = -1;
    semop(sem_avvio, &my_op,1);



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
    do{
        clock_gettime(CLOCK_REALTIME, &now);
        c.idmerce = (now.tv_nsec % (SO_MERCI*10000))/10000;
    }while(c.idmerce == id_merce_richiesta);
    c.pid = getpid();
    c.qmerce = qmerce;
    c.scadenza = shmmerci[c.idmerce].scadenza + *shmgiorno;
    return c;
}

carico creazione_richiesta(int qmerce) {
    carico c;
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    c.pid = getpid();
    c.idmerce = (now.tv_nsec % (SO_MERCI*10000))/10000;
    id_merce_richiesta = c.idmerce;
    c.qmerce = qmerce;
    c.scadenza = shmmerci[c.idmerce].scadenza + *shmgiorno;
    return c;
}

void nuova_offerta_handler(int signum) {    /*SIGUSR1*/
    nuova_offerta();
}



void nuova_offerta(){
    int offerta;
    struct timespec now;
    int totale = 0;
    sem_accesso(sem_fill,1);
    totale = shmfill[4];
    if (totale >0) {
        /*gen offerta*/
        clock_gettime(CLOCK_REALTIME, &now);
        offerta = shmfill[0] + (now.tv_nsec % (shmfill[1] * 2)) - shmfill[1];
        shmfill[2] -= offerta;
        clock_gettime(CLOCK_REALTIME, &now);
        shmporti[id].offerta = creazione_offerta(offerta);
    }
    shmfill[4]--;
    sem_uscita(sem_fill,1);
}

void nuova_richiesta(){
    int richiesta;
    struct timespec now;
    int totale = 0;
    sem_accesso(sem_fill,0);
    totale = shmfill[5];
    if (totale >0) {
        /*g        sem_uscita(sem_fill, 0);en richiesta*/
        clock_gettime(CLOCK_REALTIME, &now);
        richiesta = shmfill[0] + (now.tv_nsec % (shmfill[1] * 2)) - shmfill[1];
        shmfill[3] -= richiesta;
        msg_invio(msg_richiesta ,creazione_richiesta(richiesta));
        TEST_ERROR;
    }
    shmfill[5]--;
    sem_uscita(sem_fill,0);

}

