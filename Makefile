all: ttt
ttt:
	gcc process.c schedule.c main.c -o main -O2
clean: 
	rm -f dmes*  
