
FLAGS= -std=c89 -pedantic -D_GNU_SOURCE

all: nave porto main
	
avvia: main nave porto
	./main
	
main: main.c
	gcc $(FLAGS) main.c -o main
	
nave: nave.c
	gcc $(FLAGS) nave.c -o nave -lm

porto: porto.c
	gcc $(FLAGS) porto.c -o porto

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

