#define GNU_SOURCE
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
#include <math.h>

#define TEST_ERROR    if (errno) {fprintf(stderr,			\
					  "%s:%d: PID=%5d: Error %d (%s)\n", \
					  __FILE__,			\
					  __LINE__,			\
					  getpid(),			\
					  errno,			\
					  strerror(errno));}

void reset_sem(int sem_id);/*resetto tutti i semafori*/

void sem_accesso(int semid,int num_risorsa);

void sem_uscita(int semid,int num_risorsa);

struct sembuf my_op;
int SO_BANCHINE;
int sem_id;

/*HANDLER PER GESTIRE IL SEGNAÒLE DI TERMINAZIONE DEL PADRE*/
void handle_signal(int signum){
    printf("\033[0;31m");
    printf("ucciso porto[%d]\n", getpid());
    printf("\033[0m");
    exit(0);
}

    
int main(int argc, char** argv){
    /*DICHIARAZIONE DELLE VARIABILI*/
    struct sigaction sa;
    struct timespec now;
    bzero(&sa,sizeof(sa));
    sa.sa_handler = handle_signal;
    SO_BANCHINE = 10;/*RICORDIAMOCI CHE QUESTO NUMERO GLIELO DOBBIAMO PASSARE DAL MAIN*/
    TEST_ERROR;
    sigaction(SIGINT,&sa,NULL);
    TEST_ERROR;
    /*DEFINIZIONE DEL NUMERO DI BANCHINE*/
    clock_gettime(CLOCK_REALTIME ,&now);
    SO_BANCHINE = (now.tv_nsec % SO_BANCHINE)+1;
    sem_id = atoi(argv[1]);



    /*ENTRA IN UN CICLO INFINITO PER ATTENDERE LA TERMINAZIONE DEL PADRE.
    VA POI MODIFICATO PER ESEGUIRE LE OPERAZIONI NECESSARIE.*/
    for(;;){}
    exit(0);
}


void reset_sem(int sem_id){
    int i;
    for(i=0;i<SO_BANCHINE;i++){
        semctl(sem_id, i, SETVAL, 0);
        TEST_ERROR;
    }
}

void sem_accesso(int semid, int num_risorsa){
    struct sembuf my_op;
    printf("\nil processo:%d tenta l'accesso al semaforo:%d\n",getpid(),semid);
    my_op.sem_num = num_risorsa;
    my_op.sem_flg = 0;
    my_op.sem_op = -1;
    semop(semid,&my_op,1);
    printf("\nil processo:%d ha avuto accesso al semaforo:%d\n",getpid(),semid);
    TEST_ERROR;
}

void sem_uscita(int semid, int num_risorsa){
    struct sembuf my_op;
    my_op.sem_num = num_risorsa;
    my_op.sem_flg = 0;
    my_op.sem_op = 1;
    semop(semid,&my_op,1);
    printf("\nil processo:%d è uscito dal semaforo:%d\n",getpid(),semid);
    TEST_ERROR;
}