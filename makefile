
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