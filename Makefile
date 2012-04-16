CFLAGS=$(shell luvit --cflags | sed s/-Werror//)
LIBS=$(shell luvit --libs)
HIREDISDIR=deps/hiredis
all: build/redis.luvit


${HIREDISDIR}/Makefile:
	git submodule update --init ${HIREDISDIR}

${HIREDISDIR}/libhiredis.a: ${HIREDISDIR}/Makefile
	$(MAKE) -C ${HIREDISDIR} libhiredis.a

build/%.luvit: src/%.c ${HIREDISDIR}/libhiredis.a
	mkdir -p build
	$(CC) ${CFLAGS} -I${HIREDISDIR} -Isrc -o $@ $^ ${LIBS} ${HIREDISDIR}/libhiredis.a

clean:
	${MAKE} -C ${HIREDISDIR} clean
	rm -fr build

.PHONY: all clean deps


