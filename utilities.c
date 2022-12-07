
#include "utilities.h"
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>

int i;

void sem_accesso(int semid, int num_risorsa) {
    struct sembuf my_op;
    /*printf("\nil processo:%d tenta l'accesso al semaforo:%d\n",getpid(),semid);*/
    my_op.sem_num = num_risorsa;
    my_op.sem_flg = 0;
    my_op.sem_op = -1;
    semop(semid, &my_op, 1);
    /*printf("\nil processo:%d ha avuto accesso al semaforo:%d\n",getpid(),semid);*/
    TEST_ERROR;
}

void sem_uscita(int semid, int num_risorsa) {
    struct sembuf my_op;
    my_op.sem_num = num_risorsa;
    my_op.sem_flg = 0;
    my_op.sem_op = 1;
    semop(semid, &my_op, 1);
    /*printf("\nil processo:%d è uscito dal semaforo:%d\n",getpid(),semid);*/
    TEST_ERROR;
}

void stampa_merci(smerce* temp_merci) {
    int i;
    int size = 3;
    for (i = 0; i < size; i++) {
        printf("\nsmerce[%d]:id=%d\tdimensione=%d\ttempo di scadenza=%d\n", i, temp_merci[i].id, temp_merci[i].dimensione, temp_merci[i].scadenza);
    }
}

int msg_invio(int id, carico r){
    msg mybuf;
    int num_bytes = sizeof(r);
    mybuf.mtype = 1; /*1 lo usiamo per le domande (n>0) */
    mybuf.mtext = r;
    msgsnd(id, &mybuf, num_bytes, 0);
    return msg_error();
}

int msg_lettura(int id, carico* r){
    msg mybuf;
    msgrcv(id, &mybuf, sizeof(*r), 1, 0);
    *r = mybuf.mtext;
    return msg_error();
}

int msg_error(){
    
    switch (errno) {
    case EAGAIN:
        dprintf(2,
                "Queue is full and IPC_NOWAIT was set to have a non-blocking msgsnd()\n\
Fix it by:\n                                \
  (1) making sure that some process read messages, or\n         \
  (2) changing the queue size by msgctl()\n");
        return (-1);
    case EACCES:
        dprintf(2,
                "No write permission to the queue.\n\
Fix it by adding permissions properly\n");
        return (-1);
    case EFAULT:
        dprintf(2,
                "The address of the message isn't accessible\n");
        return (-1);
    case EIDRM:
        dprintf(2,
                "The queue was removed\n");
        return (-1);
    case EINTR:
        dprintf(2,
                "The process got unblocked by a signal, while waiting on a full queue\n");
        return (-1);
    case EINVAL:
        TEST_ERROR;
        return (-1);
    case ENOMEM:
        TEST_ERROR;
        return (-1);
    default:
        TEST_ERROR;
    }
    return 0;
}  

void msg_print_stats(int fd, int q_id) {
    struct msqid_ds my_q_data;
    int ret_val;

    while (ret_val = msgctl(q_id, IPC_STAT, &my_q_data)) {
        TEST_ERROR;
    }
    dprintf(fd, "--- IPC Message Queue ID: %8d, START ---\n", q_id);
    dprintf(fd, "---------------------- Time of last msgsnd: %ld\n",
            my_q_data.msg_stime);
    dprintf(fd, "---------------------- Time of last msgrcv: %ld\n",
            my_q_data.msg_rtime);
    dprintf(fd, "---------------------- Time of last change: %ld\n",
            my_q_data.msg_ctime);
    dprintf(fd, "---------- Number of messages in the queue: %ld\n",
            my_q_data.msg_qnum);
    dprintf(fd, "- Max number of bytes allowed in the queue: %ld\n",
            my_q_data.msg_qbytes);
    dprintf(fd, "----------------------- PID of last msgsnd: %d\n",
            my_q_data.msg_lspid);
    dprintf(fd, "----------------------- PID of last msgrcv: %d\n",
            my_q_data.msg_lrpid);            
    dprintf(fd, "--- IPC Message Queue ID: %8d, END -----\n", q_id);
}


list list_insert_head(list p, carico m){
    list new_elem;
	new_elem = malloc(sizeof(*new_elem));
	new_elem->elem.pid = m.pid;
    new_elem->elem.idmerce = m.idmerce;
    new_elem->elem.qmerce = m.qmerce;
    new_elem->elem.scadenza = m.scadenza;
	new_elem->next = p;
	return new_elem;
}

void list_print(list p){  

	if (p == NULL) {
		printf("Empty list\n");
		return;
	}
	printf("[pid:%d, idmerce:%d, qmerce:%d, tempo scadenza:%d]", p->elem.pid, p->elem.idmerce, p->elem.qmerce, p->elem.scadenza);
	for(; p->next!=NULL; p = p->next) {
		printf(" -> [pid:%d, idmerce:%d, qmerce:%d, tempo scadenza:%d]", p->elem.pid, p->elem.idmerce, p->elem.qmerce, p->elem.scadenza);
	}
	printf("\n");
}

void list_free(list p){
	if (p == NULL) {
		return;
	}
	list_free(p->next);
	free(p);
}