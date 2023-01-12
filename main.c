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
#include <termios.h>

#include "utilities.h"

/*MACRO PER NON METTERE INPUT*/
#define NO_INPUt

#define STAMPA_MINIMA
/*macro che crea un secondo terminale in cui visualizzare stato navi*/
#define SHIP_STATU


int num_tempesta, num_mareggiata;
/*DICHIARAZIONE DELLE VARIABILI GLOBALI DEL MASTER UTILI PER LA SIMULAZIONE*/
pid_t pid_maelstorm;
int SO_NAVI, SO_PORTI,  SO_BANCHINE, SO_CAPACITY, SO_SIZE, SO_FILL, SO_MERCI, SO_DAYS;    /*COSTANTI DELLA SIMULAZIONE*/
int idshmnavi, idshmporti, idshmmerci, idshmgiorno,  idshmfill; /*ID DELLE MEMORIE CONDIVISE*/
int sem_shmporto, sem_shmnave, sem_avvio, sem_porto, sem_ricoff; /*id dei semafori*/
snave* shmnavi; sporto* shmporti; smerce* shmmerci; int* shmgiorno; int* shmfill;
int* id_navi_tempesta , *id_porti_mareggiata;
int no_navi_distrutte = 0, giorno, durata_giorno;
int idshm_uragano;
int* shm_uragano;
int tot_richieste = 0;

void inizializzazione_fill();

int isRequestEmpty();

void tempesta();

void mareggiata();
int conta_quante();
void resoconto();
void chiudi_maelstorm1();

void accessoPortiNavi();
void uscitaPortiNavi();

/*HANDLER DEI VARI SEGNALI*/
void handle_alarm(int signal);
void close_all(int signum);
void chiudi_maelstorm(int signum);
void termina(int signum);

void barraCompletamento(int carico_da_scaricare);

void terminal2(int tty);
int menu();
char getch(void);

int main() {
    /* DICHIARAZIONE DELLE VARIABILI */
    char temp_char;
    int input_lines;
    int input_type, param_config, value, max1 = 0, max2 = 0;
    char stringsem_avvio[13];
    char stringsem_shmporto[13];
    char stringsem_shmnave[13];
    char stringsem_porto[3 * sizeof(sem_porto) + 1];
    char stringporti[3 * sizeof(idshmporti) + 1];
    char stringnavi[3 * sizeof(idshmnavi) + 1];
    char stringfill[13], stringsem_ricoff[13];
    char stringmerci[13], string_semmerci[13];
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
    double t1;
    int t;
    int i, j, d, banchine_effettive;
    double SO_LATO;
    int SO_MIN_VITA, SO_MAX_VITA, SO_VELOCITA, SO_LOADSPEED, SO_SPEED;
    int SO_STORM_DURATION, SO_SWELL_DURATION, SO_MAELSTROM;
    struct sigaction ca;
    struct sigaction sa;
    bzero(&ca, sizeof(ca));
    ca.sa_handler = close_all;
    sigaction(SIGINT, &ca, NULL);
    TEST_ERROR;
    bzero(&sa, sizeof(sa));
    sa.sa_handler = handle_alarm;
    sigaction(SIGALRM, &sa, NULL);
    bzero(&sa, sizeof(sa));
    sa.sa_handler = termina;
    sigaction(SIGTERM, &sa, NULL);
    TEST_ERROR;

    srand(time(NULL));
    num_tempesta = 0;
    num_mareggiata = 0;
    giorno = 0;
    durata_giorno = 2;
    /*INIZIO INPUT*/

#ifndef NO_INPUT/*TO DO: CONTROLLO CHE I PARAMETRI SIANO POSITIVI*/
    /*system("tput smcup");*/

    printf("\n\n\n");

    param_config = menu();

    input_type = 0; /*da cambiare*/

    if (input_type == 0) {

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

        case 5:/*custom*/
            printf("\033[033;33m\n SCENARIO:\033[033;32m TEST\033[033;0m\n");
            SO_NAVI = 15;
            SO_PORTI = 5;
            SO_MERCI = 10;
            SO_SIZE = 1;
            SO_MIN_VITA = 3;
            SO_MAX_VITA = 10;
            SO_LATO = 10;
            SO_SPEED = 20;
            SO_CAPACITY = 300;
            SO_BANCHINE = 10;
            SO_FILL = 1000;
            SO_LOADSPEED = 20;
            SO_DAYS = 10;
            SO_STORM_DURATION = 12;
            SO_SWELL_DURATION = 30;
            SO_MAELSTROM = 36;
            break;
        case 6:/*exit*/
            exit(0);
            break;
        }


    } else if (input_type == 1) {
        printf("\n--------------------->Input Parametri Manuale<---------------------\n");

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
            /*printf("max vita:%d, min vita:%d\n", SO_MAX_VITA, SO_MIN_VITA);*/
        } while (SO_MAX_VITA < SO_MIN_VITA);
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

    t1 = (durata_giorno * SO_MAELSTROM / 24.0);
    t = ((durata_giorno * SO_MAELSTROM) / 24);

    /*CREO LE CODE DI MESSAGGI, I SEMAFORI E LE MEMORIE CONDIVISE*/

    idshmporti = shmget(IPC_PRIVATE, sizeof(*arrayporti) * SO_PORTI, IPC_CREAT | IPC_EXCL | 0600);
    idshmnavi = shmget(IPC_PRIVATE, sizeof(*arraynavi) * SO_NAVI, IPC_CREAT | IPC_EXCL | 0600);
    idshmmerci = shmget(IPC_PRIVATE, sizeof(*arraymerci) * SO_MERCI, IPC_CREAT | IPC_EXCL | 0600);
    idshmgiorno = shmget(IPC_PRIVATE, sizeof(giorno), IPC_CREAT | IPC_EXCL | 0600);
    idshmfill = shmget(IPC_PRIVATE, 6 * 4, IPC_CREAT | IPC_EXCL | 0600);
    idshm_uragano = shmget(IPC_PRIVATE, sizeof(giorno) * ((SO_DAYS / t1) + 1), IPC_CREAT | IPC_EXCL | 0600);
    sem_avvio = semget(IPC_PRIVATE, 2, IPC_CREAT | IPC_EXCL | 0600);
    sem_porto = semget(IPC_PRIVATE, SO_PORTI, IPC_CREAT | IPC_EXCL | 0600);
    sem_shmporto = semget(IPC_PRIVATE, SO_PORTI, IPC_CREAT | IPC_EXCL | 0600);
    sem_shmnave = semget(IPC_PRIVATE, SO_NAVI, IPC_CREAT | IPC_EXCL | 0600);
    sem_ricoff = semget(IPC_PRIVATE, 2, IPC_CREAT | IPC_EXCL | 0600);
    TEST_ERROR;
    shmporti = shmat(idshmporti, NULL, 0);
    shmnavi = shmat(idshmnavi, NULL, 0);
    shmmerci = shmat(idshmmerci, NULL, 0);
    shmgiorno = (int*)shmat(idshmgiorno, NULL, 0);
    shmfill = (int*)shmat(idshmfill, NULL, 0);
    shm_uragano = (int*)shmat(idshm_uragano, NULL, 0);
    semctl(sem_ricoff, 0, SETVAL, 1);   /*richieste*/
    semctl(sem_ricoff, 1, SETVAL, 1);   /*offerte*/
    semctl(sem_avvio, 0 , SETVAL, 0);
    semctl(sem_avvio, 1 , SETVAL, 0);

    printf("\nidshmporti: %d\n\n", idshmporti);
    printf("\033[0m");
    SO_LOADSPEED /= durata_giorno;
    SO_SPEED /= durata_giorno;

    /*ALLOCAZIONE DEGLI ARRAY PER SALVARE GLI ID DEI PORTI E DELLE NAVI CHE SONO STATI COLPITI DA MAREGGIATA O TEMPESTA*/
    id_navi_tempesta = calloc(SO_DAYS, sizeof(*id_navi_tempesta));
    id_porti_mareggiata = calloc(SO_DAYS, sizeof(*id_porti_mareggiata));
    bzero(id_navi_tempesta, sizeof(id_navi_tempesta));
    bzero(id_porti_mareggiata, sizeof(id_porti_mareggiata));
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
    bzero(arraymerci, sizeof(*arraymerci)*SO_MERCI);
    for (i = 0; i < SO_MERCI; i++) {
        arraymerci[i].id = i;
        clock_gettime(CLOCK_REALTIME, &now);
        arraymerci[i].scadenza = (now.tv_nsec % (SO_MAX_VITA - SO_MIN_VITA + 1 )) + SO_MIN_VITA ;
        arraymerci[i].dimensione = (now.tv_nsec % (SO_SIZE)) + 1;
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
        value = fork();
        if (value == -1) {
            TEST_ERROR;
            exit(1);
        }
        if (value == 0) {
            /* CHILD */
            semctl(sem_shmporto, i , SETVAL, 1);
            arrayporti[i].pid = getpid();
            arrayporti[i].richiesta_soddisfatta = 0;
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
            arrayporti[i].banchine = banchine_effettive;
            sem_accesso(sem_shmporto, i);
            shmporti[i] = arrayporti[i];
            sem_uscita(sem_shmporto, i);
            sprintf(stringid, "%d", i);
            porto[4] = stringid;
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
        value = fork();
        if (value == -1) {
            TEST_ERROR;
            exit(1);
        }
        if (value == 0) {
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

            execvp("./nave", nave);
            TEST_ERROR;
            exit(EXIT_FAILURE);
        }
        else {
            /* PARENT */
        }
    }

    /*CREAIAMO UN FUGLIO PER GENERARE IL MAELSTORM*/
    if ((pid_maelstorm = fork()) == 0) {
        struct timespec uragano;
        struct sigaction ma;
        int id, status, i = 0;
        bzero(&sa, sizeof(ma));
        ma.sa_handler = chiudi_maelstorm;
        sigaction(SIGINT, &ma, NULL);
        uragano.tv_sec = (time_t)t;
        uragano.tv_nsec = (long)((double)(t1 - t) * 1000000000);
        sem_uscita(sem_avvio, 0);
        sem_accesso(sem_avvio, 1);
        while (1) {
            if (conta_quante() != 0) {
                nanosleep(&uragano, NULL);
                if (errno == 4)
                    errno = 0;
                do {
                    clock_gettime(CLOCK_REALTIME , &now);
                    id = now.tv_nsec % SO_NAVI;
                } while (shmnavi[id].stato_nave == -1);
                kill(shmnavi[id].pid, SIGINT);
                TEST_ERROR;
                shmnavi[id].stato_nave = -1;
                shm_uragano[i] = id;
                i++;
            }
            else {
                printf("Simulazione terminata perchè non ci sono più navi\n");
                kill(getppid(), SIGTERM);
                waitpid(getppid(), &status, WEXITED);
                free(arraymerci);
                free(arraynavi);
                free(arrayporti);
                close_all(1);
            }
        }
    }

    my_op.sem_num = 0;
    my_op.sem_flg = 0;
    my_op.sem_op = -(SO_PORTI + SO_NAVI + 1);
    semop(sem_avvio, &my_op, 1);

    kill(getpid(), SIGALRM);        /*print del giorno 0*/

    my_op.sem_num = 1;
    my_op.sem_flg = 0;
    my_op.sem_op = (SO_NAVI + SO_PORTI + 1);
    semop(sem_avvio, &my_op, 1);

    /*IL PROCESSO AVVIA DEGLI ALARM OGNI GIORNO (5 sec) PER STAMPARE UN RESOCONTO DELLA SIMULAZIONE*/
    for (d = SO_DAYS; d && !isRequestEmpty(); d--) {
        alarm(durata_giorno);
        pause();
    }

    /*CHIUDIAMO TUTTO E FACCIAMO UN RESOCONTO*/

    chiudi_maelstorm1();
    for (i = 0; i < SO_NAVI; i++) {
        kill(shmnavi[i].pid, SIGSTOP);
    }
    for (i = 0; i < SO_NAVI; i++) {
        if (shmnavi[i].stato_nave == -1)
            no_navi_distrutte++;
    }
    printf("NAVI COLPITE DALLA TEMPESTA:\n");
    for (i = 0; i < SO_DAYS; i++) {
        if (id_navi_tempesta[i] != 0)
            printf("%d\t", id_navi_tempesta[i]);
    }
    printf("\nPORTI COLPITI DALLA MAREGGIATA:\n");
    for (i = 0; i < SO_DAYS; i++) {
        if (id_porti_mareggiata[i] != 0)
            printf("%d\t", id_porti_mareggiata[i]);
    }
    printf("\nNAVI AFFONDATE: %d\n", no_navi_distrutte);
    for (i = 0; i < no_navi_distrutte; i++) {
        printf("%d\t ", shm_uragano[i]);
    }
    printf("\n");
    t = 0;
    d = 0;
    for (i = 0; i < SO_PORTI; i++) {
        printf("PORTO:%d\tLOTTI POSSEDUTI:%d\tMERCI SPEDITE:%d\tMERCI RICEVUTE:%d\n", i, shmporti[i].offerta.qmerce, shmporti[i].spedita, shmporti[i].ricevuta);
        if (shmporti[i].tot_offerta > max1) {
            max1 = shmporti[i].tot_offerta;
            t = i;
        }
        if (shmporti[i].tot_richiesta > max2) {
            max2 = shmporti[i].tot_richiesta;
            d = i;
        }
    }
    for (i = 0; i < SO_MERCI; i++) {
        printf("MERCE:%d\tQUANTITA' GENERATA:%d\tQUANTITA' DI MERCE SCADUTA:%d\tQUANTITA' DI MERCE CONSEGNATA:%d\tQUANTITA' DI MERCE FERMA NEI PORTI:%d\n", i, (shmmerci[i].totale * shmmerci[i].dimensione), (shmmerci[i].scaduta_nave * shmmerci[i].dimensione), (shmmerci[i].consegnata * shmmerci[i].dimensione), shmmerci[i].q_ferma);
    }
    printf("PORTO CHE HA GENERA PIU' MERCE:%d\tPORTO CHE HA RICHIESTO PIU' MERCI: %d", t, d);
    printf("\n");
    free(id_navi_tempesta);
    free(id_porti_mareggiata);
    free(arraymerci);
    free(arraynavi);
    free(arrayporti);
    close_all(1);
    exit(0);
}

void handle_alarm(int signum) {
    int i, j = 0;
    int richieste_soddisfatte = 0, num_navi_mare_carico = 0, num_navi_mare_senza = 0, num_navi_porto = 0, num_navi_scarico = 0, num_navi_carico = 0, carico_tot_navi = 0, carico_da_scaricare = 0, num_navi_tempesta = 0, num_navi_mareggiata = 0, num_navi_affondate = 0;
    *shmgiorno = giorno;
    inizializzazione_fill();
    if (giorno > 0) {
        tempesta();
        mareggiata();
    }
    /*accessoPortiNavi();*/
    for (i = 0; i < SO_MERCI; i++) {
        printf("Della merce %d, presso i porti è presente una quantità pari a %d ton, presso le navi di %d ton, ne sono state consegnate ai porti %d tonnellate e %d lotti sono scaduti in mare\n\n\n", shmmerci[i].id, (shmmerci[i].pres_porto * shmmerci[i].dimensione), (shmmerci[i].pres_na * shmmerci[i].dimensione), (shmmerci[i].consegnata * shmmerci[i].dimensione), shmmerci[i].scaduta_nave);
    }
    for (i = 0; i < SO_PORTI; i++) {
        carico_da_scaricare += shmporti[i].richiesta.qmerce * shmmerci[shmporti[i].richiesta.idmerce].dimensione;
        if (giorno == 0)
            tot_richieste = carico_da_scaricare;
        if (shmporti[i].richiesta_soddisfatta == 1)
            richieste_soddisfatte++;
    }
    for (i = 0; i < SO_NAVI; i++) {
        carico_tot_navi += shmnavi[i].carico_tot;
        switch (shmnavi[i].stato_nave) {
        case -1:
            num_navi_affondate++;
            break;
        case 0:
            num_navi_porto++;
            break;
        case 1:
            if (shmnavi[i].carico_tot == 0)
                num_navi_mare_senza++;
            else
                num_navi_mare_carico++;
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
#ifdef SHIP_STATUS
    terminal2(1);
#endif
    for (i = 0; i < SO_PORTI; i++) {
        if (giorno > 1) {
            kill(shmporti[i].offerta.pid, SIGUSR1);
        }
    }
#ifndef STAMPA_MINIMA

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
    for (i = 0; i < SO_PORTI; i++) {
        printf("porto[%d]\tOFFERTA->merce[%d]:qmerce:%d, data di scadenza:%d\t\tBANCHINE LIBERE:%d\\%d\n", shmporti[i].pid, shmporti[i].offerta.idmerce, shmporti[i].offerta.qmerce, shmporti[i].offerta.scadenza, semctl(sem_porto, i, GETVAL), shmporti[i].banchine);
        printf("porto[%d]\tRICHIESTA->merce[%d]:qmerce:%d", shmporti[i].pid, shmporti[i].richiesta.idmerce, shmporti[i].richiesta.qmerce);
        if (shmporti[i].richiesta_soddisfatta == 1)
            printf("\tRICHIESTA SODDISFATTA");
        printf("\n\n");
    }
#else
    printf("RICHIESTE SODDISFATTE:%d/%d\n", richieste_soddisfatte, SO_PORTI);
    printf("NAVI IN MARE SENZA CARICO:%d\nNAVI IN MARE CON CARICO:%d\nNAVI IN PORTO:%d\nNAVI SCARICO:%d\nNAVI CARICO:%d\nCARICO TOT NAVI:%d\tCARICO DA SODDISFARE:%d\nNAVI FERME CAUSA TEMPESTA:%d\nNAVI FERME CAUSA MAREGGIATA:%d\nNAVI AFFONDATE:%d\n", num_navi_mare_senza, num_navi_mare_carico , num_navi_porto, num_navi_scarico, num_navi_carico, carico_tot_navi, carico_da_scaricare, num_navi_tempesta, num_navi_mareggiata, num_navi_affondate);
#endif
    barraCompletamento(carico_da_scaricare);
    giorno++;
    printf("\n\n");
    /*uscitaPortiNavi();*/
}
void close_all(int signum) {
    int i, status;

    for (i = 0; i < SO_NAVI ; i++) {
        if (shmnavi[i].stato_nave != -1) {
            shmnavi[i].stato_nave = -1;
            kill(shmnavi[i].pid, SIGINT);
            waitpid(shmnavi[i].pid, &status, WEXITED);
        }
    }
    for (i = 0; i < SO_PORTI; i++) {
        kill(shmporti[i].pid, SIGINT);
        waitpid(shmporti[i].pid, &status, WEXITED);
    }

    shmctl(idshmporti, IPC_RMID, NULL);
    shmctl(idshmnavi, IPC_RMID, NULL);
    shmctl(idshmgiorno, IPC_RMID, NULL);
    shmctl(idshmmerci, IPC_RMID, NULL);
    shmctl(idshmfill, IPC_RMID, NULL);
    shmctl(idshm_uragano, IPC_RMID, NULL);
    semctl(sem_shmporto, 1, IPC_RMID);
    semctl(sem_shmnave, 1, IPC_RMID);
    semctl(sem_porto, 1, IPC_RMID);
    semctl(sem_avvio, 1, IPC_RMID);
    semctl(sem_ricoff, 1, IPC_RMID);



    printf("\n\nFine del programma\n");
    exit(0);
}

void chiudi_maelstorm(int signum) {
    exit(0);
}

void chiudi_maelstorm1() {
    int status;
    kill(pid_maelstorm, SIGKILL);
    waitpid(pid_maelstorm, &status, WEXITED);
}

void termina(int signum) {
    int i;
    for (i = 0; i < SO_NAVI; i++) {
        if (shmnavi[i].stato_nave == -1)
            no_navi_distrutte++;
    }
    printf("In tutto, sono state colpite da tempesta le navi\n");
    for (i = 0; i < SO_DAYS; i++) {
        if (id_navi_tempesta[i] != 0)
            printf("%d\t", id_navi_tempesta[i]);
    }
    printf("\ne sono stati colpiti da mareggiata i porti\n");
    for (i = 0; i < SO_DAYS; i++) {
        if (id_porti_mareggiata[i] != 0)
            printf("%d\t", id_porti_mareggiata[i]);
    }
    printf("\ne sono state distrutte le navi %d\n", no_navi_distrutte);
    for (i = 0; i < no_navi_distrutte; i++) {
        printf("%d\t ", shm_uragano[i]);
    }
    printf("\n");
    exit(0);
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

void tempesta() {
    int id;
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    id = now.tv_nsec % SO_NAVI;
    while (shmnavi[id % SO_NAVI].stato_nave != 1) {
        id++;
    }
    id_navi_tempesta[giorno - 1] = id;
    kill(shmnavi[id % SO_NAVI].pid, SIGUSR2);
}

void mareggiata() {
    int id;
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    id = now.tv_nsec % SO_PORTI;
    id_porti_mareggiata[giorno - 1] = id;
    kill(shmporti[id].pid, SIGUSR2);
}

int conta_quante() {
    int k = 0, i;
    for (i = 0; i < SO_NAVI; i++) {
        if (shmnavi[i].stato_nave != -1)
            k++;
    }
    return k;
}

void accessoPortiNavi() {
    int i;
    for (i = 0; i < SO_PORTI; i++) {
        sem_accesso(sem_shmporto, i);
    }
    for (i = 0; i < SO_NAVI; i++) {
        sem_accesso(sem_shmnave, i);
    }
}

void uscitaPortiNavi() {
    int i;
    for (i = 0; i < SO_PORTI; i++) {
        sem_uscita(sem_shmporto, i);
    }
    for (i = 0; i < SO_NAVI; i++) {
        sem_uscita(sem_shmnave, i);
    }
}

void barraCompletamento(int carico_da_scaricare) {
    int i;
    double percentuale_completamento = (1 - (double)carico_da_scaricare / (double)tot_richieste) * 10;
    printf("PERCENTUALE DI MERCI SCARICATE:%.2f", percentuale_completamento * 10);
    printf("[");
    for (i = 0; i < 10; i++) {
        if (percentuale_completamento > i)
            printf("===");
        else
            printf("   ");
    }
    printf("]\n");
}

void terminal2(int tty) {
    int i;
    FILE *fp = fopen("/dev/pts/1", "w");
    /*fprintf(fp, "\x1B[2J");*/
    fprintf(fp, "\x1b[H\x1b[2J");
    fprintf(fp, "\nSTATO NAVI (giorno %d)\n", giorno);
    for (i = 0; i < SO_NAVI; i++) {
        switch (shmnavi[i].stato_nave) {
        case -1:    /*RED = affondata*/
            fprintf(fp, "\033[033;31m◉ ");

            break;
        case 0:     /*WHITE = in porto*/
            fprintf(fp, "\033[033;37m◉ ");
            break;
        case 1:     /*BLUE = in mare*/
            fprintf(fp, "\033[033;34m◉ ");
            break;
        case 2:     /*GREEN = scarico in porto*/
            fprintf(fp, "\033[033;32m◉ ");
            break;
        case 3:     /*YELLOW = carico in nave*/
            fprintf(fp, "\033[033;33m◉ ");
            break;
        case 4:     /*LIGHT BLUE = tempesta*/
            fprintf(fp, "\033[033;36m◉ ");
            break;
        case 5:     /*MAGENTA = mareggiata in porto*/
            fprintf(fp, "\033[033;35m◉ ");
            break;
        }
    }
    fprintf(fp, "\033[0m");
    fprintf(fp, "\nROSSO = AFFONDATA\nBIANCO = IN PORTO\nBLU = IN MARE\nVERDE = SCARICO IN PORTO\nGIALLO = CARICO IN PORTO\nAZZURRO = TEMPESTA\nMAGENTA = MAREGGIATA IN PORTO");
    fclose(fp);
}

int menu() {
    int key = 0;
    int option = 0;
    int max_options = 6;

    static struct termios oldt, newt;

    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON);
    /*new stdin settings*/
    system("tput smcup");
    system("tput civis");
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);


    /*
    system("clear");
    */

    while ((int)key != 10) {
        system("clear");


        switch (option) {

        case 0:
            printf("\033[033;43m\033[033;39m\n\n\n");
            printf("\t╔═══════════════════════════════════════╗\n");
            printf("\t║                \033[033;39mSCENARI                ║\n");
            printf("\t╠═══════════════════════════════════════╣\n");
            printf("\t║\033[033;30m  [0] Dense, Small Ships \033[5m←\033[0;43m             ║\n");
            printf("\t║ [1] Dense, Small Ships + Trashing     ║\n");
            printf("\t║ [2] Born To Run                       ║\n");
            printf("\t║ [3] Cargos, Big Stuff                 ║\n");
            printf("\t║ [4] Unlucky Cargos                    ║\n");
            printf("\t║ [5] Custom                            ║\n");
            printf("\t║\033[033;31m [6] Exit\033[033;39m                              ║\n");
            printf("\t╚═══════════════════════════════════════╝\033[034;0m\n");
            break;

        case 1:
            printf("\033[033;43m\033[033;39m\n\n\n\t╔═══════════════════════════════════════╗\n");
            printf("\t║                \033[033;39mSCENARI                ║\n");
            printf("\t╠═══════════════════════════════════════╣\n");
            printf("\t║ [0] Dense, Small Ships                ║\n");
            printf("\t║\033[033;30m  [1] Dense, Small Ships + Trashing \033[5m←\033[0;43m  ║\n");
            printf("\t║ [2] Born To Run                       ║\n");
            printf("\t║ [3] Cargos, Big Stuff                 ║\n");
            printf("\t║ [4] Unlucky Cargos                    ║\n");
            printf("\t║ [5] Custom                            ║\n");
            printf("\t║\033[033;31m [6] Exit\033[033;39m                              ║\n");
            printf("\t╚═══════════════════════════════════════╝\033[033;0m\n");
            break;

        case 2:
            printf("\033[033;43m\033[033;39m\n\n\n\t╔═══════════════════════════════════════╗\n");
            printf("\t║                \033[033;39mSCENARI                ║\n");
            printf("\t╠═══════════════════════════════════════╣\n");
            printf("\t║ [0] Dense, Small Ships                ║\n");
            printf("\t║ [1] Dense, Small Ships + Trashing     ║\n");
            printf("\t║\033[033;30m  [2] Born To Run \033[5m←\033[0;43m                    ║\n");
            printf("\t║ [3] Cargos, Big Stuff                 ║\n");
            printf("\t║ [4] Unlucky Cargos                    ║\n");
            printf("\t║ [5] Custom                            ║\n");
            printf("\t║\033[033;31m [6] Exit\033[033;39m                              ║\n");
            printf("\t╚═══════════════════════════════════════╝\033[033;0m\n");
            break;

        case 3:
            printf("\033[033;43m\033[033;39m\n\n\n\t╔═══════════════════════════════════════╗\n");
            printf("\t║                \033[033;39mSCENARI                ║\n");
            printf("\t╠═══════════════════════════════════════╣\n");
            printf("\t║ [0] Dense, Small Ships                ║\n");
            printf("\t║ [1] Dense, Small Ships + Trashing     ║\n");
            printf("\t║ [2] Born To Run                       ║\n");
            printf("\t║\033[033;30m  [3] Cargos, Big Stuff \033[5m←\033[0;43m              ║\n");
            printf("\t║ [4] Unlucky Cargos                    ║\n");
            printf("\t║ [5] Custom                            ║\n");
            printf("\t║\033[033;31m [6] Exit\033[033;39m                              ║\n");
            printf("\t╚═══════════════════════════════════════╝\033[033;0m\n");
            break;

        case 4:
            printf("\033[033;43m\033[033;39m\n\n\n\t╔═══════════════════════════════════════╗\n");
            printf("\t║                \033[033;39mSCENARI                ║\n");
            printf("\t╠═══════════════════════════════════════╣\n");
            printf("\t║ [0] Dense, Small Ships                ║\n");
            printf("\t║ [1] Dense, Small Ships + Trashing     ║\n");
            printf("\t║ [2] Born To Run                       ║\n");
            printf("\t║ [3] Cargos, Big Stuff                 ║\n");
            printf("\t║\033[033;30m  [4] Unlucky Cargos \033[5m←\033[0;43m                 ║\n");
            printf("\t║ [5] Custom                            ║\n");
            printf("\t║\033[033;31m [6] Exit\033[033;39m                              ║\n");
            printf("\t╚═══════════════════════════════════════╝\033[033;0m\n");
            break;

        case 5:
            printf("\033[033;43m\033[033;39m\n\n\n\t╔═══════════════════════════════════════╗\n");
            printf("\t║                \033[033;39mSCENARI                ║\n");
            printf("\t╠═══════════════════════════════════════╣\n");
            printf("\t║ [0] Dense, Small Ships                ║\n");
            printf("\t║ [1] Dense, Small Ships + Trashing     ║\n");
            printf("\t║ [2] Born To Run                       ║\n");
            printf("\t║ [3] Cargos, Big Stuff                 ║\n");
            printf("\t║ [4] Unlucky Cargos                    ║\n");
            printf("\t║\033[033;30m  [5] Custom \033[5m←\033[0;43m                         ║\n");
            printf("\t║\033[033;31m [6] Exit\033[033;39m                              ║\n");
            printf("\t╚═══════════════════════════════════════╝\033[033;0m\n");
            break;

        case 6:
            printf("\033[033;43m\033[033;39m\n\n\n\t╔═══════════════════════════════════════╗\n");
            printf("\t║                \033[033;39mSCENARI                ║\n");
            printf("\t╠═══════════════════════════════════════╣\n");
            printf("\t║ [0] Dense, Small Ships                ║\n");
            printf("\t║ [1] Dense, Small Ships + Trashing     ║\n");
            printf("\t║ [2] Born To Run                       ║\n");
            printf("\t║ [3] Cargos, Big Stuff                 ║\n");
            printf("\t║ [4] Unlucky Cargos                    ║\n");
            printf("\t║ [5] Custom                            ║\n");
            printf("\t║\033[033;31m  [6] Exit \033[5m←\033[0;43m                           ║\n");
            printf("\t╚═══════════════════════════════════════╝\033[033;0m\n");
            break;

        }

        printf("\n\n\033[033;36m\t\ttasti:\t A = ↑\t W = ↓\033[0m");


        key = getchar();

        if (key == 's' && option < max_options) {
            option++;
        } else if (key == 'w' && option > 0) {
            option--;
        }


    }
    /*restore old settings*/
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
    system("tput cnorm");
    system("tput rmcup");

    return option;
}


char getch(void)
{
    char buf = 0;
    struct termios old = {0};
    fflush(stdout);
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    printf("%c\n", buf);
    return buf;
}