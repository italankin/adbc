CC=gcc

LDLIBS=-lncurses

EXECUTABLE=adbc
OUT=build

all: adbc

adbc: out
	$(CC) adbc.c $(LDLIBS) -o $(OUT)/$(EXECUTABLE)

out:
	mkdir -p $(OUT)

clean:
	rm -rf $(OUT)
