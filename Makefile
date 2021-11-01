CC=gcc
OBJ = sc.o

%.o: %.c
	$(CC) -g -c -o $@ $< $(CFLAGS)

sc: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) -lreadline

clean:
	rm sc sc.o

