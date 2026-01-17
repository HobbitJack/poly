.POSIX:	
.PHONY:	all clean install release source uninstall
.SUFFIXES:

PREFIX 	?= /usr/local

all:	poly

poly:	poly.c poly.g.c poly.g.h poly.l.c poly.l.h
	c99 -lm -lgsl -o $@ poly.c poly.g.c poly.l.c

poly.l.c poly.l.h:	poly.l
	lex -D_POSIX_C_SOURCE=200809L -o poly.l.c poly.l

poly.g.c poly.g.h:	poly.g
	gengetopt <poly.g
	sed -E 's/(\\n)?[[:blank:]]+\(default=.*\)//' <poly.g.c >poly.g.c.tmp
	mv -f poly.g.c.tmp poly.g.c

clean:
	rm -f poly poly.g.? poly.l.? poly*.tar.gz poly.1.gz Makefile

source:
	rm -f poly_source.tar.gz
	tar -cf poly_source.tar poly.c poly.g poly.l poly.1 makefile
	gzip poly_source.tar

release:	poly
	rm -f poly.tar.gz
	sed 6,33d makefile | sed '2c .PHONY:	install uninstall'> Makefile
	tar -cf poly.tar poly poly.c poly.g poly.l poly.1 makefile
	gzip poly.tar

install:	poly
	mkdir -p $(PREFIX)/bin/
	install poly $(PREFIX)/bin/
	gzip -k poly.1
	mkdir -p $(PREFIX)/share/man/man1/
	install poly.1.gz $(PREFIX)/share/man/man1/

uninstall:
	rm $(PREFIX)/bin/poly
	rm $(PREFIX)/share/man/man1/poly.1.gz 
