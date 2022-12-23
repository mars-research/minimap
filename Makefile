
CC := clang

CFLAGS := -g -Wall -O3 -DNDEBUG
CFLAGS += -mno-sse2

all: minimap.c rdtsc.h
	$(CC) -o minimap minimap.c $(CFLAGS)

clean:
	@rm -f minimap
