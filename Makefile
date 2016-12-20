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
LIB=lib

CC ?= gcc
CFLAGS ?= -Wextra -Wall -iquote$(SRC) -fpic

.PHONY: all install uninstall clean

EXES = libnms.so

all: $(EXES)
	
libnms.so: $(OBJ)/libnms.o | $(LIB)
	$(CC) -shared -o $(LIB)/$@ $^

$(OBJ)/%.o: $(SRC)/%.c | $(OBJ)
	$(CC) $(CFLAGS) -o $@ -c $<

$(LIB):
	mkdir -p $(LIB)

$(OBJ):
	mkdir -p $(OBJ)

clean:
	rm -rf $(LIB)
	rm -rf $(OBJ)

install:
	install -d $(DESTDIR)$(libdir)
	cd $(BIN) && install $(EXES) $(DESTDIR)$(libdir)

uninstall:
	for exe in $(EXES); do rm $(DESTDIR)$(libdir)/$$exe; done
