CC = /home/jim/workspace/android/king_rp_rk3399_DDR3_8.1_SDK-20200716/prebuilts/gcc/linux-x86/aarch64/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc
#CC = gcc
APP_SRC = libv4l2/libv4l2.c example_capture.c getusbdev/srcfile/find_usbdevice.c
APP = capture

all:
	$(CC) -g -o $(APP) $(APP_SRC) -I ./getusbdev/srcfile -static
	#$(CC) -g -o $(EXAMPLE2) $(EXAMPLE2_SRC)'
clean:
	rm $(APP)
