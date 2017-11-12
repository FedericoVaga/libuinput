LIB = libuinput.a
LIBS = libuinput.so
LOBJ := libuinput.o

CFLAGS = -fPIC -Wall -Werror -ggdb -O2 -I.
LDFLAGS = -L. -luinput

modules all: lib

lib: $(LIB) $(LIBS)


%: %.c $(LIB)
	$(CC) $(CFLAGS) $*.c $(LDFLAGS) -o $@

$(LIB): $(LOBJ)
	ar -c -s -r $@ $^

$(LIBS): $(LOBJ)
	$(CC) -shared -o $@ $^

clean:
	rm -f $(LIB) .depend *.o *~
