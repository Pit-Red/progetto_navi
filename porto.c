#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

struct porto{
        int velocita;
        int tempo;
    };

/*HANDLER PER GESTIRE IL SEGNAÒLE DI TERMINAZIONE DEL PADRE*/
void handle_signal(int signum){
    printf("E' stato ucciso il porto.\n");
    exit(0);
}

int main(int argc, char** argv){
    /*DICHIARAZIONE DELLE VARIABILI*/
    struct sigaction sa;
    bzero(&sa,sizeof(sa));
    sa.sa_handler = handle_signal;
    sigaction(SIGINT,&sa,NULL);
    printf("E' stato creato un porto\n\n\n");

    /*ENTRA IN UN CICLO INFINITO PER ATTENDERE LA TERMINAZIONE DEL PADRE.
    VA POI MODIFICATO PER ESEGUIRE LE OPERAZIONI NECESSARIE.*/
    for(;;){}
    exit(0);
}
