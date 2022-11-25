
FLAGS= -std=c89 -pedantic -D_GNU_SOURCE

all: nave porto main
	echo $^
	
main: main.c
	gcc $(FLAGS) main.c -o main
	
nave: nave.c
	gcc $(FLAGS) nave.c -o nave

porto: porto.c
	gcc $(FLAGS) porto.c -o porto

clean:
	rm -f *.o main nave porto
	
download:
	rm -f *
	git init
	git pull https://github.com/Pit-Red/progetto_navi.git

upload:
	git add .
	git commit -m "commit from make"
	git push https://github.com/Pit-Red/progetto_navi.git
	echo ciao
