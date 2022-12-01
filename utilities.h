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

typedef struct {
    pid_t pid;
    double x;
    double y;
    smerce* carico;
}snave;

typedef struct {
    pid_t pid;
    double x;
    double y;
}sporto;


typedef struct{
    int quantita;
    int tempo_scadenza;
}smerce;

#define TEST_ERROR    if (errno) {fprintf(stderr,           \
                      "%s:%d: PID=%5d: Error %d (%s)\n", \
                      __FILE__,         \
                      __LINE__,         \
                      getpid(),         \
                      errno,            \
                      strerror(errno));}

void sem_accesso(int semid,int num_risorsa);

void sem_uscita(int semid,int num_risorsa);
