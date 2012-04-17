CFLAGS  = $(shell luvit --cflags | sed s/-Werror//)
LIBS    = $(shell luvit --libs)
HIREDISDIR = deps/hiredis

# uncomment to use freelist for references (saves malloc/free calls)
#CFLAGS += -DUSE_REF_FREELIST=1

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

profile:
	valgrind --tool=callgrind --dump-instr=yes --simulate-cache=yes --collect-jumps=yes luvit benchmark.lua

profile-mem:
	valgrind --leak-check=full --show-reachable=yes -v luvit benchmark.lua


.PHONY: all clean deps profile profile-mem
