CC = gcc
SRC = libv4l2.c example_capture.c
TARGET = capture

all:
	$(CC) -g -o $(TARGET) $(SRC)
