CC=gcc
CFLAGS=-Wall -std=gnu99 -O2

DEPS = DataBuffer.h GoBackNMessageStruct.h SocketConnection.h timeval_macros.h CRC.h

all: GoBackNSender GoBackNReceiver

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

GoBackNSender: GoBackNSender.o DataBuffer.o SocketConnection.o GoBackNMessageStruct.o CRC.o
	$(CC) -o $@ $^ $(CFLAGS)

GoBackNReceiver: GoBackNReceiver.o SocketConnection.o GoBackNMessageStruct.o CRC.o
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o GoBackNSender GoBackNReceiver
