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

/*MACRO PER NON METTERE INPUT*/
#define NO_INPUT
/*MACRO PER LA VELOCITA DELLE NAVI E LA CAPACITA*/
#define SO_VELOCITA "20"
#define SO_CAPACITY "100"

/* LA SEGUENTE MACRO E' STATA PRESA DA test-pipe-round.c */
#define TEST_ERROR    if (errno) {fprintf(stderr,           \
                      "%s:%d: PID=%5d: Error %d (%s)\n", \
                      __FILE__,         \
                      __LINE__,         \
                      getpid(),         \
                      errno,            \
                      strerror(errno));}

/*DICHIARAZIONE DEGLI ARRAY DEI PID DEI PORTI E DELLE NAVI*/
pid_t* na;
pid_t* po;
int SO_NAVI, SO_PORTI, q_id;
int idshmnavi, idshmporti;

/*STRUCT PER DEFINIRE LE COORDINATE DEI PORTI E DELLE NAVI E I RELATIVI PID*/
typedef struct {
    pid_t pid;
    double x;
    double y;
} sinfo;

struct my_msg_q {
    long mtype;
    int quantita;
    int tipo;
};

void handle_alarm(int signal);
/*HANDLER PER IL SEGNALE MANUALE DI TERMINAZIONE*/
void close_all(int signum);

void sem_accesso(int semid,int num_risorsa);

void sem_uscita(int semid,int num_risorsa);

/*HANDLER PER IL SEGNALE DI FINE PROGRAMMA (ALARM)*/

int main() {
    /* DICHIARAZIONE DELLE VARIABILI */
    int sem_id; /*id del semaforo che permette l'accesso alla shm*/
    char stringsem_id[3 * sizeof(sem_id) + 1];
    char stringporti[3 * sizeof(idshmporti) + 1];
    char stringnavi[3 * sizeof(idshmnavi) + 1];
    char* nave[8] = {""};
    char* porto[8] = {""};
    sinfo* shmnavi, *shmporti;
    short uguali;
    struct timespec now;
    sinfo* arrayporti;
    sinfo* arraynavi;
    int i, j, c;
    double SO_LATO;
    int status;
    struct sigaction ca;
    struct sigaction sa;
    bzero(&ca, sizeof(ca));
    ca.sa_handler = close_all;
    sigaction(SIGINT, &ca, NULL);
    bzero(&sa, sizeof(sa));
    sa.sa_handler = handle_alarm;
    sigaction(SIGALRM, &sa, NULL);

    sem_id = semget(IPC_PRIVATE, 2, 0600); /*INIZIALIZZAZIONE DI 2 SEMAFORI,
                                            IL SEMAFORO 0 SI OCCUPA DELLA SHM DEL 
                                            PORTO ED IL SEMAFORO 1 DELLA SHM DELLE NAVI*/

    /*CREO LA CODA DI MESSAGGI*/
    /*q_id = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | 0666);
    TEST_ERROR;*/
    printf("sem_id: %d\n\n",sem_id);
    srand(time(NULL));


    /*INIZIO INPUT*/
    printf("\033[033;34m");
    
#ifndef NO_INPUT
    printf("inserisci la grandezza della mappa: ");
    scanf("%le", &SO_LATO);
    do {
        printf("inserisci il numero di navi: ");
        scanf("%d", &SO_NAVI);
    } while (SO_NAVI < 1);
    do {
        printf("inserisci il numero di porti: ");
        scanf("%d", &SO_PORTI);
    } while (SO_PORTI < 4);
    
#endif

#ifdef NO_INPUT
    SO_LATO = 10;   /*(n > 0) !di tipo double!*/
    SO_NAVI = 5;    /*(n >= 1)*/
    SO_PORTI = 5;   /*(n >= 4)*/
#endif
    /*FINE INPUT*/

    /*INIZIO MENU*/
    printf("\nSO_LATO = %.2f", SO_LATO);
    printf("\nSO_NAVI = %d", SO_NAVI);
    printf("\nSO_PORTI = %d", SO_PORTI);
   
    idshmporti = shmget(IPC_PRIVATE, sizeof(arrayporti), 0600);
    idshmnavi = shmget(IPC_PRIVATE, sizeof(arraynavi), 0600);
    shmporti = shmat(idshmporti, NULL, 0);
    shmnavi = shmat(idshmnavi, NULL, 0);
    /*ALLOCAZIONE DELLA MEMORIA PER GLI ARRAY DEI PID DEI FIGLI*/
    na = calloc(SO_NAVI, sizeof(*na));
    po = calloc(SO_PORTI, sizeof(*po));
    printf("\nidshmporti: %d\n\n", idshmporti);
    printf("\033[0m");
    /*FINE MENU*/
    sprintf(stringsem_id, "%d", sem_id);
    sprintf(stringporti, "%d", idshmporti);
    TEST_ERROR;
    porto[1] = stringsem_id;
    porto[2] = stringporti;
    porto[3] = stringnavi;
    porto[4] = NULL;
    sprintf(stringnavi, "%d", idshmnavi);
    TEST_ERROR;
    nave[1] = stringsem_id;
    nave[2] = stringporti;
    nave[3] = stringnavi;
    nave[4] = SO_CAPACITY;
    nave[5] = SO_VELOCITA;
    nave[6] = NULL;

    /*DICHIARAZOINE SEMAFORO FIRST*/
    semctl(sem_id, 0 , SETVAL, 1);
    semctl(sem_id, 1 , SETVAL, 1);
    TEST_ERROR;


    TEST_ERROR;
    printf("\n\nciao\n\n");

    alarm(5);
    arrayporti = calloc(SO_PORTI, sizeof(*arrayporti));
    /*CREAZIONE DEI PORTI*/
    for (i = 0; i < SO_PORTI; i++) {
        po[i] = fork();
        if (po[i] == -1) {
            TEST_ERROR;
            exit(1);
        }
        if (po[i] == 0) {
            /* CHILD */
            arrayporti[i].pid = getpid();
            switch (i) {
            case 0:
                arrayporti[i].x = 0;
                arrayporti[i].y = 0;
                break;
            case 1:
                arrayporti[i].x = SO_LATO;
                arrayporti[i].y = 0;
                break;
            case 2:
                arrayporti[i].x = 0;
                arrayporti[i].y = SO_LATO;
                break;
            case 3:
                arrayporti[i].x = SO_LATO;
                arrayporti[i].y = SO_LATO;
                break;
            }
            if (i > 3) {
                j = 0;
                do {
                    int RANDMAX = (int)SO_LATO;
                    clock_gettime(CLOCK_REALTIME , &now);
                    arrayporti[i].x = (double)(now.tv_nsec % (RANDMAX * 100)) / 100;
                    clock_gettime(CLOCK_REALTIME , &now);
                    arrayporti[i].y = (double)(now.tv_nsec % (RANDMAX * 100)) / 100;
                    uguali = 0;
                    for (j = 0; j < i; j++) {
                        if (arrayporti[i].x == arrayporti[j].x && arrayporti[i].y == arrayporti[j].y) {
                            uguali = 1;
                        }
                    }
                } while (uguali);
            }
            sem_accesso(sem_id,0);
            shmporti[i] = arrayporti[i];
            sem_uscita(sem_id,0);
            printf("creazione porto[%d], di pid:%d con coordinate x=%.2f, y=%.2f\n\n", i, arrayporti[i].pid, arrayporti[i].x, arrayporti[i].y);
            execvp("./porto", porto);
            TEST_ERROR;
            exit(EXIT_FAILURE);
        }
        else {

        }
    }
    /*sem_accesso(sem_id,0);
    strncpy((char*)shmporti, (char*)arrayporti, sizeof(arrayporti));
    TEST_ERROR;
    sem_uscita(sem_id,0);*/
    arraynavi = calloc(SO_NAVI, sizeof(*arraynavi));
    /* CREAZIONE DELLE NAVI */
    for (i = 0; i < SO_NAVI; i++) {
        na[i] = fork();
        if (na[i] == -1) {
            TEST_ERROR;
            exit(1);
        }
        if (na[i] == 0) {
            /* CHILD */
            arraynavi[i].pid = getpid();
            do {
                int RANDMAX = (int)SO_LATO;
                clock_gettime(CLOCK_REALTIME , &now);
                arraynavi[i].x = (double)(now.tv_nsec % (RANDMAX * 100)) / 100;
                clock_gettime(CLOCK_REALTIME , &now);
                arraynavi[i].y = (double)(now.tv_nsec % (RANDMAX * 100)) / 100;
                uguali = 0;
                for (j = 0; j < i; j++) {
                    if (arraynavi[i].x == arraynavi[j].x && arraynavi[i].y == arraynavi[j].y) {
                        uguali = 1;
                    }
                }
            } while (uguali);
            /*           sprintf(nave[3], "%f",arraynavi[i].y);
                       sprintf(nave[4], "%f",arraynavi[i].x);
                       TEST_ERROR;*/
            sem_accesso(sem_id,1);
            shmnavi[i] = arraynavi[i];
            sem_uscita(sem_id,1);
            printf("creazione nave[%d], di pid:%d con coordinate x=%.2f, y=%.2f\n\n",i, arraynavi[i].pid, arraynavi[i].x, arraynavi[i].y);
            execvp("./nave", nave);
            TEST_ERROR;
            exit(EXIT_FAILURE);
        }
        else {
            /* PARENT */
        }
    }
    /*sem_accesso(sem_id,1);
    strncpy((char*)shmnavi, (char*)arraynavi, sizeof(arraynavi));
    TEST_ERROR;
    sem_uscita(sem_id,1);*/

    TEST_ERROR;
    /*IL PROCESSO PADRE RIMANE IN PAUSA FINO ALL'ARRIVO DI UN SEGNALE (ALARM)*/
    printf("\n\n%d\n\n",pause());
    TEST_ERROR;

    shmctl(idshmporti,IPC_RMID,NULL);
    TEST_ERROR;
    shmctl(idshmnavi,IPC_RMID,NULL);
    TEST_ERROR;
    semctl(sem_id,1,IPC_RMID);
    TEST_ERROR;
    /*DEALLOCAZIONE DELLA CODA*/
    /*while(msgctl(q_id, IPC_RMID, NULL)){
        TEST_ERROR;
    }


    while(msgctl(1, IPC_RMID, NULL)){
        TEST_ERROR;
    }
    while(msgctl(7, IPC_RMID, NULL)){
        TEST_ERROR;
    }
    while(msgctl(0, IPC_RMID, NULL)){
        TEST_ERROR;
    }
    while(msgctl(2, IPC_RMID, NULL)){
        TEST_ERROR;
    }
    while(msgctl(3, IPC_RMID, NULL)){
        TEST_ERROR;
    }
    while(msgctl(4, IPC_RMID, NULL)){
        TEST_ERROR;
    }
    while(msgctl(5, IPC_RMID, NULL)){
        TEST_ERROR;
    }
    while(msgctl(6, IPC_RMID, NULL)){
        TEST_ERROR;
    }*/
    printf("\n\nFine del programma\n");

    exit(EXIT_SUCCESS);
}

void handle_alarm(int signum) {
    int i, status;
    printf("\n\n\n\n");
    for (i = 0; i < SO_NAVI; i++) {
        kill(na[i], SIGINT);
        TEST_ERROR;
        wait(&status);
        TEST_ERROR;
    }
    for (i = 0; i < SO_PORTI; i++) {
        kill(po[i], SIGINT);
        TEST_ERROR;
        wait(&status);
        TEST_ERROR;
    }
}
void close_all(int signum) {
    /*DEALLOCAZIONE DELLA CODA*/
    while (msgctl(q_id, IPC_RMID, NULL)) {
        TEST_ERROR;
    }
    while (msgctl(6, IPC_RMID, NULL)) {
        TEST_ERROR;
    }
    shmctl(idshmporti, IPC_RMID, NULL);
    shmctl(29, IPC_RMID, NULL);
    shmctl(30, IPC_RMID, NULL);
    TEST_ERROR;
    printf("\n\n\033[0;33mFine del programma\n");
    exit(0);
    
}

void sem_accesso(int semid,int num_risorsa){
    struct sembuf my_op;
    /*printf("\nil processo:%d tenta l'accesso al semaforo:%d\n",getpid(),semid);*/
    my_op.sem_num = num_risorsa;
    my_op.sem_flg = 0;
    my_op.sem_op = -1;
    semop(semid,&my_op,1);
    /*printf("\nil processo:%d ha avuto accesso al semaforo:%d\n",getpid(),semid);*/
    TEST_ERROR;
}

void sem_uscita(int semid,int num_risorsa){
    struct sembuf my_op;
    my_op.sem_num = num_risorsa;
    my_op.sem_flg = 0;
    my_op.sem_op = 1;
    semop(semid,&my_op,1);
    /*printf("\nil processo:%d è uscito dal semaforo:%d\n",getpid(),semid);*/
    TEST_ERROR;
}