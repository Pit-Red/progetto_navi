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
#include <sys/msg.h>


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
int SO_NAVI, SO_PORTI;

typedef struct{
    pid_t pid;
    double x;
    double y;
}sporto;

struct my_msg_q{
	long mtype;
	int quantita;
	int tipo;
};

void handle_alarm(int signal);


/*HANDLER PER IL SEGNALE DI FINE PROGRAMMA (ALARM)*/

int main(){
    /* DICHIARAZIONE DELLE VARIABILI */
    struct timespec now;
    sporto* arrayporti;
    int i,c, q_id;
    double SO_LATO;
    char* nave[]= {"","35","15.3","3.5", NULL};  /*STO PASSANDO COME ARGOMENTO LA VELOCITA DELLA NAVE E LA POSIZIONE INIZIALE*/
    char* porto[] = {"", "12", "25","34",NULL};
    int status;
    FILE* my_f;
    struct sigaction sa;
    bzero(&sa, sizeof(sa));
    sa.sa_handler = handle_alarm;
    sigaction(SIGALRM, &sa, NULL);

    /*CREO LA CODA DI MESSAGGI*/
    q_id = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | 0666);
    TEST_ERROR;

    srand(time(NULL));
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

    /*ALLOCAZ   IONE DELLA MEMORIA PER GLI ARRAY DEI PID DEI FIGLI*/
    na = calloc(SO_NAVI,sizeof(*na));
    po = calloc(SO_PORTI, sizeof(*po));

    alarm(4);
    arrayporti = calloc(SO_PORTI,sizeof(*arrayporti));
    /*CREAZIONE DEI PORTI*/
    for(i=0;i<SO_PORTI;i++){
        po[i] = fork();
        if(po[i] == -1){
            TEST_ERROR;
            exit(1);
        }
        if(po[i] == 0){
            /* CHILD */
            arrayporti[i].pid = getpid();
            switch(i){
                case 0:
                    arrayporti[i].x = 0;
                    arrayporti[i].y = 0;
                    break;
                case 1:
                    arrayporti[i].x = SO_LATO;
                    arrayporti[i].y = 0;
                    break;
                case 2:
                    arrayporti[i].x = SO_LATO;
                    arrayporti[i].y = 0;
                    break;
                case 3:
                    arrayporti[i].x = SO_LATO;
                    arrayporti[i].y = SO_LATO;
                    break;
            }
            if(i>3){
                int RANDMAX = (int)SO_LATO;
                clock_gettime(CLOCK_REALTIME ,&now);
                arrayporti[i].x = (double)(now.tv_nsec % (RANDMAX*100))/100;
                clock_gettime(CLOCK_REALTIME ,&now);
                arrayporti[i].y = (double)(now.tv_nsec % (RANDMAX*100))/100;
            }
            printf("creazione porto %d con cordinate x=%.2f, y=%.2f\n\n", arrayporti[i].pid, arrayporti[i].x,arrayporti[i].y);
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
    /*IL PROCESSO PADRE RIMANE IN PAUSA FINO ALL'ARRIVO DI UN SEGNALE (ALARM)*/
    pause();
	
    /*DEALLOCAZIONE DELLA CODA*/
    while(msgctl(q_id, IPC_RMID, NULL)){
	    TEST_ERROR;
    }
    
    printf("\n\nFine del programma\n");
    
    exit(EXIT_SUCCESS);
}

void handle_alarm(int signum){
    int i,status;
    printf("\n\n\n\n");
    for(i=0;i<SO_NAVI;i++){
        kill(na[i], SIGINT);
        wait(&status);
    }
    for(i=0;i<SO_PORTI;i++){
        kill(po[i], SIGINT);
        wait(&status);
    }
}