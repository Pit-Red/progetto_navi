#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <math.h>
#include "utilities.h"

int id;
int sem_shmporto, sem_shmnave, sem_fill, sem_porto;
sporto* shmporti; smerce* shmmerci; snave* shmnavi;
int* shmgiorno;
int SO_MERCI, SO_NAVI, id_merce_richiesta;
int* shmfill;

void nuova_richiesta();
void nuova_offerta();
carico creazione_offerta(int qmerce);
carico creazione_richiesta(int qmerce);

/*HANDLER PER GESTIRE IL SEGNAÒLE DI TERMINAZIONE DEL PADRE*/
void handle_signal(int signum) {
    exit(EXIT_SUCCESS);
}
void nuova_offerta_handler(int signum);
void handler_mareggiata(int sigunum);


int main(int argc, char** argv) {
    /*DICHIARAZIONE DELLE VARIABILI*/
    int sem_avvio;
    struct sigaction sa;
    struct timespec now;
    bzero(&sa, sizeof(sa));
    sa.sa_handler = nuova_offerta_handler;
    sigaction(SIGUSR1, &sa, NULL);
    bzero(&sa, sizeof(sa));
    sa.sa_handler = handler_mareggiata;
    sigaction(SIGUSR2, &sa, NULL);
    TEST_ERROR;
    shmporti = shmat(atoi(argv[2]), NULL, 0);
    shmnavi = shmat(atoi(argv[3]), NULL, 0);
    sem_shmporto = atoi(argv[1]);
    sem_shmnave = atoi(argv[10]);
    sem_porto = atoi(argv[5]);
    id = atoi(argv[4]);
    SO_NAVI = atoi(argv[6]);
    SO_MERCI = atoi(argv[7]);
    shmmerci = shmat(atoi(argv[8]), NULL, 0);
    shmgiorno = shmat(atoi(argv[9]), NULL, 0);
    sem_avvio = atoi(argv[11]);
    shmfill = shmat(atoi(argv[12]), NULL, 0);
    sem_fill = atoi(argv[13]);
    
    nuova_richiesta();
    /*SEMAFORO PER AVVISARE IL PADRE CHE IL PORTO E' PRONTO*/
    sem_uscita(sem_avvio, 0);
    /*SEMAFORO CON CUI IL PADRE DA' IL VIA ALLA SIMULAZIONE*/
    sem_accesso(sem_avvio, 1);

    /*ENTRA IN UN CICLO INFINITO PER ATTENDERE LA TERMINAZIONE DEL PADRE.
    VA POI MODIFICATO PER ESEGUIRE LE OPERAZIONI NECESSARIE.*/
    for (;;) {}
    exit(0);
}

carico creazione_offerta(int ton) {
    carico c;
    struct timespec now;
    if (SO_MERCI > 1) {

        do {
            clock_gettime(CLOCK_REALTIME, &now);
            c.idmerce = (now.tv_nsec % (SO_MERCI * 10000)) / 10000;
        } while (c.idmerce == id_merce_richiesta);
    }else{
        c.idmerce = 0;
    }
    c.pid = getpid();
    c.qmerce = ton/shmmerci[c.idmerce].dimensione;
    c.scadenza = shmmerci[c.idmerce].scadenza + *shmgiorno;
    shmmerci[c.idmerce].pres_porto += c.qmerce;
    shmmerci[c.idmerce].totale += c.qmerce;
    return c;
}

carico creazione_richiesta(int ton) {
    carico c;
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    c.pid = getpid();
    c.idmerce = (now.tv_nsec % (SO_MERCI * 10000)) / 10000;
    id_merce_richiesta = c.idmerce;
    c.qmerce = ton/shmmerci[c.idmerce].dimensione;
    c.scadenza = -1;                        /*SETTO LA SCADENZA A -1 PERCHE' LA RICHIESTA NON HA SCADENZA*/
    return c;
}

void nuova_offerta_handler(int signum) {    /*SIGUSR1*/
    nuova_offerta();
}

void nuova_offerta() {
    int offerta;
    struct timespec now;
    int totale = 0;
    shmmerci[shmporti[id].richiesta.idmerce].q_ferma += shmporti[id].offerta.qmerce;
    sem_accesso(sem_fill, 1);
    /*gen offerta*/
    clock_gettime(CLOCK_REALTIME, &now);
    offerta = shmfill[0] + (now.tv_nsec % (shmfill[1] * 2)) - shmfill[1];
    clock_gettime(CLOCK_REALTIME, &now);
    shmporti[id].offerta = creazione_offerta(offerta);

    shmfill[4]--;
    sem_uscita(sem_fill, 1);
    shmporti[id].tot_offerta += shmporti[id].offerta.qmerce;
}

void nuova_richiesta() {
    int richiesta;
    struct timespec now;
    int totale = 0;
    sem_accesso(sem_fill, 0);
    totale = shmfill[5];
    if (totale > 0) {
        /*g        sem_uscita(sem_fill, 0);en richiesta*/
        clock_gettime(CLOCK_REALTIME, &now);
        if(shmfill[3]<=0)
            shmfill[3] = 5;
        richiesta = shmfill[2] + (now.tv_nsec % (shmfill[3] * 2)) - shmfill[3];
        shmporti[id].richiesta = creazione_richiesta(richiesta);
        shmporti[id].tot_richiesta += shmporti[id].richiesta.qmerce;
        TEST_ERROR;
    }
    shmfill[5]--;
    sem_uscita(sem_fill, 0);
    nuova_offerta();
}

void handler_mareggiata(int signum){
    int i;
    for(i=0; i< SO_NAVI; i++){
        sem_accesso(sem_shmnave, i);
        if(shmnavi[i].stato_nave != -1){
            if(shmnavi[i].x == shmporti[id].x && shmnavi[i].y == shmporti[id].y && shmnavi[i].stato_nave != 1 && shmnavi[i].stato_nave != 4){
                kill(shmnavi[i].pid, SIGUSR1);
            }
        }
        sem_uscita(sem_shmnave, i);
    }
}