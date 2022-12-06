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
    int idmerce;            /*REQUEST DATA*/
    int qmerce;
    int scadenza;
}carico;

typedef struct node {
	carico elem;
	struct node * next;  
} node;

typedef node* list;

typedef struct {
    pid_t pid;
    double x;
    double y;
    list lista_merci;
    int carico_tot;
    int stato_nave;
}snave;

typedef struct {
    pid_t pid;
    double x;
    double y;
    carico offerta;
}sporto;


typedef struct{
	long mtype;            
	carico mtext;    
}msg;



#define TEST_ERROR    if (errno) {fprintf(stderr,           \
                      "%s:%d: PID=%5d: Error %d (%s)\n", \
                      __FILE__,         \
                      __LINE__,         \
                      getpid(),         \
                      errno,            \
                      strerror(errno));}

list list_insert_head(list p, carico m);

void list_print(list p);

void list_free(list p);



void sem_accesso(int semid, int num_risorsa);

void sem_uscita(int semid, int num_risorsa);

void stampa_merci(smerce* temp_merci);

int msg_invio(int id, carico r);

int msg_lettura(int id, carico* r);

int msg_error();

void msg_print_stats(int fd, int q_id);