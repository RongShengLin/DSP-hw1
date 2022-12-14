.PHONY: all clean run
CC=gcc
CFLAGS=-std=c99 -O2
LDFLAGS=-lm
TARGET=train test test_acc
TRAIN_ITER=100

all: $(TARGET)

train: src/train.c
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -Iinc

test: src/test.c
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -Iinc

test_acc: src/test_acc.c
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -Iinc
clean:
	rm -f $(TARGET)

