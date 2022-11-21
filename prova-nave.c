#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define SO_CAPACITY rand()%massimo + 50

int main(int argc, char** argv){
    srand(time(NULL));
    int velocità = atoi(argv[1]);
    double ordinata = atof(argv[2]), ascissa = atof(argv[3]), distanza;
    exit(0);
}