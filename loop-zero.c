#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>



int numc;
int* arrayPid;


void handle_signal(int signal){
    printf("\n\n-------segnale ricevuto-------\n\n");
    
    time_t t;
    srandom((unsigned)time(&t));
    int a =  (rand() % (numc))+1;
    printf("\n\n%d %d\n\n",a,numc);

    printf("\n\nterminazione processo:%d\n\n", arrayPid[a]);
    numc--;
    kill(arrayPid[a] ,SIGTERM);
    sleep(2);
}

void handle_alarm(){
    kill(getpid(), SIGKILL);
}


int main(int argc, char** argv){
    
    struct sigaction sa;
    bzero(&sa, sizeof(sa));
    sa.sa_handler = handle_signal;
    sigaction(SIGUSR1, &sa, NULL);

    /*per evitare processi infiti :)*/
    struct sigaction alarmh;
    bzero(&alarmh, sizeof(alarmh));
    alarmh.sa_handler = handle_alarm;
    sigaction(SIGALRM, &alarmh, NULL);

    numc = atoi(argv[1]); 
    int n = atoi(argv[2]);
    int var=0,i=0;
    pid_t my_pid, my_ppid;

    pid_t temp[numc];
    arrayPid = temp;

    for(i=0; i<numc; i++){
        if(arrayPid[i] = fork()!=0){ 
        }
        else{
            //child
            alarm(10);
            my_pid = getpid();
            my_ppid = getppid();
            while(1){
                /*var++;
                printf("\nPID:%d\tPPID:%d\tvar:%d\t\n",my_pid,my_ppid,var);
                if(var==n){
                    printf("\n\n-------segnale mandato-------\n\n");
                    var = 0;
                    printf("\n\n%d\n\n",kill(my_ppid, SIGUSR1));
                    sleep(5);
                    //exit(0);
                }*/
            } 
        }
    }
    for(i=0 ;i<(sizeof(arrayPid)/sizeof(*arrayPid));i++){
        printf("%d ",arrayPid[i]);
    }
    for(i=0; i<numc; i++){
        wait(NULL);
    }
}