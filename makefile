# Compiler
CC = gcc

# Compiler flags
CFLAGS = `pkg-config --cflags gtk+-3.0 glib-2.0 gstreamer-1.0 gstreamer-video-1.0` -Wall -g
LDFLAGS = `pkg-config --libs gtk+-3.0 glib-2.0 gstreamer-1.0 gstreamer-video-1.0` -lpthread

# Source files
SRCS = main.c
OBJS = $(SRCS:.c=.o)

# Executable name
TARGET = ippis_tft

# Rules
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean

