CC=gcc
CFLAGS=-I.
DEPS = 
OBJTST = psock_test.o

all: psock_test 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

psock_test: $(OBJTST)
	$(CC) -o $@ $^ $(CFLAGS)

clean : 
	-rm $(OBJFFS) $(OBJTST)
	-rm psock_test
