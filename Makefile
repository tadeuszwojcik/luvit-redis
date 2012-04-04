CFLAGS=-I/usr/local/include/luvit -I/usr/local/include/luvit/uv -I/usr/local/include/luvit/luajit -I/usr/local/include/luvit/http_parser -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fPIC

LIBS=$(shell luvit --libs)

all: redis.luvit

luvit-redis.o: src/luvit-redis.c
	$(CC) -c src/luvit-redis.c ${CFLAGS} -I/usr/local/include/hiredis src/libev.h

redis.luvit: luvit-redis.o
	$(CC) -o redis.luvit luvit-redis.o ${LIBS} /usr/local/lib/libhiredis.a

clean:
	rm -f luvit-redis.o redis.luvit
