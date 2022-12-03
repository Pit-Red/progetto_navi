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
#include <sys/sem.h>
#include <math.h>
#define MSG_LEN 128

typedef struct {
    int quantita;
    int dimensione;
    int tempo_scadenza;
} smerce;

typedef struct {
    pid_t pid;
    double x;
    double y;
    smerce* carico;
    int carico_tot;
} snave;

typedef struct {
    pid_t pid;
    double x;
    double y;
} sporto;

/*struct requestbuf {
    long rtype;           
    char rtext[MSG_LEN];    
};*/

typedef struct {
    long mtype;             /*MSG TYPE (n>0)*/
    char mtext[MSG_LEN];            /*MSG DATA*/
}msg;



#define TEST_ERROR    if (errno) {fprintf(stderr,           \
                      "%s:%d: PID=%5d: Error %d (%s)\n", \
                      __FILE__,         \
                      __LINE__,         \
                      getpid(),         \
                      errno,            \
                      strerror(errno));}

void sem_accesso(int semid, int num_risorsa);

void sem_uscita(int semid, int num_risorsa);

void stampa_merci(smerce* temp_merci);

msg create_demand(int idporto, int idmerce, int qmerce);

int msg_send(int queue, const msg* my_msg , size_t msg_length);

void msg_print_stats(int fd, int q_id);

void print_msg(msg my_msg); /*TODO*/

