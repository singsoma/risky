# risky - RISC-V emulator
# See LICENSE file for copyright and license details.

CC = cc

PREFIX    = /usr/local
MANPREFIX = $(PREFIX)/share/man

CFLAGS = -std=c99 -O0 -Wall -Wextra -pedantic

SRC = risky.c io.c cpu.c dram.c bus.c 
BIN = risky
MAN = risky.1

# OpenBSD (uncomment)
# MANPREFIX = ${PREFIX}/man

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) -o $@ $>

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f $(BIN) $(DESTDIR)$(PREFIX)/bin
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	cp -f $(MAN) $(DESTDIR)$(MANPREFIX)/man1

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(BIN)
	rm -f $(DESTDIR)$(MANPREFIX)/man1/$(MAN)

clean:
	rm -f $(BIN)

.PHONY: all clean install uninstall
