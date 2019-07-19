CC = gcc
EXAMPLE1_SRC = libv4l2/libv4l2.c example_capture.c
EXAMPLE1 = capture

EXAMPLE2_SRC = libv4l2/libv4l2.c libv4l2/color.c example_video2lcd.c
EXAMPLE2 = video2lcd

all:
	$(CC) -g -o $(EXAMPLE1) $(EXAMPLE1_SRC)
	$(CC) -g -o $(EXAMPLE2) $(EXAMPLE2_SRC)
