LIB = libuinput.a
LOBJ := libuinput.o

CFLAGS = -Wall -ggdb -O2 -I.
LDFLAGS = -L. -luinput

modules all: lib

lib: $(LIB)


%: %.c $(LIB)
	$(CC) $(CFLAGS) $*.c $(LDFLAGS) -o $@

$(LIB): $(LOBJ)
	ar -c -s -r $@ $^

clean:
	rm -f $(LIB) .depend *.o *~
