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
#include <math.h>


typedef struct {
    int id;
    int dimensione;
    int scadenza;
}smerce;

typedef struct {
    int pid;
    int idmerce;            
    int qmerce;
    int scadenza;
}carico;

typedef struct node {
	carico elem;
	struct node * next;  
} node;

typedef node* list;

/*STATO NAVE:
-1: DISTRUTTA DA MAELSTORM
0:IN PORTO
1:IN MARE
2:IN PORTO CHE EFFETTUA UNO SCARICO
3:IN PORTO CHE EFFETTUA UN CARICO
4:IN MARE FERMA A CAUSA DI UNA TEMPESTA
5:IN PORTO FERMA A CAUSA DI UNA MAREGGIATA
*/
typedef struct {
    pid_t pid;
    double x;
    double y;
    int carico_tot;
    int stato_nave;
}snave;

typedef struct {
    pid_t pid;
    double x;
    double y;
    carico offerta;
    carico richiesta;
    int richiesta_soddisfatta;
    int destinazione;
}sporto;


#define TEST_ERROR    if (errno) {fprintf(stderr,           \
                      "%s:%d: PID=%5d: Error %d (%s)\n", \
                      __FILE__,         \
                      __LINE__,         \
                      getpid(),         \
                      errno,            \
                      strerror(errno));}\
                      errno = 0;

#define STAMPA_ROSSO(x) printf("\033[0;31m"); x; printf("\033[0m"); 

list list_insert_head(list p, carico m);

void list_print(list p);

void list_free(list p);

int list_sum(list p, smerce* m);

list list_remove(list s, int p);

int list_sum_merce(list p, smerce* m, int tipo);

list list_controllo_scadenza(list p, smerce* m, int giorno, int* capacita);

list list_rimuovi_richiesta(list p, carico richiesta, sporto* shmporti, int id);



void sem_accesso(int semid, int num_risorsa);

void sem_uscita(int semid, int num_risorsa);

void stampa_merci(smerce* temp_merci);

list carico_nave(carico c, list p, int speed, smerce* m, snave n);

int pid_to_id_porto(pid_t pid, sporto* p);

void rmLinesTerminal(int n);

double dist(double x1, double y1, double x2, double y2);