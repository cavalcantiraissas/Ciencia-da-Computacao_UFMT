CC = gcc
CFLAGS = -Wall -Werror -pthread -O
TSAN_CFLAGS = -Wall -Werror -pthread -fsanitize=thread -g

.PHONY: all clean tsan

all: opzip ounzip

opzip: opzip.c
	$(CC) $(CFLAGS) -o opzip opzip.c

ounzip: ounzip.c
	$(CC) -Wall -Werror -O -o ounzip ounzip.c

tsan: opzip_tsan

opzip_tsan: opzip.c
	$(CC) $(TSAN_CFLAGS) -o opzip_tsan opzip.c

clean:
	rm -f opzip ounzip opzip_tsan *.o
