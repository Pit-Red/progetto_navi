
FLAGS= -std=c89 -pedantic -D_GNU_SOURCE

all: touch utilities.o nave porto main 
	
avvia: all
	rm -f *.o
	./main
	
main: main.c
	gcc $(FLAGS) -c main.c -o main.o
	gcc $(FLAGS) main.o utilities.o -o main
	
nave: nave.c
	gcc $(FLAGS) -c	nave.c -o nave.o -lm
	gcc $(FLAGS) nave.c utilities.o -o nave -lm

porto: porto.c
	gcc $(FLAGS) -c porto.c -o porto.o
	gcc $(FLAGS) porto.c utilities.o -o porto

utilities.o : utilities.c utilities.h
	gcc $(FLAGS) -c utilities.c -o utilities.o

clean:
	rm -f *.o main nave porto
	
touch:
	touch main.c
	touch nave.c
	touch porto.c

download:
	rm -f *
	rm -rf .git
	git init
	git pull https://github.com/Pit-Red/progetto_navi.git

#attualmente l'upload non funge sorry
upload:
	git add .
	git commit -m "commit from make"
	git push https://github.com/Pit-Red/progetto_navi.git
	echo ciao

