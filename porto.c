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

int SO_BANCHINE;
int sem_id;

/*HANDLER PER GESTIRE IL SEGNAÒLE DI TERMINAZIONE DEL PADRE*/
void handle_signal(int signum){
    semctl(sem_id,1,IPC_RMID);
    TEST_ERROR;
    printf("E' stato ucciso il porto.\n");
    exit(0);
}

int main(int argc, char** argv){
    /*DICHIARAZIONE DELLE VARIABILI*/
    struct sembuf sops;
    struct sigaction sa;
    struct timespec now;
    bzero(&sa,sizeof(sa));
    sa.sa_handler = handle_signal;
    SO_BANCHINE = 10;/*RICORDIAMOCI CHE QUESTO NUMERO GLIELO DOBBIAMO PASSARE DAL MAIN*/
    sem_id = semget(IPC_PRIVATE, SO_BANCHINE, 0600); /*INIZIALIZZAZIONE DI SO_BANCHINE SEMAFORI*/
    TEST_ERROR;
    sops.sem_flg = 0;/*NESSUNA FLAG*/
    sigaction(SIGINT,&sa,NULL);
    TEST_ERROR;
    /*DEFINIZIONE DEL NUMERO DI BANCHINE*/
    clock_gettime(CLOCK_REALTIME ,&now);
    SO_BANCHINE = now.tv_nsec % SO_BANCHINE;
    printf("E' stato creato un porto con %d banchine\n\n\n", SO_BANCHINE);
    TEST_ERROR;



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
