CC = gcc
CFLAGS = #-Wall
LDFLAGS=-lpthread -lrt -lwiringPi
OBJFILES = Lab3.o
TARGET = Lab3
	
$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)
clean:
	rm -f $(OBJFILES) $(TARGET) *~
