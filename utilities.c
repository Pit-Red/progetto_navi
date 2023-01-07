#include "utilities.h"
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>

int i;
int j;

void sem_accesso(int semid, int num_risorsa) {
    struct sembuf my_op;
    my_op.sem_num = num_risorsa;
    my_op.sem_flg = 0;
    my_op.sem_op = -1;
    semop(semid, &my_op, 1);
    if (errno == 4) {
        errno = 0;
    }
    TEST_ERROR;
}

void sem_uscita(int semid, int num_risorsa) {
    struct sembuf my_op;
    my_op.sem_num = num_risorsa;
    my_op.sem_flg = 0;
    my_op.sem_op = 1;
    semop(semid, &my_op, 1);
    if (errno == 4) {
        errno = 0;
    }
    TEST_ERROR;
    if (errno == 27) {
        STAMPA_ROSSO(fprintf(stderr, "semid = %d\n", semid));
    }
    TEST_ERROR;
}

void stampa_merci(smerce* temp_merci) {
    int size = 3;
    for (i = 0; i < size; i++) {
        printf("\nsmerce[%d]:id=%d\tdimensione=%d\ttempo di scadenza=%d\n", i, temp_merci[i].id, temp_merci[i].dimensione, temp_merci[i].scadenza);
    }
}


list list_insert_head(list p, carico m) {
    list new_elem;
    new_elem = malloc(sizeof(*new_elem));
    new_elem->elem.pid = m.pid;
    new_elem->elem.idmerce = m.idmerce;
    new_elem->elem.qmerce = m.qmerce;
    new_elem->elem.scadenza = m.scadenza;
    new_elem->next = p;
    return new_elem;
}

void list_free(list p) {
    if (p == NULL) {
        return;
    }
    list_free(p->next);
    free(p);
}

int list_sum(list p, smerce* m) { /*restituisce tonnellate*/
    int sum = 0;
    for (; p != NULL; p = p->next) {
        sum += p->elem.qmerce * m[p->elem.idmerce].dimensione;
    }
    return sum;
}

int list_sum_merce(list p, smerce* m, int tipo) {       /*restituisce la quantitaa di merce i*/
    int sum = 0;
    for (; p != NULL; p = p->next) {
        if (p->elem.idmerce == tipo)
            sum += p->elem.qmerce;
    }
    return sum;
}

list list_controllo_scadenza(list p, smerce* m, int giorno, int* capacita) {
    list temp = NULL;
    for (; p != NULL; p = p->next) {
        if (p->elem.scadenza > giorno) {
            temp = list_insert_head(temp, p->elem);
            *capacita += (p->elem.qmerce * m[p->elem.idmerce].dimensione);
        }
        else{
            m[p->elem.idmerce].scaduta_nave.qmerce += p->elem.qmerce;
            m[p->elem.idmerce].pres_na.qmerce -= p->elem.qmerce;
        }
    }
    return temp;
}

list list_rimuovi_richiesta(list p, sporto* shmporti, int id, smerce* shmmerci) {
    list temp = NULL;
    for (; p != NULL; p = p->next) {
        if (shmporti[id].richiesta_soddisfatta != 1 && shmporti[id].richiesta.idmerce == p->elem.idmerce) {
            if (p->elem.qmerce > shmporti[id].richiesta.qmerce) {
                p->elem.qmerce -= shmporti[id].richiesta.qmerce;
                shmmerci[p->elem.idmerce].consegnata.qmerce += shmporti[id].richiesta.qmerce;
                shmmerci[p->elem.idmerce].pres_na.qmerce -= shmporti[id].richiesta.qmerce;
                temp = list_insert_head(temp, p->elem);
            }
            else if (p->elem.qmerce < shmporti[id].richiesta.qmerce) {
                shmporti[id].richiesta.qmerce -= p->elem.qmerce;
                shmmerci[p->elem.idmerce].consegnata.qmerce += p->elem.qmerce;
                shmmerci[p->elem.idmerce].pres_na.qmerce -= p->elem.qmerce;
            }
            else {
                shmmerci[p->elem.idmerce].consegnata.qmerce += shmporti[id].richiesta.qmerce;
                shmmerci[p->elem.idmerce].pres_na.qmerce -= shmporti[id].richiesta.qmerce;
            }
        }
        else {
            temp = list_insert_head(temp, p->elem);
        }
    }
    return temp;
}

void rmLinesTerminal(int n) {
    for (i = 0; i < n - 1; i++) {
        printf("\33[2K\r");
        printf("\033[A\r");
    }
    printf("\33[2K\r");
}

double dist(double x1, double y1, double x2, double y2) {
    return sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2));
}