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

HEADERS = libnms.h
LIBRARIES = libnms.so

all: $(LIBRARIES)
	
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
	install -d $(DESTDIR)$(includedir)
	cd $(LIB) && install $(LIBRARIES) $(DESTDIR)$(libdir)
	cd $(SRC) && install $(HEADERS) $(DESTDIR)$(includedir)

uninstall:
	for library in $(LIBRARIES); do rm $(DESTDIR)$(libdir)/$$library; done
	for header in $(HEADERS); do rm $(DESTDIR)$(includedir)/$$header; done
