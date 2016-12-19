# Installation directories following GNU conventions
prefix ?= /usr/local
exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin
sbindir = $(exec_prefix)/sbin
datarootdir = $(prefix)/share
datadir = $(datarootdir)
includedir = $(prefix)/include
mandir = $(datarootdir)/man
libdir = $(prefix)/lib

BIN=bin
OBJ=obj
SRC=src

CC ?= gcc
CFLAGS ?= -Wextra -Wall -iquote$(SRC)

.PHONY: all install uninstall clean

EXES = libnms.so

all: $(EXES)

libnms.so: $(OBJ)/libnms.o | $(BIN)
	$(CC) $(CFLAGS) -o $(BIN)/$@ $^

$(OBJ)/%.o: $(SRC)/%.c | $(OBJ)
	$(CC) $(CFLAGS) -o $@ -c $<

$(BIN):
	mkdir -p $(BIN)

$(OBJ):
	mkdir -p $(OBJ)

clean:
	rm -rf $(BIN)
	rm -rf $(OBJ)

install:
	install -d $(DESTDIR)$(libdir)
	cd $(BIN) && install $(EXES) $(DESTDIR)$(libdir)

uninstall:
	for exe in $(EXES); do rm $(DESTDIR)$(libdir)/$$exe; done
