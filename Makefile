# quirc -- QR-code recognition library
# Copyright (C) 2010-2012 Daniel Beer <dlbeer@gmail.cppom>
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

# Copyright (C) 2014 brm: Patrizio Bertoni, Giovanni Richini, Michael Maghella
 
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.

#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.

#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

#    For any further information please contact 
#	Patrizio Bertoni at giskard89@gmail.com, 
#	Giovanni Richini at richgio2@hotmail.it, 
#	Michael Maghella at magoo90@gmail.com

PREFIX ?= /usr/local

LIB_VERSION = 1.0
LIB_SONAME = libquirc.so.1

QUIRC_CFLAGS = -g -Wall -Ilib $(CFLAGS)
LIB_OBJ = \
    lib/decode.o \
    lib/identify.o \
    lib/quirc.o \
    lib/version_db.o
LIB_SOBJ = $(subst .o,.lo,$(LIB_OBJ))
OPT= #aggiunto da pat per le unordered map -std=gnu++11 -std=c++0x
#-O3
VERS= -std=c++0x -pthread

all: libquirc.so whereami calibration

whereami: src/util.o src/worldKB.o src/explorerFSM.o src/whereami.o libquirc.a
	g++ $(OPT) -o $@ $^ -lm `pkg-config --cflags --libs opencv`
	g++ src/scorri_camera.cpp -o scorri_camera

calibration: src/util.o src/calibration.o  libquirc.a
	g++ $(OPT) -o $@ $^ -lm `pkg-config --cflags --libs opencv`

libquirc.a: $(LIB_OBJ)
	rm -f $@
	ar cru $@ $^
	ranlib $@

libquirc.so: $(LIB_SOBJ)
	cc $(OPT) -shared -dynamiclib -o $@ $^ -lm

%.o: %.cpp
	g++ $(VERS) $(OPT) -O3 $(QUIRC_CFLAGS) -o $*.o -c $*.cpp

%.lo: %.cpp
	cc -fPIC $(QUIRC_CFLAGS) -o $*.lo -c $*.cpp

clean:
	rm -f */*.o
	rm -f *.o
	#rm -f */*.lo
	#rm -f libquirc.a
	#rm -f libquirc.so
	rm -f whereami
	#rm -f calibration
