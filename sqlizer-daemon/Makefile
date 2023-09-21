################################################################
#  Makefile --  Makefile for the sqlizer-daemon code
#
#  Copyright:   Copyright (C) 2023 by Atomlab, LLC
# 
#  License:     This program is free software; you can redistribute it and/or
#               modify it under the terms of the Version 2 of the GNU General
#               Public License as published by the Free Software Foundation.
#               GPL2.txt in the docs directory is a copy of this license.
#               This program is distributed in the hope that it will be useful,
#               but WITHOUT ANY WARRANTY; without even the implied warranty of
#               MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#               GNU General Public License for more details.
# 
################################################################

PGINC       = -I/usr/include/postgresql -I/usr/include/pgsql -I/usr/local/include
OPT        := -c $(PGINC)
LDOPTS     := -lrta -lm
CC         ?= gcc
DEBUG       = -g -DDEBUG -Wall
CFLAGS      = $(OPT) $(DEBUG)

SYNTHOBJS   = main.o tables.o voices.o

all: sqlizer-daemon

sqlizer-daemon: $(SYNTHOBJS) 
	$(CC) $(SYNTHOBJS) -g -o $@ $(LDOPTS)

main.o: main.c sqlizer.h
	$(CC) $(CFLAGS) $< -o $@

tables.o: tables.c sqlizer.h
	$(CC) $(CFLAGS) $< -o $@

voices.o: voices.c sqlizer.h
	$(CC) $(CFLAGS) $< -o $@

standard: clean
	for i in *.c ; \
	do \
	 indent  -bad -bap -nbbb -bli0 -cdw -cli2 -cbi0 -saf -nut \
		-sai -saw -di9 -nbc -bfda -bls -lp -ci4 -i4 -nbbo \
		-fca -nbfda -nbfde -nlp -bbo -sob -cd32 -ts0 -npcs -nut \
		-npsl -br -ce -T RTA_TBLDEF -T RTA_COLDEF $$i ; \
	 rm $$i~ ; \
	done

clean: 
	rm -rf *.o sqlizer-daemon

