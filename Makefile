CFLAGS=$(shell luvit --cflags)
LIBS=$(shell luvit --libs)

all: build/redis.luvit

build/%.luvit: src/%.c
	mkdir -p build

	$(CC) ${CFLAGS} -I/usr/local/include/hiredis src/libev.h -o $@ $^ ${LIBS} /usr/local/lib/libhiredis.a

clean:
	rm -fr build
