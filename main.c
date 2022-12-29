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
#include "utilities.h"

/*MACRO PER NON METTERE INPUT*/
#define NO_INPUt

#define STAMPA_MINIMA


int num_tempesta, num_mareggiata;
/*DICHIARAZIONE DELLE VARIABILI GLOBALI DEL MASTER UTILI PER LA SIMULAZIONE*/
pid_t pid_maelstorm;
pid_t* na;  /*array con i Pid delle navi*/
pid_t* po;  /*array con i Pid dei porti*/
int SO_NAVI, SO_PORTI, msg_richiesta, msg_offerta, SO_BANCHINE, SO_SIZE, SO_FILL, SO_DAYS;
int idshmnavi, idshmporti, idshmmerci, idshmgiorno,  idshmfill;
int sem_shmporto; int sem_shmnave; int sem_avvio; /*id del semaforo che permette l'accesso alla shm*/
int sem_porto, sem_ricoff;/*semaforo per far approdare le navi al porto*/
snave* shmnavi; sporto* shmporti; smerce* shmmerci; int giorno; int* shmgiorno; int* shmfill;
int SO_CAPACITY;
int durata_giorno;

void inizializzazione_fill();

int isRequestEmpty();

void tempesta();

void mareggiata();
int conta_quante();

void accessoPortiNavi();
void uscitaPortiNavi();

/*HANDLER DEI VARI SEGNALI*/
void handle_alarm(int signal);
void close_all(int signum);
void chiudi_maelstorm(int signum);
void chiudi_maelstorm1();

int main() {
    /* DICHIARAZIONE DELLE VARIABILI */
    int input_type, param_config;
    char stringsem_avvio[13];
    char stringsem_shmporto[13];
    char stringsem_shmnave[13];
    char stringsem_porto[3 * sizeof(sem_porto) + 1];
    char stringporti[3 * sizeof(idshmporti) + 1];
    char stringnavi[3 * sizeof(idshmnavi) + 1];
    char stringfill[13], stringsem_ricoff[13];
    char stringmerci[13];
    char stringid[13];
    char stringSO_NAVI[13];
    char stringofferta[13];
    char stringvelocita[13], stringcapacity[13], stringgiorno[13], stringload_speed[13], stringnum_merci[13], stringoretempesta[13], stringoremareggiata[13];
    char* nave[20] = {""};
    char* porto[20] = {""};
    short uguali;
    struct sembuf my_op;
    struct timespec now;
    sporto* arrayporti;
    snave* arraynavi;
    smerce* arraymerci;
    int i, j, c, banchine_effettive, d;
    double SO_LATO;
    int SO_MERCI;
    int SO_MIN_VITA, SO_MAX_VITA;
    int SO_VELOCITA;
    int SO_LOADSPEED;
    int SO_SPEED;
    int SO_STORM_DURATION, SO_SWELL_DURATION, SO_MAELSTROM;
    int status;
    struct sigaction ca;
    struct sigaction sa;
    bzero(&ca, sizeof(ca));
    ca.sa_handler = close_all;
    sigaction(SIGINT, &ca, NULL);
    TEST_ERROR;
    bzero(&sa, sizeof(sa));
    sa.sa_handler = handle_alarm;
    sigaction(SIGALRM, &sa, NULL);
    TEST_ERROR;
    bzero(&sa, sizeof(sa));
    sa.sa_handler = chiudi_maelstorm;
    sigaction(SIGTERM, &sa, NULL);

    srand(time(NULL));
    num_tempesta = 0;
    num_mareggiata = 0;
    giorno = 0;
    durata_giorno = 2;
    /*INIZIO INPUT*/

#ifndef NO_INPUT/*TO DO: CONTROLLO CHE I PARAMETRI SIANO POSITIVI*/
    printf("\033[033;33m\n╔═══════════════════════════════════╗\n║ [0] Scelta Scenario               ║\n║ [1] Input Parametri Manuale       ║\n╚═══════════════════════════════════╝\n\n\n\n\033[033;0m");
    do {
        rmLinesTerminal(4);
        printf("\nScegliere modalità di input\n");
        scanf("%d", &input_type);
    } while (input_type != 0 && input_type != 1);
    rmLinesTerminal(8);

    if (input_type == 0) {
        printf("\033[033;33m\n╔═══════════════════════════════════╗\n║              SCENARI              ║\n");

        printf("╠═══════════════════════════════════╣\n║ [0] Dense, Small Ships            ║\n║ [1] Dense, Small Ships + Trashing ║\n║ [2] Born To Run                   ║\n║ [3] Cargos, Big Stuff             ║\n║ [4] Unlucky Cargos                ║\n║ [5] Test                          ║\n╚═══════════════════════════════════╝\033[033;0m\n\n\n\n");

        do {
            rmLinesTerminal(4);
            printf("\nScegliere scenario\n");
            scanf("%d", &param_config);
        } while (param_config != 0 && param_config != 1 && param_config != 2 && param_config != 3 && param_config != 4 && param_config != 5);
        rmLinesTerminal(14);

        switch (param_config) {
        case 0:/*dense, small ships*/
            printf("\033[033;33m\n SCENARIO:\033[033;32m DENSE, SMALL SHIPS\033[033;0m\n");
            SO_NAVI = 1000;
            SO_PORTI = 100;
            SO_MERCI = 1;
            SO_SIZE = 1;
            SO_MIN_VITA = 50;
            SO_MAX_VITA = 50;
            SO_LATO = 1000;
            SO_SPEED = 500;
            SO_CAPACITY = 10;
            SO_BANCHINE = 2;
            SO_FILL = 500000;
            SO_LOADSPEED = 200;
            SO_DAYS = 10;
            SO_STORM_DURATION = 6;
            SO_SWELL_DURATION = 24;
            SO_MAELSTROM = 1;
            break;

        case 1:/*as above + trashing*/
            printf("\033[033;32m\n SCENARIO:\033[033;31m DENSE, SMALL SHIPS + TRASHING\033[033;0m\n");
            SO_NAVI = 1000;
            SO_PORTI = 100;
            SO_MERCI = 10;
            SO_SIZE = 1;
            SO_MIN_VITA = 3;
            SO_MAX_VITA = 10;
            SO_LATO = 1000;
            SO_SPEED = 500;
            SO_CAPACITY = 10;
            SO_BANCHINE = 2;
            SO_FILL = 500000;
            SO_LOADSPEED = 200;
            SO_DAYS = 10;
            SO_STORM_DURATION = 6 ;
            SO_SWELL_DURATION = 24;
            SO_MAELSTROM = 1;
            break;

        case 2:/*born to run*/
            printf("\033[033;33m\n SCENARIO:\033[033;32m BORN TO RUN\033[033;0m\n");
            SO_NAVI = 10;
            SO_PORTI = 1000;
            SO_MERCI = 100;
            SO_SIZE = 100;
            SO_MIN_VITA = 3;
            SO_MAX_VITA = 10;
            SO_LATO = 1000;
            SO_SPEED = 2000;
            SO_CAPACITY = 1000;
            SO_BANCHINE = 10;
            SO_FILL = 1000000;
            SO_LOADSPEED = 500;
            SO_DAYS = 10;
            SO_STORM_DURATION = 6;
            SO_SWELL_DURATION = 24;
            SO_MAELSTROM = 60;
            break;

        case 3:/*cargos, big stuff*/
            printf("\033[033;33m\n SCENARIO:\033[033;32m CARGOS, BIG STUFF\033[033;0m\n");
            SO_NAVI = 100;
            SO_PORTI = 5;
            SO_MERCI = 10;
            SO_SIZE = 100;
            SO_MIN_VITA = 3;
            SO_MAX_VITA = 10;
            SO_LATO = 1000;
            SO_SPEED = 500;
            SO_CAPACITY = 1000;
            SO_BANCHINE = 10;
            SO_FILL = 1000000;
            SO_LOADSPEED = 200;
            SO_DAYS = 10;
            SO_STORM_DURATION = 6;
            SO_SWELL_DURATION = 24;
            SO_MAELSTROM = 24;
            break;

        case 4:/*unlucky cargos*/
            printf("\033[033;33m\n SCENARIO:\033[033;32m UNLUCKY CARGOS\033[033;0m\n");
            SO_NAVI = 100;
            SO_PORTI = 5;
            SO_MERCI = 10;
            SO_SIZE = 100;
            SO_MIN_VITA = 3;
            SO_MAX_VITA = 10;
            SO_LATO = 1000;
            SO_SPEED = 5;
            SO_CAPACITY = 1000;
            SO_BANCHINE = 10;
            SO_FILL = 1000000;
            SO_LOADSPEED = 20;
            SO_DAYS = 10;
            SO_STORM_DURATION = 12;
            SO_SWELL_DURATION = 10;
            SO_MAELSTROM = 1;
            break;

        case 5:/*test*/
            printf("\033[033;33m\n SCENARIO:\033[033;32m TEST\033[033;0m\n");
            SO_NAVI = 15;
            SO_PORTI = 5;
            SO_MERCI = 3;
            SO_SIZE = 1;
            SO_MIN_VITA = 3;
            SO_MAX_VITA = 10;
            SO_LATO = 100;
            SO_SPEED = 20;
            SO_CAPACITY = 300;
            SO_BANCHINE = 10;
            SO_FILL = 1000;
            SO_LOADSPEED = 20;
            SO_DAYS = 10;
            SO_STORM_DURATION = 12;
            SO_SWELL_DURATION = 12;
            SO_MAELSTROM = 100000;
            break;
        }


    } else if (input_type == 1) {
        printf("\n--------------------->Input Parametri Manuale<---------------------\n");

        printf("inserisci la grandezza della mappa: ");
        scanf("%le", &SO_LATO);
        do {
            printf("inserisci il numero di navi: ");
            scanf("%d", &SO_NAVI);
        } while (SO_NAVI < 1 && SO_NAVI <= 0);
        do {
            printf("inserisci il numero di porti: ");
            scanf("%d", &SO_PORTI);
        } while (SO_PORTI < 4 && SO_PORTI <= 0);
        do {
            printf("inserisci il numero di merci utilzzabi: ");
            scanf("%d", &SO_MERCI);
        } while (SO_MERCI <= 0);
        do {
            printf("inserisci la dimensione massima della merce: ");
            scanf("%d", &SO_SIZE);
        } while (SO_SIZE <= 0);
        do {
            printf("inserisci il minimo di giorni di vita delle merci: ");
            scanf("%d", &SO_MIN_VITA);
        } while (SO_MIN_VITA <= 0);
        do {
            printf("inserisci il massimo di giorni di vita delle merci: ");
            scanf("%d", &SO_MAX_VITA);
        } while (SO_MAX_VITA <= 0 && SO_MAX_VITA < SO_MIN_VITA);
        do {
            printf("inserisci la velocita' delle navi: ");
            scanf("%d", &SO_SPEED);
        } while (SO_SPEED <= 0);
        do {
            printf("inserisci la velocita' di carico e scarico delle navi: ");
            scanf("%d", &SO_LOADSPEED);
        } while (SO_LOADSPEED <= 0);
        do {
            printf("inserisci il carico massimo trasportabile delle navi: ");
            scanf("%d", &SO_CAPACITY);
        } while (SO_CAPACITY <= 0);
        do {
            printf("inserisci il numero massimo di banchine in un porto: ");
            scanf("%d", &SO_BANCHINE);
        } while (SO_BANCHINE <= 0);
        do {
            printf("inserisci numero massimo di tonnelate genereabile da offerte/richieste: ");
            scanf("%d", &SO_FILL);
        } while (SO_FILL <= 0);
        do {
            printf("inserisci la durata della simulazione(in giorni): ");
            scanf("%d", &SO_DAYS);
        } while (SO_DAYS <= 0);
        do {
            printf("inserisci la durata della tempesta(in ore): ");
            scanf("%d", &SO_STORM_DURATION);
        } while (SO_STORM_DURATION <= 0);
        do {
            printf("inserisci la durata della mareggiata(in ore): ");
            scanf("%d", &SO_SWELL_DURATION);
        } while (SO_SWELL_DURATION <= 0);
        do {
            printf("inserisci l' intervallo di tempo tra gli uragani(in ore): ");
            scanf("%d", &SO_MAELSTROM);
        } while (SO_MAELSTROM <= 0);

    }
    sleep(3);/*WARNING USATA UNA SLEEP*/
#endif

#ifdef NO_INPUT
    SO_LATO = 100;   /*(n > 0) !di tipo double!*/
    SO_NAVI = 1000;    /*(n >= 1)*/
    SO_PORTI = 100;   /*(n >= 4)*/
    SO_BANCHINE = 2;
    SO_MERCI = 1;
    SO_SIZE = 10;
    SO_CAPACITY = 10000;
    SO_VELOCITA = 20;
    SO_MAX_VITA = 50;
    SO_MIN_VITA = 50;
    SO_LOADSPEED = 2000;
    SO_FILL = 1000000;
    SO_DAYS = 10;
    SO_SPEED = 200;

#endif

    /*FINE INPUT*/
    /*INIZIO MENU*/
    printf("\nSO_LATO = %.2f", SO_LATO);
    printf("\nSO_NAVI = %d", SO_NAVI);
    printf("\nSO_PORTI = %d", SO_PORTI);

    /*CREO LE CODE DI MESSAGGI, I SEMAFORI E LE MEMORIE CONDIVISE*/
    msg_richiesta = msgget(IPC_PRIVATE, IPC_NOWAIT | IPC_CREAT | IPC_EXCL | 0666);
    msg_offerta = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | 0666);
    sem_porto = semget(IPC_PRIVATE, SO_PORTI, IPC_CREAT | IPC_EXCL | 0600);
    idshmporti = shmget(IPC_PRIVATE, sizeof(*arrayporti) * SO_PORTI, IPC_CREAT | IPC_EXCL | 0600);
    idshmnavi = shmget(IPC_PRIVATE, sizeof(*arraynavi) * SO_NAVI, IPC_CREAT | IPC_EXCL | 0600);
    idshmmerci = shmget(IPC_PRIVATE, sizeof(arraymerci), IPC_CREAT | IPC_EXCL | 0600);
    idshmgiorno = shmget(IPC_PRIVATE, sizeof(giorno), IPC_CREAT | IPC_EXCL | 0600);
    idshmfill = shmget(IPC_PRIVATE, 6 * 4, IPC_CREAT | IPC_EXCL | 0600);
    sem_shmporto = semget(IPC_PRIVATE, SO_PORTI, IPC_CREAT | IPC_EXCL | 0600);
    sem_shmnave = semget(IPC_PRIVATE, SO_NAVI, IPC_CREAT | IPC_EXCL | 0600);
    sem_avvio = semget(IPC_PRIVATE, 2, IPC_CREAT | IPC_EXCL | 0600);
    sem_ricoff = semget(IPC_PRIVATE, 2, IPC_CREAT | IPC_EXCL | 0600);
    TEST_ERROR;
    shmporti = shmat(idshmporti, NULL, 0);
    shmnavi = shmat(idshmnavi, NULL, 0);
    shmmerci = shmat(idshmmerci, NULL, 0);
    shmgiorno = (int*)shmat(idshmgiorno, NULL, 0);
    shmfill = (int*)shmat(idshmfill, NULL, 0);
    semctl(sem_ricoff, 0, SETVAL, 1);   /*richieste*/
    semctl(sem_ricoff, 1, SETVAL, 1);   /*offerte*/
    semctl(sem_avvio, 0 , SETVAL, 0);
    semctl(sem_avvio, 1 , SETVAL, 0);
    /*ALLOCAZIONE DELLA MEMORIA PER GLI ARRAY DEI PID DEI FIGLI*/
    na = calloc(SO_NAVI, sizeof(*na));
    po = calloc(SO_PORTI, sizeof(*po));
    printf("\nidshmporti: %d\n\n", idshmporti);
    printf("\033[0m");
    SO_LOADSPEED /= durata_giorno;
    SO_SPEED /= durata_giorno;
    /*FINE MENU*/
    sprintf(stringsem_avvio, "%d", sem_avvio);
    sprintf(stringsem_porto, "%d", sem_porto);
    sprintf(stringsem_shmporto, "%d", sem_shmporto);
    sprintf(stringsem_shmnave, "%d", sem_shmnave);
    sprintf(stringporti, "%d", idshmporti);
    sprintf(stringnavi, "%d", idshmnavi);
    sprintf(stringmerci, "%d", idshmmerci);
    sprintf(stringfill, "%d", idshmfill);
    sprintf(stringSO_NAVI, "%d", SO_NAVI);
    sprintf(stringofferta, "%d", SO_PORTI);
    sprintf(stringcapacity, "%d", SO_CAPACITY);
    sprintf(stringvelocita, "%d", SO_SPEED);
    sprintf(stringgiorno, "%d", idshmgiorno);
    sprintf(stringload_speed, "%d", SO_LOADSPEED);
    sprintf(stringnum_merci, "%d", SO_MERCI);
    sprintf(stringsem_ricoff, "%d", sem_ricoff);
    sprintf(stringoretempesta, "%d", SO_STORM_DURATION);
    sprintf(stringoremareggiata, "%d", SO_SWELL_DURATION);

    /*PORTO*/
    porto[1] = stringsem_shmporto;
    porto[2] = stringporti;
    porto[3] = stringnavi;
    porto[5] = stringsem_porto;
    porto[6] = stringSO_NAVI;
    porto[7] = stringnum_merci;
    porto[8] = stringmerci;
    porto[9] = stringgiorno;
    porto[10] = stringsem_shmnave;
    porto[11] = stringsem_avvio;
    porto[12] = stringfill;
    porto[13] = stringsem_ricoff;
    porto[14] = NULL;
    /*NAVE*/
    nave[1] = stringsem_shmporto;
    nave[2] = stringporti;
    nave[3] = stringnavi;
    nave[4] = stringcapacity;
    nave[5] = stringvelocita;
    nave[7] = stringsem_porto;
    nave[8] = stringSO_NAVI;
    nave[9] = stringofferta;
    nave[10] = stringgiorno;
    nave[11] = stringload_speed;
    nave[12] = stringmerci;
    nave[13] = stringsem_shmnave;
    nave[14] = stringsem_avvio;
    nave[15] = stringoretempesta;
    nave[16] = stringoremareggiata;
    nave[17] = NULL;

    /*INIZIALIZZAZIONE SEMAFORO FIRST*/
    TEST_ERROR;

    inizializzazione_fill();

    *shmgiorno = giorno;
    shmgiorno[1] = durata_giorno;

    /*ALLOCAZIONE DELLA MEMOIRA E CREAZIONE DELLE MERCI*//*ALLOCAZIONE DELLA MEMOIRA E CREAZIONE DELLE MERCI*/
    arraymerci = calloc(SO_MERCI, sizeof(*arraymerci));
    for (i = 0; i < SO_MERCI; i++) {
        arraymerci[i].id = i;
        clock_gettime(CLOCK_REALTIME, &now);
        arraymerci[i].scadenza = (now.tv_nsec % (SO_MAX_VITA - SO_MIN_VITA + 1 )) + SO_MIN_VITA ;
        arraymerci[i].dimensione = (now.tv_nsec % (SO_SIZE * 100000)) / 100000 + 1;
        clock_gettime(CLOCK_REALTIME, &now);
        printf("merce[%d]:\tSCADENZA:%d\tDIMENSIONE:%d\n", i, arraymerci[i].scadenza, arraymerci[i].dimensione);
        shmmerci[i] = arraymerci[i];
    }


    printf("\n\n\nsem_shmporti:%d\tsem_shmnavi:%d\tsem_avvio:%d\tsem_banchine:%d\n\n\n", sem_shmporto, sem_shmnave, sem_avvio, sem_porto);
    arrayporti = calloc(SO_PORTI, sizeof(*arrayporti));
    bzero(arrayporti, sizeof(*arrayporti) * SO_PORTI);
    bzero(shmporti , sizeof(*arrayporti) * SO_PORTI);
    /*CREAZIONE DEI PORTI*/

    for (i = 0; i < SO_PORTI; i++) {
        po[i] = fork();
        if (po[i] == -1) {
            TEST_ERROR;
            exit(1);
        }
        if (po[i] == 0) {
            /* CHILD */
            semctl(sem_shmporto, i , SETVAL, 1);
            arrayporti[i].pid = getpid();
            arrayporti[i].richiesta_soddisfatta = 0;
            arrayporti[i].destinazione = 0;
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
            default:
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
            clock_gettime(CLOCK_REALTIME, &now);
            banchine_effettive = ((now.tv_nsec % SO_BANCHINE * 1000) / 1000) + 1;
            semctl(sem_porto, i , SETVAL, banchine_effettive);
            sem_accesso(sem_shmporto, i);
            shmporti[i] = arrayporti[i];
            sem_uscita(sem_shmporto, i);
            sprintf(stringid, "%d", i);
            porto[4] = stringid;
            my_op.sem_num = 0;
            my_op.sem_op = 1;
            semop(sem_avvio,&my_op,1);
            /*printf("creazione porto[%d], di pid:%d con coordinate x=%.2f, y=%.2f, con %d banchine\n\n", i, arrayporti[i].pid, arrayporti[i].x, arrayporti[i].y, banchine_effettive);*/

            execvp("./porto", porto);
            TEST_ERROR;
            exit(EXIT_FAILURE);
        }
        else {

        }
    }
    arraynavi = calloc(SO_NAVI, sizeof(*arraynavi));
    /* CREAZIONE DELLE NAVI */
    bzero(arraynavi, sizeof(*arraynavi) * SO_NAVI);
    bzero(shmnavi, sizeof(*arraynavi) * SO_NAVI);

    for (i = 0; i < SO_NAVI; i++) {
        na[i] = fork();
        if (na[i] == -1) {
            TEST_ERROR;
            exit(1);
        }
        if (na[i] == 0) {
            /* CHILD */
            semctl(sem_shmnave, i , SETVAL, 1);
            arraynavi[i].pid = getpid();
            arraynavi[i].stato_nave = 1;
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
            sem_accesso(sem_shmnave, i);
            shmnavi[i] = arraynavi[i];
            sem_uscita(sem_shmnave, i);
            sprintf(stringid, "%d", i);
            nave[6] = stringid;
            my_op.sem_num = 0;
            my_op.sem_op = 1;
            semop(sem_avvio, &my_op, 1);
            /*printf("creazione nave[%d], di pid:%d con coordinate x=%.2f, y=%.2f\n\n", i, arraynavi[i].pid, arraynavi[i].x, arraynavi[i].y);*/
            my_op.sem_num = 1;
            my_op.sem_op = -1;
            semop(sem_avvio, &my_op, 1);

            execvp("./nave", nave);
            TEST_ERROR;
            exit(EXIT_FAILURE);
        }
        else {
            /* PARENT */
        }
    }

    my_op.sem_num = 0;
    my_op.sem_flg = 0;
    my_op.sem_op = -(SO_PORTI + SO_NAVI);
    semop(sem_avvio, &my_op, 1);


    kill(getpid(), SIGALRM);        /*print del giorno 0*/

    my_op.sem_num = 1;
    my_op.sem_flg = 0;
    my_op.sem_op = (SO_NAVI + SO_PORTI);
    semop(sem_avvio, &my_op, 1);
    



    if((pid_maelstorm = fork()) == 0){
        struct timespec uragano;
        struct sigaction ma;
        int id, status;
        double t1 = (durata_giorno * SO_MAELSTROM / 24.0);
        int t = ((durata_giorno * SO_MAELSTROM) / 24);
        bzero(&sa,sizeof(ma));
        ma.sa_handler = chiudi_maelstorm;
        sigaction(SIGINT, &ma, NULL);
        uragano.tv_sec = (time_t)t;
        uragano.tv_nsec = (long)((double)(t1-t)*1000000000);
        while(1){
            if(conta_quante() != 0){
                nanosleep(&uragano, NULL);
                if(errno == 4)
                    errno = 0;
                do{
                    clock_gettime(CLOCK_REALTIME , &now);
                    id = now.tv_nsec % SO_NAVI;
                }while(shmnavi[id].stato_nave ==-1);
                kill(shmnavi[id].pid, SIGINT);
                TEST_ERROR;
                shmnavi[id].stato_nave = -1;
            }
            else{
                printf("Simulazione terminata perchè non ci sono più navi\n");
                close_all(1);
                kill(getppid(), SIGTERM);
                waitpid(getppid(), &status, WEXITED);
                exit(0);
                /*break;*/
            }
        }
    }
    
    /*IL PROCESSO AVVIA DEGLI ALARM OGNI GIORNO (5 sec) PER STAMPARE UN RESOCONTO DELLA SIMULAZIONE*/
    for (d = SO_DAYS; d && !isRequestEmpty(); d--) {
        alarm(durata_giorno);
        pause();
    }

    close_all(1);
    /*printf("NUMERO DI NAVI CHE SI SONO FERMATE CAUSA TEMPESTA:%d\nNUMERO DI NAVI CHE SI SONO FERMATE CAUSA MAREGGIATA:%d\n", num_tempesta, num_mareggiata);*/
    exit(0);
}

void handle_alarm(int signum) {
    int i;
    int richieste_soddisfatte = 0, num_navi_mare = 0, num_navi_porto = 0, num_navi_scarico = 0, num_navi_carico = 0, carico_tot_navi = 0, carico_da_scaricare = 0, num_navi_tempesta = 0, num_navi_mareggiata = 0, num_navi_affondate = 0;
    *shmgiorno = giorno;
    inizializzazione_fill();
    if(giorno>0){
        tempesta();
        mareggiata();
    }
    accessoPortiNavi();
    for(i=0; i<SO_PORTI; i++){
        carico_da_scaricare += shmporti[i].richiesta.qmerce * shmmerci[shmporti[i].richiesta.idmerce].dimensione; 
        if(shmporti[i].richiesta_soddisfatta == 1)
            richieste_soddisfatte++;
    }
    for(i=0; i<SO_NAVI; i++){
        carico_tot_navi += shmnavi[i].carico_tot;
        switch(shmnavi[i].stato_nave){
            case -1:
                num_navi_affondate++;
                break;
            case 0:
                num_navi_porto++;
                break;
            case 1:
                num_navi_mare++;
                break;
            case 2:
                num_navi_porto++;
                num_navi_scarico++;
                break;
            case 3:
                num_navi_porto++;
                num_navi_carico++;
                break;
            case 4:
                num_navi_tempesta++;
                num_tempesta += num_navi_tempesta;
                break;
            case 5:
                num_navi_porto++;
                num_navi_mareggiata++;
                num_mareggiata += num_navi_mareggiata;
                break;
        }
    }
    printf("giorno:%d\n", giorno);
    for (i = 0;i< SO_PORTI; i++){
        if (giorno > 1) {
            kill(shmporti[i].offerta.pid, SIGUSR1);
        }
    }
    #ifndef STAMPA_MINIMA
    for (i = 0; i < SO_PORTI; i++) {
        printf("porto[%d]\tOFFERTA->merce[%d]:qmerce:%d, data di scadenza:%d\t\tBANCHINE LIBERE:%d\n", shmporti[i].pid, shmporti[i].offerta.idmerce, shmporti[i].offerta.qmerce, shmporti[i].offerta.scadenza, semctl(sem_porto, i, GETVAL));
        printf("porto[%d]\tRICHIESTA->merce[%d]:qmerce:%d", shmporti[i].pid, shmporti[i].richiesta.idmerce, shmporti[i].richiesta.qmerce);
        if (shmporti[i].richiesta_soddisfatta == 1)
            printf("\tRICHIESTA SODDISFATTA");
        printf("\n\n");
    }
    for (i = 0; i < SO_NAVI; i++) {
        if (shmnavi[i].stato_nave == -1)
            printf("nave[%d]\tSTATO: affondata\n", shmnavi[i].pid);
        else if (shmnavi[i].stato_nave == 0)
            printf("nave[%d]\tSTATO: in porto\tCARICO: %d/%d\t\tCORDINATE:(%.2f,%.2f)\n", shmnavi[i].pid, shmnavi[i].carico_tot, SO_CAPACITY, shmnavi[i].x, shmnavi[i].y);
        else if (shmnavi[i].stato_nave == 1)
            printf("nave[%d]\tSTATO: in mare\tCARICO: %d/%d\t\tCORDINATE:(%.2f,%.2f)\n", shmnavi[i].pid, shmnavi[i].carico_tot, SO_CAPACITY, shmnavi[i].x, shmnavi[i].y);
        else if (shmnavi[i].stato_nave == 2)
            printf("nave[%d]\tSTATO: scarico merce\t\t\tCORDINATE:(%.2f,%.2f)\n", shmnavi[i].pid, shmnavi[i].x, shmnavi[i].y);
        else if (shmnavi[i].stato_nave == 3)
            printf("nave[%d]\tSTATO: carico merce\t\t\tCORDINATE:(%.2f,%.2f)\n", shmnavi[i].pid, shmnavi[i].x, shmnavi[i].y);
        else if (shmnavi[i].stato_nave == 4)
            printf("nave[%d]\tSTATO: ferma causa tempesta\t\t\tCARICO: %d/%d\n", shmnavi[i].pid, shmnavi[i].carico_tot, SO_CAPACITY);
        else if (shmnavi[i].stato_nave == 5)
            printf("nave[%d]\tSTATO: ferma causa mareggiata\t\t\tCORDINATE:(%.2f,%.2f)\n", shmnavi[i].pid, shmnavi[i].x, shmnavi[i].y);
    }
    #else
        printf("RICHIESTE SODDISFATTE:%d/%d\n",richieste_soddisfatte, SO_PORTI);
        printf("NAVI IN MARE:%d\nNAVI IN PORTO:%d\nNAVI SCARICO:%d\nNAVI CARICO:%d\nCARICO TOT NAVI:%d\tCARICO DA SODDISFARE:%d\nNAVI FERME CAUSA TEMPESTA:%d\nNAVI FERME CAUSA MAREGGIATA:%d\nNAVI AFFONDATE:%d\n", num_navi_mare, num_navi_porto, num_navi_scarico, num_navi_carico, carico_tot_navi, carico_da_scaricare, num_navi_tempesta, num_navi_mareggiata, num_navi_affondate);
    #endif
    giorno++;
    printf("\n\n");
    uscitaPortiNavi();
}
void close_all(int signum) {
    int i, status;

    for (i = 0; i < SO_NAVI ; i++) {
        if(shmnavi[i].stato_nave != -1){
            shmnavi[i].stato_nave = -1;
            /*kill(shmnavi[i].pid, SIGINT);*/
            waitpid(shmnavi[i].pid, &status, WEXITED);
        }
    }
    for (i = 0; i < SO_PORTI; i++) {
        /*kill(shmporti[i].pid, SIGINT);*/
        waitpid(shmporti[i].pid, &status, WEXITED);
    }

    /*kill(pid_maelstorm, SIGINT);*/
    waitpid(shmnavi[i].pid, &status, WEXITED);



    msgctl(msg_richiesta, IPC_RMID, NULL);
    msgctl(msg_offerta, IPC_RMID, NULL);
    shmctl(idshmporti, IPC_RMID, NULL);
    shmctl(idshmnavi, IPC_RMID, NULL);
    shmctl(idshmgiorno, IPC_RMID, NULL);
    shmctl(idshmmerci, IPC_RMID, NULL);
    shmctl(idshmfill, IPC_RMID, NULL);
    semctl(sem_shmporto, 1, IPC_RMID);
    semctl(sem_shmnave, 1, IPC_RMID);
    semctl(sem_porto, 1, IPC_RMID);
    semctl(sem_avvio, 1, IPC_RMID);
    semctl(sem_ricoff, 1, IPC_RMID);

    printf("\n\nFine del programma\n");
    exit(0);

}

void chiudi_maelstorm(int signum){
    printf("chiusura processo malestorm\n");
    exit(0);
}

void chiudi_maelstorm1(){
    int status;
    printf("chiusura processo malestorm\n");
    kill(pid_maelstorm, SIGKILL);
    waitpid(pid_maelstorm, &status, WEXITED);
}


void inizializzazione_fill() {
    shmfill[0] = (SO_FILL / SO_DAYS) / SO_PORTI;
    shmfill[1] = shmfill[0] / (SO_PORTI - 1) - 1;
    shmfill[2] = SO_FILL / SO_PORTI;
    shmfill[3] = shmfill[2] / (SO_PORTI - 1) - 1;
    shmfill[4] = SO_PORTI;
    shmfill[5] = SO_PORTI;
}

int isRequestEmpty() {
    int id = 0;
    while (id < SO_PORTI) {
        if (shmporti[id].richiesta_soddisfatta == 0) {
            return 0;
        }
        id++;
    }
    return 1;
}

void tempesta(){
    int id;
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    id = now.tv_nsec % SO_NAVI;
    while (shmnavi[id % SO_NAVI].stato_nave != 1){
        id++;
    }
    kill(shmnavi[id % SO_NAVI].pid, SIGUSR2);
}

void mareggiata(){
    int id;
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    id = now.tv_nsec % SO_PORTI;
    kill(shmporti[id].pid, SIGUSR2);
}

int conta_quante(){
    int k = 0, i;
    for(i=0; i<SO_NAVI; i++){
        if(shmnavi[i].stato_nave != -1)
            k++;
    }
    return k;
}

void accessoPortiNavi(){
    int i;
    for(i=0;i<SO_PORTI; i++){
        sem_accesso(sem_shmporto, i);
    }
    for(i=0;i<SO_NAVI; i++){
        sem_accesso(sem_shmnave, i);
    }
}

void uscitaPortiNavi(){
    int i;
    for(i=0;i<SO_PORTI; i++){
        sem_uscita(sem_shmporto, i);
    }
    for(i=0;i<SO_NAVI; i++){
        sem_uscita(sem_shmnave, i);
    }
}