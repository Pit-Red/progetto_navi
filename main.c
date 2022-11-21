#define GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>

/* LA SEGUENTE MACRO E' STATA PRESA DA test-pipe-round.c */
#define TEST_ERROR    if (errno) {fprintf(stderr,			\
					  "%s:%d: PID=%5d: Error %d (%s)\n", \
					  __FILE__,			\
					  __LINE__,			\
					  getpid(),			\
					  errno,			\
					  strerror(errno));}
/*DICHIARAZIONE DEGLI ARRAY DEI PID DEI PORTI E DELLE NAVI*/

pid_t* na;
pid_t* po;

int main(){
    /* DICHIARAZIONE DELLE VARIABILI */
    int SO_NAVI, SO_PORTI, i;
    double SO_LATO;
    char* nave[]= {"","35","15.3","3.5", NULL};  /*STO PASSANDO COME ARGOMENTO LA VELOCITA DELLA NAVE E LA POSIZIONE INIZIALE*/
    char* porto[] = {"ciao", "12", "25","34",NULL};
    int status;

    printf("inserisci la grandezza della mappa:");
    scanf("%le",&SO_LATO);
    do{
        printf("inserisci il numero di navi:");
        scanf("%d",&SO_NAVI);
    }while(SO_NAVI<1);
    do{
        printf("inserisci il numero di porti:");
        scanf("%d",&SO_PORTI);
    }while(SO_PORTI<4);

    /*ALLOCAZIONE DELLA MEMORIA PER GLI ARRAY DEI PID DEI FIGLI*/
    na = calloc(SO_NAVI,sizeof(*na));
    po = calloc(SO_PORTI, sizeof(*po));

    /*CREAZIONE DEI PORTI*/
    for(i=0;i<SO_PORTI;i++){
        po[i] = fork();
        if(po[i] == -1){
            TEST_ERROR;
            exit(1);
        }
        if(po[i] == 0){
            /* CHILD */
            execvp("./porto", porto);
            TEST_ERROR;
            exit(EXIT_FAILURE);
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
            exit(1);
        }
        if(na[i] == 0){
            /* CHILD */
            execvp("./nave", nave);
            TEST_ERROR;
            exit(EXIT_FAILURE);
        }
        else{
            /* PARENT */
        }
    }
    sleep(1);
    /*UCCISIONE DI TUTTI I PROCESSI FIGLI*/

    for(i=0;i<SO_PORTI;i++){
        kill(po[i], SIGINT);
    }
    exit(EXIT_SUCCESS);
}
