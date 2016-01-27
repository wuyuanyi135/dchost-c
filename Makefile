CC = gcc
CCFLAGS = -Wall -g -c -std=c99

LD = gcc
LDFLAGS = -lwiringPi -lm -g

.phony: clean all
all: main

main: nrf24l01.o app.o main.o
	$(LD) $(LDFLAGS) $^ -o $@

main_dwire: nrf24l01.o app_dwire.o main.o dwire.o queue.o
	$(LD) $(LDFLAGS) $^ -o $@

queue.o: queue.c
	$(CC) $(CCFLAGS) -o $@ $^

app_dwire.o: app_dwire.c
	$(CC) $(CCFLAGS) -o $@ $^
dwire.o: dwire.c
	$(CC) $(CCFLAGS) -o $@ $^
nrf24l01.o: nrf24l01.c
	$(CC) $(CCFLAGS) -o $@ $^
app.o: app.c
	$(CC) $(CCFLAGS) -o $@ $^
main.o: main.c
	$(CC) $(CCFLAGS) -o $@ $^
clean:
	rm -f *.o *.out
