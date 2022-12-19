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
/*MACRO PER LA VELOCITA DELLE NAVI E LA CAPACITA*/



/*DICHIARAZIONE DELLE VARIABILI GLOBALI DEL MASTER UTILI PER LA SIMULAZIONE*/
pid_t* na;  /*array con i Pid delle navi*/
pid_t* po;  /*array con i Pid dei porti*/
int SO_NAVI, SO_PORTI, msg_richiesta, msg_offerta, SO_BANCHINE, SO_SIZE, SO_FILL, SO_DAYS;
int idshmnavi, idshmporti, idshmmerci, idshmgiorno,  idshmfill;
int sem_shmporto;int sem_shmnave; int sem_avvio;/*id del semaforo che permette l'accesso alla shm*/
int sem_porto, sem_ricoff;/*semaforo per far approdare le navi al porto*/
snave* shmnavi; sporto* shmporti; smerce* shmmerci;int giorno;int* shmgiorno;int* shmfill;

void inizializzazione_fill();

/*HANDLER DEI VARI SEGNALI*/
void handle_alarm(int signal);
void close_all(int signum);

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
    char stringrichiesta[13];
    char stringofferta[13];
    char stringvelocita[13], stringcapacity[13], stringgiorno[13], stringload_speed[13], stringnum_merci[13];
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
    int SO_CAPACITY;
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

    srand(time(NULL));

    giorno = 0;
    /*INIZIO INPUT*/
    
#ifndef NO_INPUT/*TO DO: CONTROLLO CHE I PARAMETRI SIANO POSITIVI*/
    printf("\033[033;33m\n╔═══════════════════════════════════╗\n║ [0] Scelta Scenario               ║\n║ [1] Input Parametri Manuale       ║\n╚═══════════════════════════════════╝\n\n\n\n\033[033;0m");
    do {
        rm_lines_terminal(4);
        printf("\nScegliere modalità di input\n");
        scanf("%d", &input_type);
    } while (input_type != 0 && input_type != 1);
    rm_lines_terminal(8);

    if (input_type == 0) {
        printf("\033[033;33m\n╔═══════════════════════════════════╗\n║              SCENARI              ║\n");

        printf("╠═══════════════════════════════════╣\n║ [0] Dense, Small Ships            ║\n║ [1] Dense, Small Ships + Trashing ║\n║ [2] Born To Run                   ║\n║ [3] Cargos, Big Stuff             ║\n║ [4] Unlucky Cargos                ║\n╚═══════════════════════════════════╝\033[033;0m\n\n\n\n");

        do {
            rm_lines_terminal(4);
            printf("\nScegliere scenario\n");
            scanf("%d", &param_config);
        } while (param_config != 0 && param_config != 1 && param_config != 2 && param_config != 3 && param_config != 4);
        rm_lines_terminal(13);

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
            SO_STORM_DURATION = 6;
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
            SO_SPEED = 500;
            SO_CAPACITY = 1000;
            SO_BANCHINE = 10;
            SO_FILL = 1000000;
            SO_LOADSPEED = 200;
            SO_DAYS = 10;
            SO_STORM_DURATION = 12;
            SO_SWELL_DURATION = 10;
            SO_MAELSTROM = 1;
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
        printf("inserisci il carico massimo trasportabile delle navi: ");
        scanf("%d", &SO_CAPACITY);
        printf("inserisci la velocita' delle navi: ");
        scanf("%d", &SO_VELOCITA);
        printf("inserisci la dimensione massima della merce: ");
        scanf("%d", &SO_SIZE);
        printf("inserisci il numero di merci utilzzabi: ");
        scanf("%d", &SO_MERCI);
        printf("inserisci il minimo di giorni di vita delle merci: ");
        scanf("%d", &SO_MIN_VITA);
        printf("inserisci il massimo di giorni di vita delle merci: ");
        scanf("%d", &SO_MAX_VITA);
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

#endif

#ifdef NO_INPUT
    SO_LATO = 1000;   /*(n > 0) !di tipo double!*/
    SO_NAVI = 10;    /*(n >= 1)*/
    SO_PORTI = 10;   /*(n >= 4)*/
    SO_BANCHINE = 2;
    SO_MERCI = 1;
    SO_SIZE = 1;
    SO_CAPACITY = 10;
    SO_VELOCITA = 500;
    SO_MAX_VITA = 50;
    SO_MIN_VITA = 50;
    SO_LOADSPEED = 200;
    SO_FILL = 500000;
    SO_DAYS = 10;
#endif
    /*FINE INPUT*/

    /*INIZIO MENU*/
    printf("\nSO_LATO = %.2f", SO_LATO);
    printf("\nSO_NAVI = %d", SO_NAVI);
    printf("\nSO_PORTI = %d", SO_PORTI);
    
    /*CREO LE CODE DI MESSAGGI, I SEMAFORI E LE MEMORIE CONDIVISE*/
    msg_richiesta = msgget(IPC_PRIVATE, IPC_NOWAIT| IPC_CREAT | IPC_EXCL | 0666);
    msg_offerta = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | 0666);
    sem_porto = semget(IPC_PRIVATE, SO_PORTI, IPC_CREAT | IPC_EXCL | 0600);
    idshmporti = shmget(IPC_PRIVATE, sizeof(*arrayporti) * SO_PORTI, IPC_CREAT | IPC_EXCL | 0600);
    idshmnavi = shmget(IPC_PRIVATE, sizeof(*arraynavi) * SO_NAVI, IPC_CREAT | IPC_EXCL | 0600);
    idshmmerci = shmget(IPC_PRIVATE, sizeof(arraymerci), IPC_CREAT | IPC_EXCL | 0600);
    idshmgiorno = shmget(IPC_PRIVATE, sizeof(giorno), IPC_CREAT | IPC_EXCL | 0600);
    idshmfill = shmget(IPC_PRIVATE, 6*4, IPC_CREAT | IPC_EXCL | 0600);
    sem_shmporto = semget(IPC_PRIVATE, SO_PORTI, IPC_CREAT | IPC_EXCL | 0600); 
    sem_shmnave = semget(IPC_PRIVATE, SO_NAVI, IPC_CREAT | IPC_EXCL | 0600);
    sem_avvio = semget(IPC_PRIVATE,2, IPC_CREAT | IPC_EXCL | 0600); 
    sem_ricoff = semget(IPC_PRIVATE, 2, IPC_CREAT | IPC_EXCL | 0600);
    TEST_ERROR;
    shmporti = shmat(idshmporti, NULL, 0);
    shmnavi = shmat(idshmnavi, NULL, 0);
    shmmerci = shmat(idshmmerci, NULL, 0);
    shmgiorno = (int*)shmat(idshmgiorno, NULL, 0);
    shmfill = (int*)shmat(idshmfill, NULL, 0);
    semctl(sem_ricoff, 0, SETVAL, 1);   /*richieste*/
    semctl(sem_ricoff, 1, SETVAL, 1);   /*offerte*/
    STAMPA_ROSSO(printf("pid:%d, sem_fillval = %d", getpid(), semctl(sem_ricoff, 0, GETVAL, NULL)));
    semctl(sem_avvio, 0 , SETVAL, 0);
    semctl(sem_avvio, 1 , SETVAL, 0);
    /*ALLOCAZIONE DELLA MEMORIA PER GLI ARRAY DEI PID DEI FIGLI*/
    na = calloc(SO_NAVI, sizeof(*na));
    po = calloc(SO_PORTI, sizeof(*po));
    printf("\nidshmporti: %d\n\n", idshmporti);
    printf("\033[0m");
    /*FINE MENU*/
    sprintf(stringsem_avvio, "%d", sem_avvio);
    sprintf(stringsem_porto, "%d", sem_porto);
    sprintf(stringsem_shmporto, "%d", sem_shmporto);
    sprintf(stringsem_shmnave, "%d", sem_shmnave);
    sprintf(stringporti, "%d", idshmporti);
    sprintf(stringnavi, "%d", idshmnavi);
    sprintf(stringmerci, "%d", idshmmerci);
    sprintf(stringfill, "%d", idshmfill);
    sprintf(stringrichiesta, "%d", msg_richiesta);
    sprintf(stringofferta, "%d", msg_offerta);
    sprintf(stringcapacity, "%d", SO_CAPACITY);
    sprintf(stringvelocita, "%d", SO_VELOCITA);
    sprintf(stringgiorno, "%d", idshmgiorno);
    sprintf(stringload_speed, "%d", SO_LOADSPEED);
    sprintf(stringnum_merci, "%d", SO_MERCI);
    sprintf(stringsem_ricoff, "%d", sem_ricoff);
    /*PORTO*/
    porto[1] = stringsem_shmporto;
    porto[2] = stringporti;
    porto[3] = stringnavi;
    porto[5] = stringsem_porto;
    porto[6] = stringrichiesta;
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
    nave[8] = stringrichiesta;
    nave[9] = stringofferta;
    nave[10] = stringgiorno;
    nave[11] = stringload_speed;
    nave[12] = stringmerci;
    nave[13] = stringsem_shmnave;
    nave[14] = stringsem_avvio;
    nave[15] = stringfill;
    nave[16] = NULL;

    /*INIZIALIZZAZIONE SEMAFORO FIRST*/
    TEST_ERROR;

    inizializzazione_fill();

    *shmgiorno = giorno;
    /*ALLOCAZIONE DELLA MEMOIRA E CREAZIONE DELLE MERCI*//*ALLOCAZIONE DELLA MEMOIRA E CREAZIONE DELLE MERCI*/
    arraymerci = calloc(SO_MERCI, sizeof(*arraymerci));
    for(i=0; i<SO_MERCI; i++){
        arraymerci[i].id = i;
        clock_gettime(CLOCK_REALTIME, &now);
        arraymerci[i].scadenza = (now.tv_nsec % (SO_MAX_VITA-SO_MIN_VITA +1 )) + SO_MIN_VITA ;
        arraymerci[i].dimensione = (now.tv_nsec % (SO_SIZE*100000))/100000 + 1;
        clock_gettime(CLOCK_REALTIME, &now);
        printf("merce[%d]:\tSCADENZA:%d\tDIMENSIONE:%d\n",i, arraymerci[i].scadenza,arraymerci[i].dimensione);
        shmmerci[i] = arraymerci[i];
    }


    /*sinfo = semctl(sem_shmporto, 0, SEM_INFO);*/

    printf("\n\n\nsem_shmporti:%d\tsem_shmnavi:%d\tsem_avvio:%d\tsem_banchine:%d\n\n\n",sem_shmporto, sem_shmnave, sem_avvio, sem_porto);
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
            sem_accesso(sem_shmporto,i);
            shmporti[i] = arrayporti[i];
            sem_uscita(sem_shmporto,i);
            sprintf(stringid,"%d",i);
            porto[4] = stringid;
            /*my_op.sem_num = 0;
            my_op.sem_op = 1;
            semop(sem_avvio,&my_op,1);*/
            printf("creazione porto[%d], di pid:%d con coordinate x=%.2f, y=%.2f, con %d banchine\n\n", i, arrayporti[i].pid, arrayporti[i].x, arrayporti[i].y, banchine_effettive);

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
            sem_accesso(sem_shmnave,i);
            shmnavi[i] = arraynavi[i];
            sem_uscita(sem_shmnave,i);
            sprintf(stringid,"%d",i);
            nave[6] = stringid;
            my_op.sem_num = 0;
            my_op.sem_op = 1;
            semop(sem_avvio,&my_op,1);
            printf("creazione nave[%d], di pid:%d con coordinate x=%.2f, y=%.2f\n\n",i, arraynavi[i].pid, arraynavi[i].x, arraynavi[i].y);
            my_op.sem_num = 1;
            my_op.sem_op = -1;
            semop(sem_avvio,&my_op,1);
            
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
    my_op.sem_op = -(SO_PORTI+SO_NAVI);
    semop(sem_avvio, &my_op, 1);


    kill(getpid(), SIGALRM);        /*print del giorno 0*/
    
    my_op.sem_num = 1;
    my_op.sem_flg = 0;
    my_op.sem_op = (SO_NAVI+SO_PORTI);
    semop(sem_avvio, &my_op, 1);

    /*printf("\n\nHO PASSATO IL PRIMO SEMAFORO\n\n");*/

    /*IL PROCESSO AVVIA DEGLI ALARM OGNI GIORNO (5 sec) PER STAMPARE UN RESOCONTO DELLA SIMULAZIONE*/
    for(d= SO_DAYS; d; d--){
        alarm(5);
        pause();
    }

    close_all(1);

    /*for(i=0;i < SO_PORTI;i++){
        kill(arrayporti[i].pid, SIGINT);
        wait(&status);
    }

    for(i=0;i < SO_NAVI ;i++){
        kill(arraynavi[i].pid, SIGINT);
        wait(&status);
    }
    
    msgctl(msg_richiesta,IPC_RMID,NULL);
    msgctl(msg_offerta,IPC_RMID,NULL);
    shmctl(idshmporti, IPC_RMID, NULL);
    shmctl(idshmnavi,IPC_RMID,NULL);
    shmctl(idshmgiorno, IPC_RMID, NULL);
    shmctl(idshmmerci, IPC_RMID, NULL);
    shmctl(idshmfill, IPC_RMID, NULL);
    semctl(sem_shmporto,1,IPC_RMID);
    semctl(sem_shmnave, 1, IPC_RMID);
    semctl(sem_porto,1,IPC_RMID);
    semctl(sem_avvio, 1, IPC_RMID);
    semctl(sem_ricoff, 1, IPC_RMID);*/

    printf("\n\nFine del programma\n");
    exit(0);
}

void handle_alarm(int signum) {
    int i;
    *shmgiorno = giorno;
    inizializzazione_fill();
    printf("giorno:%d\n",giorno);
    for(i = 0; i<SO_PORTI; i++){
        if(giorno > 1){
            kill(shmporti[i].offerta.pid, SIGUSR1);
        }
        if(shmporti[i].richiesta_soddisfatta==0)
            printf("porto[%d]\tOFFERTA->merce[%d]:qmerce:%d, data di scadenza:%d\t\tBANCHINE LIBERE:%d\n",shmporti[i].pid, shmporti[i].offerta.idmerce, shmporti[i].offerta.qmerce, shmporti[i].offerta.scadenza, semctl(sem_porto, i, GETVAL));
        else
            printf("porto[%d]\tOFFERTA->merce[%d]:qmerce:%d, data di scadenza:%d\t\tBANCHINE LIBERE:%d\tRICHIESTA SODDISFATTA\n",shmporti[i].pid, shmporti[i].offerta.idmerce, shmporti[i].offerta.qmerce, shmporti[i].offerta.scadenza, semctl(sem_porto, i, GETVAL));
    }
    for(i = 0; i<SO_NAVI; i++){
        if(shmnavi[i].stato_nave == 0)
            printf("nave[%d]\tSTATO: in porto\tCARICO TOT: %d\t\tCORDINATE:(%.2f,%.2f)\n",shmnavi[i].pid,shmnavi[i].carico_tot,shmnavi[i].x,shmnavi[i].y);
        else if(shmnavi[i].stato_nave == 1)
            printf("nave[%d]\tSTATO: in mare\tCARICO TOT: %d\t\tCORDINATE:(%.2f,%.2f)\n",shmnavi[i].pid,shmnavi[i].carico_tot,shmnavi[i].x,shmnavi[i].y);
        else
            printf("nave[%d]\tSTATO: carico/scarico\tCORDINATE:(%.2f,%.2f)\n",shmnavi[i].pid,shmnavi[i].x,shmnavi[i].y);
    }
    giorno++;
    msg_print_stats(1, msg_richiesta);
    printf("\n\n\n\n");
}
void close_all(int signum) {
    int i, status;

    for(i=0;i < SO_PORTI;i++){
        kill(shmporti[i].pid, SIGINT);
    }

    for(i=0;i < SO_NAVI ;i++){
        kill(shmnavi[i].pid, SIGINT);
    }

    msgctl(msg_richiesta,IPC_RMID,NULL);
    msgctl(msg_offerta,IPC_RMID,NULL);
    shmctl(idshmporti, IPC_RMID, NULL);
    shmctl(idshmnavi,IPC_RMID,NULL);
    shmctl(idshmgiorno, IPC_RMID, NULL);
    shmctl(idshmmerci, IPC_RMID, NULL);
    shmctl(idshmfill, IPC_RMID, NULL);
    semctl(sem_shmporto,1,IPC_RMID);
    semctl(sem_shmnave, 1, IPC_RMID);
    semctl(sem_porto,1,IPC_RMID);
    semctl(sem_avvio, 1, IPC_RMID);
    semctl(sem_ricoff, 1, IPC_RMID);
    for(i=0;i <SO_NAVI + SO_PORTI;i++){
        wait(&status);
    }
    printf("\n\nFine del programma\n");
    exit(0);
    
}


void inizializzazione_fill(){
    shmfill[0] = (SO_FILL/SO_DAYS)/SO_PORTI;
    shmfill[1] = shmfill[0]/(SO_PORTI - 1) -1;
    shmfill[2] = SO_FILL;
    shmfill[3] = SO_FILL;
    shmfill[4] = SO_PORTI;
    shmfill[5] = SO_PORTI;
}

