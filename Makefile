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

HEADERS = libnms.h
LIBRARIES = libnms.so

.PHONY: all install uninstall clean

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
	if [ -d /etc/ld.so.conf.d ]; then \
		echo "$(DESTDIR)$(libdir)" > /etc/ld.so.conf.d/libnms.conf ; \
	fi
	ldconfig

uninstall:
	for library in $(LIBRARIES); do rm $(DESTDIR)$(libdir)/$$library; done
	for header in $(HEADERS); do rm $(DESTDIR)$(includedir)/$$header; done
	if [ -a /etc/ld.so.conf.d/libnms.conf ] ; then \
		rm /etc/ld.so.conf.d/libnms.conf ; \
	fi
