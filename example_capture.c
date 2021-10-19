#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "find_usbdevice.h"
#include "libv4l2/libv4l2.h"
//#define FORMAT V4L2_PIX_FMT_YUYV

#define FORMAT V4L2_PIX_FMT_MJPEG

int set_exposure(int Handle)
{
	int ret;
	struct v4l2_control ctrl;
	//得到曝光模式
	ctrl.id = V4L2_CID_EXPOSURE_AUTO;
	ret = ioctl(Handle, VIDIOC_G_CTRL, &ctrl);
	if (ret < 0) {
		printf("Get exposure auto Type failed\n");
		return -1;
	}
	printf("\nGet Exposure Auto Type:[%d]\n", ctrl.value);
	// 此时，得到曝光模式。曝光模式分以下几种：

	//设置曝光模式为手动模式
	ctrl.id = V4L2_CID_EXPOSURE_AUTO; 
	ctrl.value = V4L2_EXPOSURE_MANUAL;   //手动曝光模式
	ret = ioctl(Handle, VIDIOC_S_CTRL, &ctrl); 
	if (ret < 0) {
		printf("Get exposure auto Type failed\n"); 
		return -1;
	}

	ctrl.id = V4L2_CID_EXPOSURE;   //得到曝光档次，A20接受从 -4到4 共9个档次
	ret = ioctl(Handle, VIDIOC_G_CTRL, &ctrl); 
	if (ret < 0) 
	{ printf("Get exposure failed (%d)\n", ret); 
		return -1;
	} 
	printf("\nGet Exposure :[%d]\n", ctrl.value);
	//设置曝光档次
	ctrl.id = V4L2_CID_EXPOSURE; 
	ctrl.value = 3; 
	ret = ioctl(Handle, VIDIOC_S_CTRL, &ctrl); 
	if (ret < 0) {
		printf("Set exposure failed (%d)\n", ret); 
		return -1;
	}  
		
	ctrl.id = V4L2_CID_EXPOSURE_ABSOLUTE;
	ret = ioctl(Handle, VIDIOC_G_CTRL, &ctrl);
	if (ret < 0) 
	{
		printf("Set exposure failed (%d)\n", ret);
		//return V4L2_UTILS_SET_EXPSURE_ERR;
	}
	printf("\nGet ABS EXP Success:[%d]\n", ctrl.value);

	sleep(1);

	//设置曝光绝对值
	ctrl.id = V4L2_CID_EXPOSURE_ABSOLUTE;
	ctrl.value = 5;
	ret = ioctl(Handle, VIDIOC_S_CTRL, &ctrl);
	if (ret < 0) {
		printf("Set exposure failed (%d)\n", ret);
		//return V4L2_UTILS_SET_EXPSURE_ERR;
	}
	return ret;
}

int main(int argc, char* argv[])
{
	int fd;
	int ret;
	char name[100] = {0};
	int width = 1280, height = 720;
	int nr_bufs = 4;
	int i;
	struct v4l2_capability cap;
	struct v4l2_buf* v4l2_buf;
	struct v4l2_buf_unit* v4l2_buf_unit;
	FILE* fp;
	char str[20] = {0};
	int t;
	char dev_name[10] = {0};
	char DEFAULT_DEV[13] = {0};

	memset(&cap, 0, sizeof(cap));

	if (argc != 2) {
		/*check pid-vid search the device*/
		#if 1
        	get_usbdevname("9230","05a3", video, dev_name);
        	//get_usbdevname("636a","0c46", video, dev_name);
        	//get_usbdevname("636b","0c45", video, dev_name);
		snprintf(DEFAULT_DEV, 13, "/dev/%s", dev_name);
		#endif
		printf("Usage:%s </dev/videox>, default: %s\n", argv[0], DEFAULT_DEV);
		chdir("/data/");
		//return -1;
	} else {
		strncpy(DEFAULT_DEV, argv[1], 13);
	}
	printf("%s\n", DEFAULT_DEV);
	t = time((time_t*)NULL);
	sprintf(str, "%d.jpeg", t);
	printf("str: %s\n", str);
	fp = fopen(str, "w+");
	if (!fp) {
		perror("failed to open picture");
		return -1;
	}

	fd = v4l2_open(DEFAULT_DEV, O_RDWR);
	if(fd < 0)
		goto err;

	ret = v4l2_querycap(fd, &cap);
	if(ret < 0)
		goto err;

	if(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)
		printf("dev support capture\n");

	if(cap.capabilities & V4L2_CAP_VIDEO_OUTPUT)
		printf("dev support output\n");

	if(cap.capabilities & V4L2_CAP_VIDEO_OVERLAY)
		printf("dev support overlay\n");

	if(cap.capabilities & V4L2_CAP_STREAMING)
		printf("dev support streaming\n");

	if(cap.capabilities & V4L2_CAP_READWRITE)
		printf("dev support read write\n");

	set_exposure(fd);
	ret = v4l2_enuminput(fd, 0, name);
	if(ret < 0)
		goto err;
	printf("input device name:%s\n", name);

	ret = v4l2_s_input(fd, 0);
	if(ret < 0)
		goto err;

	ret = v4l2_enum_fmt(fd, FORMAT, V4L2_BUF_TYPE_VIDEO_CAPTURE);
	if(ret < 0)
		goto err;

	ret = v4l2_s_fmt(fd, &width, &height, FORMAT, V4L2_BUF_TYPE_VIDEO_CAPTURE);
	if(ret < 0)
		goto err;
	printf("image width:%d\n", width);
	printf("image height:%d\n", height);

	v4l2_buf = v4l2_reqbufs(fd, V4L2_BUF_TYPE_VIDEO_CAPTURE, nr_bufs);
	if(!v4l2_buf)
		goto err;

	ret = v4l2_querybuf(fd, v4l2_buf);
	if(ret < 0)
		goto err;

	ret = v4l2_mmap(fd, v4l2_buf);
	if(ret < 0)
		goto err;

	ret = v4l2_qbuf_all(fd, v4l2_buf);
	if(ret < 0)
		goto err;

	ret = v4l2_streamon(fd);
	if(ret < 0)
		goto err;

	ret = v4l2_poll(fd);
	if(ret < 0)
		goto err;

	v4l2_buf_unit = v4l2_dqbuf(fd, v4l2_buf);
	if(!v4l2_buf_unit)
		goto err;

	fwrite(v4l2_buf_unit->start, 1, v4l2_buf_unit->length, fp);

	ret = v4l2_qbuf(fd, v4l2_buf_unit);
	if(ret < 0)
		goto err;

	ret = v4l2_streamoff(fd);
	if(ret < 0)
		goto err;

	ret = v4l2_munmap(fd, v4l2_buf);
	if(ret < 0)
		goto err;

	ret = v4l2_relbufs(v4l2_buf);
	if(ret < 0)
		goto err;

	v4l2_close(fd);
	fclose(fp);

	return 0;

err:
	perror("err");

	return -1;
}
