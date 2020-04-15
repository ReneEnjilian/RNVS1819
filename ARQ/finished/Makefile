CC=gcc
CFLAGS=-Wall -std=gnu99 -O2

all: GoBackNSender GoBackNReceiver

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

GoBackNSender: GoBackNSender.o CRC.o
	$(CC) -o $@ $^ $(CFLAGS)

GoBackNReceiver: GoBackNReceiver.o
	$(CC) -o $@ $^ $(CFLAGS)
clean:
	rm -f *.o GoBackNSender GoBackNReceiver
