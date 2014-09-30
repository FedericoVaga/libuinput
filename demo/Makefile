LIB = ..

CFLAGS = -Wall -ggdb -O2 -I$(LIB)/
LDFLAGS = -L$(LIB)/ -luinput

DEMOS := keyboard_demo
DEMOS += keyboard_demo_shared

all: demo

demo: $(DEMOS)

keyboard_demo: keyboard_demo.c
	$(CC) $(CFLAGS) $^ -o $@ -static $(LDFLAGS)

keyboard_demo_shared: keyboard_demo.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) -ldl

clean:
	rm -f $(DEMOS) *.o *~

.PHONY: all, clean
