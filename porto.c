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
int q_id;
sporto* shmporti;

/*HANDLER PER GESTIRE IL SEGNAÒLE DI TERMINAZIONE DEL PADRE*/
void handle_signal(int signum){
    printf("\033[0;31m");
    printf("ucciso porto[%d]\n", getpid());
    printf("\033[0m");
    exit(0);
}

struct my_request{
    long rtype;
    int idporto;            /*REQUEST DATA*/
    int idmerce;
    int qmerce;
};

my_request create_request(int idporto, int idmerce, int qmerce){
    //
}

int request_send(int queue, const struct my_request*, size_t request_length);

static void request_print_stats(int fd, int q_id);
    
int main(int argc, char** argv){
    /*DICHIARAZIONE DELLE VARIABILI*/
    struct sigaction sa;
    struct timespec now;
    bzero(&sa,sizeof(sa));
    sa.sa_handler = handle_signal;
    sigaction(SIGINT,&sa,NULL);
    TEST_ERROR;
    shmporti = shmat(atoi(argv[2]), NULL, 0);
    /*DEFINIZIONE DEL NUMERO DI BANCHINE*/
    sem_id = atoi(argv[1]);
    sem_porto = atoi(argv[5]);
    id = atoi(argv[4]);
    q_id = atoi(argv[6]);
    /*DEFINIZIONE VAR CODA MEX*/
    size_t requestsize_user, requestsize_max;
    int status, num_bytes;
    struct my_request mybuf;
    FILE * in_stream;
    struct msqid_ds my_queue_stat;
    pid_t snd_pid;
    pid_t rcv_pid;
    /*CREAZIONE QUEUE*/
    q_id = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | 0600);
    TEST_ERROR;




    printf("il porto %d \n\n\n\n\n", getpid());
    /*ENTRA IN UN CICLO INFINITO PER ATTENDERE LA TERMINAZIONE DEL PADRE.
    VA POI MODIFICATO PER ESEGUIRE LE OPERAZIONI NECESSARIE.*/
    for(;;){}
    exit(0);
}

int request_send(int queue, const struct my_request* my_requestbuf, size_t request_length){

    msgsnd(queue, my_requestbuf, request_length, 0);

    switch (errno) {
    case EAGAIN:
        dprintf(2,
            "Queue is full and IPC_NOWAIT was set to have a non-blocking msgsnd()\n\
Fix it by:\n                                \
  (1) making sure that some process read messages, or\n         \
  (2) changing the queue size by msgctl()\n");
        return(-1);
    case EACCES:
        dprintf(2,
        "No write permission to the queue.\n\
Fix it by adding permissions properly\n");
        return(-1);
    case EFAULT:
        dprintf(2,
            "The address of the message isn't accessible\n");
        return(-1);
    case EIDRM:
        dprintf(2,
            "The queue was removed\n");
        return(-1);
    case EINTR:
        dprintf(2,
            "The process got unblocked by a signal, while waiting on a full queue\n");
        return(-1);
    case EINVAL:
        TEST_ERROR;
        return(-1);
    case ENOMEM:
        TEST_ERROR;
        return(-1);
    default:
        TEST_ERROR;
    }
    return(0);
}

static void request_print_stats(int fd, int q_id){
    struct msqid_ds my_q_data;
    int ret_val;
    
    while (ret_val = msgctl(q_id, IPC_STAT, &my_q_data)) {
        TEST_ERROR;
    }
    dprintf(fd, "--- IPC Message Queue ID: %8d, START ---\n", q_id);
    dprintf(fd, "---------------------- Time of last msgsnd: %ld\n",
        my_q_data.msg_stime);
    dprintf(fd, "---------------------- Time of last msgrcv: %ld\n",
        my_q_data.msg_rtime);
    dprintf(fd, "---------------------- Time of last change: %ld\n",
        my_q_data.msg_ctime);
    dprintf(fd, "---------- Number of messages in the queue: %ld\n",
        my_q_data.msg_qnum);
    dprintf(fd, "- Max number of bytes allowed in the queue: %ld\n",
        my_q_data.msg_qbytes);
    dprintf(fd, "----------------------- PID of last msgsnd: %d\n",
        my_q_data.msg_lspid);
    dprintf(fd, "----------------------- PID of last msgrcv: %d\n",
        my_q_data.msg_lrpid);  
    dprintf(fd, "--- IPC Message Queue ID: %8d, END -----\n", q_id);
}