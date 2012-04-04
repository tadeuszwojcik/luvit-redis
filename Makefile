CFLAGS=$(shell luvit --cflags)
LIBS=$(shell luvit --libs)

all: build/redis.luvit

build/%.luvit: src/%.c
	mkdir -p build
	$(CC) ${CFLAGS} -Ideps/hiredis/ src/luvit-hiredis-adapter.h -o $@ $^ ${LIBS} deps/hiredis/libhiredis.a

clean:
	rm -fr build


