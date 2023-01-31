CC ?= cc
LINK_DEPS = -lSDL2
CFLAGS ?= -Ofast
LDFLAGS = -lm
PRJ_NAME = borg

ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

all:
	$(CC) $(CFLAGS) main.c $(LINK_DEPS) $(LDFLAGS) -o $(PRJ_NAME)

install:
	install -Dm 0755 $(PRJ_NAME) -t $(DESTDIR)$(PREFIX)/bin

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(PRJ_NAME)

clean:
	rm -f $(PRJ_NAME)
