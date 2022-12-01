#include "utilities.h"
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>



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

void stampa_merci(smerce* temp_merci){
    int i;
    int size = sizeof(temp_merci)/sizeof(*temp_merci);
    for(i=0; i<size;i++){
        printf("\nsmerce[%d]:quantita=%d\tdimensione=%d\ttempo di scadenza=%d\n", i,temp_merci[i].quantita,temp_merci[i].dimensione, temp_merci[i].tempo_scadenza);
    }
}