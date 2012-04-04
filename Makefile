CFLAGS=-I/usr/local/include/luvit -I/usr/local/include/luvit/uv -I/usr/local/include/luvit/luajit -I/usr/local/include/luvit/http_parser -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fPIC

LIBS=$(shell luvit --libs)

all: hiredis.luvit

luvit-hiredis.o: src/luvit-hiredis.c
	$(CC) -c src/luvit-hiredis.c ${CFLAGS} -I/usr/local/include/hiredis src/libev.h

hiredis.luvit: luvit-hiredis.o
	$(CC) -o hiredis.luvit luvit-hiredis.o ${LIBS} /usr/local/lib/libhiredis.a

clean:
	rm -f luvit-hiredis.o hiredis.luvit
