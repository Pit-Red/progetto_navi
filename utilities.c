#include "utilities.h"
#include <unistd.h>
#include <errno.h>
#include <math.h>



void sem_accesso(int semid, int num_risorsa){
    struct sembuf my_op;
    /*printf("\nil processo:%d tenta l'accesso al semaforo:%d\n",getpid(),semid);*/
    my_op.sem_num = num_risorsa;
    my_op.sem_flg = 0;
    my_op.sem_op = -1;
    semop(semid,&my_op,1);
    /*printf("\nil processo:%d ha avuto accesso al semaforo:%d\n",getpid(),semid);*/
    TEST_ERROR;
}

void sem_uscita(int semid, int num_risorsa){
    struct sembuf my_op;
    my_op.sem_num = num_risorsa;
    my_op.sem_flg = 0;
    my_op.sem_op = 1;
    semop(semid,&my_op,1);
    /*printf("\nil processo:%d è uscito dal semaforo:%d\n",getpid(),semid);*/
    TEST_ERROR;
}