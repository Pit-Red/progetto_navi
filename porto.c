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
int sem_id;
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

void nuova_offerta(int signum);

void nuova_richiesta(int signum);


int main(int argc, char** argv) {
    /*DICHIARAZIONE DELLE VARIABILI*/
    /*DEFINIZIONE VAR CODA MEX*/
    int tmerce, qmerce; /*tmerce = tipo merce, qmerce = quantita merce*/
    size_t msgsize_user, msgesize_max;
    int status, num_bytes;
    struct sigaction sa;
    struct sigaction ca;
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
    sem_id = atoi(argv[1]);
    sem_porto = atoi(argv[5]);
    id = atoi(argv[4]);
    msg_richiesta = atoi(argv[6]);
    SO_MERCI = atoi(argv[7]);
    shmmerci = shmat(atoi(argv[8]),NULL,0);
    shmgiorno = shmat(atoi(argv[9]),NULL,0);


        sem_accesso(sem_id,0);
        shmporti[id].offerta = creazione_offerta();
        msg_invio(msg_richiesta, shmporti[id].offerta);
        TEST_ERROR;
        sem_uscita(sem_id,0);
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

carico creazione_offerta(){
    carico c;
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    c.idmerce = now.tv_nsec % SO_MERCI;
    c.pid = getpid();
    clock_gettime(CLOCK_REALTIME, &now);
    c.qmerce = now.tv_nsec % 100 +1;
    c.scadenza = shmmerci[c.idmerce].scadenza + *shmgiorno;
    return c;
}

carico creazione_richiesta(){
    carico c;
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    do{
        c.idmerce = now.tv_nsec % SO_MERCI;
    }while(c.idmerce == shmporti[id].offerta.idmerce);
    clock_gettime(CLOCK_REALTIME, &now);
    c.qmerce = now.tv_nsec % 100 +1;
    c.scadenza = shmmerci[c.idmerce].scadenza + *shmgiorno;
    return c; 
}

void nuova_offerta(int signum){
    sem_accesso(sem_id, 0);
    shmporti[id].offerta = creazione_offerta();
    sem_uscita(sem_id, 0);
}

void nuova_richiesta(int signum){
    msg_invio(msg_offerta, creazione_offerta());
}
