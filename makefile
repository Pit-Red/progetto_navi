FLAGS= -std=c89 -pedantic -D_GNU_SOURCE

all: compile avvia

compile: touch utilities.o nave porto main
	@bash loading.sh
	
avvia:
	@rm -f *.o
	@./main
	
main: main.c
	@gcc $(FLAGS) -c main.c -o main.o -lm
	@gcc $(FLAGS) main.o utilities.o -o main -lm
	
nave: nave.c
	@gcc $(FLAGS) -c	nave.c -o nave.o -lm
	@gcc $(FLAGS) nave.c utilities.o -o nave -lm

porto: porto.c
	@gcc $(FLAGS) -c porto.c -o porto.o -lm
	@gcc $(FLAGS) porto.c utilities.o -o porto -lm

utilities.o : utilities.c utilities.h
	@gcc $(FLAGS) -c utilities.c -o utilities.o -lm

clean:
	rm -f *.o main nave porto
	
touch:
	@touch main.c
	@touch nave.c
	@touch porto.c

rm: 
	ipcrm -a
	killall -SIGKILL porto nave

download:
	@bash press-enter.sh
	rm -f *
	rm -rf .git
	git init
	git pull https://github.com/Pit-Red/progetto_navi.git

down:
	@bash press-enter.sh
	@bash down.sh $(filter-out $@, $(MAKECMDGOALS))

#attualmente l'upload non funge sorry
upload:
	git add .
	git commit -m "commit from make"
	git push https://github.com/Pit-Red/progetto_navi.git
	echo ciao