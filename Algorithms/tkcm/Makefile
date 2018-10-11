CFLAGS += -Wall -Wextra -O3 -std=c11

all: clean paper chlorine

paper: paper.c tkcm.c tkcm.h
	$(CC) $(CFLAGS) -o paper paper.c tkcm.c

chlorine: chlorine.c tkcm.c tkcm.h
	$(CC) $(CFLAGS) -o chlorine chlorine.c tkcm.c

hydro: hydro.c tkcm.c tkcm.h
	$(CC) $(CFLAGS) -o hydro hydro.c tkcm.c

stock: stock.c tkcm.c tkcm.h
	$(CC) $(CFLAGS) -o stock stock.c tkcm.c -lm

bball: clean bball.c tkcm.c tkcm.h testscenario.h
	$(CC) $(CFLAGS) -o bball bball.c tkcm.c -lm

clean:
	rm -rf *.o ./paper ./chlorine ./hydro ./stock ./bball
