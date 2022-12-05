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
sporto* shmporti;

/*HANDLER PER GESTIRE IL SEGNAÒLE DI TERMINAZIONE DEL PADRE*/
void handle_signal(int signum) {
    printf("\033[0;31m");
    printf("ucciso porto[%d]\n", getpid());
    printf("\033[0m");
    exit(0);
}




/*int message_send(int queue, const message* my_message , size_t message_length);*/


int main(int argc, char** argv) {
    /*DICHIARAZIONE DELLE VARIABILI*/
    /*DEFINIZIONE VAR CODA MEX*/
    argomento_coda r;
    int tmerce, qmerce; /*tmerce = tipo merce, qmerce = quantita merce*/
    size_t msgsize_user, msgesize_max;
    int status, num_bytes;
    /*msg mybuf;*/
    FILE * in_stream;
    struct msqid_ds my_queue_stat;
    pid_t snd_pid;
    pid_t rcv_pid;
    struct sigaction sa;
    struct timespec now;
    bzero(&sa, sizeof(sa));
    sa.sa_handler = handle_signal;
    sigaction(SIGINT, &sa, NULL);
    TEST_ERROR;
    shmporti = shmat(atoi(argv[2]), NULL, 0);
    sem_id = atoi(argv[1]);
    sem_porto = atoi(argv[5]);
    id = atoi(argv[4]);
    msg_richiesta = atoi(argv[6]);
    msg_offerta = atoi(argv[7]);
    

    /*num_bytes = sprintf(mybuf.mtext,"porto[%5d]: %dx%d\n", getpid(), tmerce, qmerce);
    num_bytes++;
    mybuf.mtype = 1; 


    msg_send(q_id, &mybuf, num_bytes);*/


    printf("porto[%d] \n\n\n", getpid());
    if(id == 0){
        r.rtype = 1;
        r.idporto = id;
        r.idmerce = 2;
        r.qmerce = 10;

        msg_invio(msg_richiesta, r);
    }

    /*ENTRA IN UN CICLO INFINITO PER ATTENDERE LA TERMINAZIONE DEL PADRE.
    VA POI MODIFICATO PER ESEGUIRE LE OPERAZIONI NECESSARIE.*/
    for (;;) {}
    exit(0);
}



