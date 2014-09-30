LIB = libuinput.so
LOBJ := libuinput.o

CFLAGS = -fPIC -Wall -ggdb -O2 -I.
LDFLAGS = -L. -luinput -shared

modules all: lib

lib: $(LIB)


%: %.c $(LIB)
	$(CC) $(CFLAGS) $*.c $(LDFLAGS) -o $@

$(LIB): $(LOBJ)
	ar -c -s -r $@ $^

clean:
	rm -f $(LIB) .depend *.o *~
