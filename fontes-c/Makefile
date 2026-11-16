CC ?= cc
AR ?= ar
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Werror -O2

FONTES := $(filter-out biblioteca.c,$(wildcard *.c))
PROGRAMAS := $(patsubst %.c,bin/%,$(FONTES))

.PHONY: all test clean

all: $(PROGRAMAS)

libdados.a: biblioteca.o
	$(AR) rcs $@ $^

biblioteca.o: biblioteca.c biblioteca.h
	$(CC) $(CFLAGS) -c $< -o $@

bin:
	mkdir -p $@

bin/%: %.c biblioteca.h libdados.a | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $< libdados.a $(LDLIBS) -o $@

test: all
	@set -e; for programa in $(PROGRAMAS); do \
	  "$$programa" > /dev/null; printf "OK %s\n" "$$programa"; \
	done

clean:
	rm -f biblioteca.o libdados.a
	rm -rf bin
