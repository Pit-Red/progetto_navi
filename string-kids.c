#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>


char* s;
pid_t* child;

void printkids(){
    int i;
    for(i=0; i<3; i++){
        printf("\nchild[%d] = %d",i,child[i]);
    }
}

int main(int argc,char** argv){
    int numfigli = *argv[1]-48;
    char temp[numfigli];
    char* ar[] = {"ciao", "ciao","ciao", NULL};
    s = temp;
    pid_t* tempp = calloc(numfigli, sizeof(*child));
    child = tempp;

    pid_t pid, ppid;
    int i;
    for(i=0; i<numfigli; i++){
        child[i] = fork();
        if(child[i]==-1){
            printf("GIGAERRORE");
            exit(EXIT_FAILURE);
        }
        else if(child[i]==0){
            execvp("./char-loop", ar);
            printf("\ndiocane\n");
        }
    }
    sleep(1);
    for(i=0; i<numfigli; i++){
        kill(child[i], SIGINT);
        int status;
        wait(&status);
        printf("il processo:%d ha rilasciato lo status:%d\n", child[i], WEXITSTATUS(status));
        s[i]=WEXITSTATUS(status);
    }

    printf("\n\nstringa s = %s", s);

    printkids();
}