#define GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#define SO_NAVI     8
#define SO_PORTI    10
#define SO_LATO     50
#define ASCISSA_PORTO rand()%SO_LATO
/* LA SEGUENTE MACRO E' STATA PRESA DA test-pipe-round.c */
#define TEST_ERROR    if (errno) {fprintf(stderr,			\
					  "%s:%d: PID=%5d: Error %d (%s)\n", \
					  __FILE__,			\
					  __LINE__,			\
					  getpid(),			\
					  errno,			\
					  strerror(errno));}

int* na;
int* po;


int main(){
    /* DICHIARAZIONE DELLE VARIABILI */
    int numNavi = SO_NAVI, numPorti = SO_PORTI, i, temp;
    double lato = SO_LATO;
    char* nave[]= {"","35","15.3","3.5"};  /*STO PASSANDO COME ARGOMENTO LA VELOCITA DELLA NAVE E LA POSIZIONE INIZIALE*/
    char* porto[] = {"ciao", "12", "25"};
    char* env[] = {"ajfjdslv","fsknd","fslnfdl"};
    
    na = calloc(numNavi,sizeof(*na));
    po = calloc(numPorti, sizeof(*po));

    /*CREAZIONE DEI PORTI*/
    for(i=0;i<SO_PORTI;i++){
        po[i] = fork();
        if(po[i] == -1){
            TEST_ERROR;
        }
        if(po[i] == 0){
            /* CHILD */
            execve("prova-porto", porto, env);
        }
        else{
            /* PARENT */
        }
    }

    /* CREAZIONE DELLE NAVI */
    for(i=0;i<SO_NAVI;i++){
        na[i] = fork();
        if(na[i] == -1){
            TEST_ERROR;
        }
        if(na[i] == 0){
            /* CHILD */
            execve("prova-nave", nave, env);
        }
        else{
            /* PARENT */
        }
    }
    

}
