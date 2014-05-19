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

all: libquirc.so inspect calibration client

inspect: src/util.o src/inspect.o libquirc.a
	g++ -O3 -o $@ $^ -lm `pkg-config --cflags --libs opencv`

calibration: src/util.o src/calibration.o  libquirc.a
	g++ -O3 -o $@ $^ -lm `pkg-config --cflags --libs opencv`

client: src/client.o
	g++ -O3 -o $@ $^ -lm

libquirc.a: $(LIB_OBJ)
	rm -f $@
	ar cru $@ $^
	ranlib $@

libquirc.so: $(LIB_SOBJ)
	cc -O3 -shared -dynamiclib -o $@ $^ -lm

%.o: %.cpp
	cc -O3 $(QUIRC_CFLAGS) -o $*.o -c $*.cpp

%.lo: %.cpp
	cc -O3 -fPIC $(QUIRC_CFLAGS) -o $*.lo -c $*.cpp

clean:
	rm -f */*.o
	rm -f *.o
	rm -f */*.lo
	rm -f libquirc.a
	rm -f libquirc.so
	rm -f inspect
	rm -f calibration
	rm -f client
