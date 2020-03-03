VERSION = 0.2

ld-floxlib.so: ld-floxlib.c
	$(CC) $(CFLAGS) -shared -fPIC $< -o $@ -ldl

install: ld-floxlib.so
	mkdir -p $(PREFIX)/lib
	cp $< $(PREFIX)/lib
