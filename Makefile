
#Makefile for songoftime cmd line utility
#This program is free software; you can redistrubute it and/or modify it
#under the terms of the GNU General Public License.

SHELL = /bin/bash

#### Start of system configuration section. ####

srcdir = .

CC = g++
INSTALL = /usr/bin/install -c
INSTALLDATA = /usr/bin/install -c -m 644

#Prefix for each installed program
prefix = /usr/local

#directory to install songoftime in
exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin

#directory to install info files in
infodir = $(prefix)/info

#### End of system configuration section. ####
#exec_prefix = '$(prefix)';
#libdir = '$(exec_prefix)/lib';
#datadir = '$(prefix)/lib';
#statedir = '$(prefix)/lib';
#includedir = '$(prefix)/include';
#oldincludedir = "";
#mandir = '$(prefix)/man/man1';
#manext = '.1'


#Source file variables
SRC1 = SongOfTime.cpp

all:	songoftime

songoftime:
	$(CC) -o $@ $(SRC1)

clean:
	echo "Clean completed"

install: all
	$(INSTALL) songoftime $(bindir)/songoftime
	#$(INSTALLDATA) $(srcdir)/songoftime.info* $(infodir)
