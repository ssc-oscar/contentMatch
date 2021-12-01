CC=gcc
WIN64CC=x86_64-w64-mingw32-gcc
# Enable all compiler warnings. 
CCFLAGS+= -fPIC -g -Wall -Werror -std=gnu99 -I./inc -I./external/inc

# Linker flags
LDFLAGS+=-lpthread -lcrypto -lssl -lm
LDFLAGS_WIN64+=-L/home/osboxes/scanoss/curl-7.76.1/lib -L/usr/local/lib/ -lpthread -lcrypto -lssl -lcurl -lm

LIBFLAGS=-O -g -Wall -std=gnu99 -fPIC -c

SOURCES=$(wildcard *.c) 
OBJECTS=$(SOURCES:.c=.o)

TARGET=main
BIN_NAME=main
EXPORTED_HEADERS=$(wildcard *.h)

all: main

$(TARGET): $(OBJECTS)
	$(CC) -g -o $(BIN_NAME) $^ $(LDFLAGS)

.PHONY: main 

%.o: %.c
	$(CC) $(CCFLAGS) -o $@ -c $<

clean:
	rm -rf *.o 

distclean: clean

	

