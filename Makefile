CC=gcc
OBJ = sc.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

sc: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) -lreadline
