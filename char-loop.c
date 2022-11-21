#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>

unsigned char c;

void handle_signal(int signal){
    printf("c=%c\n", c);
    exit(c);
}

int main(int argc, char** argv){
    
    c = *argv[0];
    struct sigaction sa;
    bzero(&sa, sizeof(sa));
    sa.sa_handler = handle_signal;
    sigaction(SIGINT, &sa, NULL);

    while(1){
        c++;
        if(c>=126)
            c=33;
    }
}


/*soos*/
