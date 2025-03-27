SRC := aesdsocket.c
TARGET = aesdsocket
OBJS := $(SRC:.c=.o)
CFLAGS := -Wall -Werror -g

all: $(TARGET)

default: $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(TARGET) $(LDFLAGS)

clean:
	-rm -f *.o $(TARGET) *.elf *.map
