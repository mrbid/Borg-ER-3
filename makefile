all:
	gcc main.c -Ofast -lSDL2 -lm -o borg

install:
	cp borg $(DESTDIR)

uninstall:
	rm $(DESTDIR)/borg

clean:
	rm borg
