#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#include "libv4l2/libv4l2.h"

int main(int argc, char* argv[])
{
	int fd;
	int ret;
	char name[100];
	int width = 640, height = 480;
	int nr_bufs = 4;
	int i;
	struct v4l2_capability cap;
	struct v4l2_buf* v4l2_buf;
	struct v4l2_buf_unit* v4l2_buf_unit;
	FILE* fp;
	char str[20] = {0};
	int t;

	memset(&cap, 0, sizeof(cap));

	if (argc != 2) {
		printf("Usage:%s </dev/videox>\n", argv[0]);
		return -1;
	}
	t = time((time_t*)NULL);
	sprintf(str, "%d.yuv", t);

	fp = fopen(str, "w");
	if (!fp) {
		printf("failed to open picture\n");
		return -1;
	}

	fd = v4l2_open(argv[1], O_RDWR);
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

	ret = v4l2_enuminput(fd, 0, name);
	if(ret < 0)
		goto err;
	printf("input device name:%s\n", name);

	ret = v4l2_s_input(fd, 0);
	if(ret < 0)
		goto err;

	ret = v4l2_enum_fmt(fd, V4L2_PIX_FMT_YUYV, V4L2_BUF_TYPE_VIDEO_CAPTURE);
	if(ret < 0)
		goto err;

	ret = v4l2_s_fmt(fd, &width, &height, V4L2_PIX_FMT_YUYV, V4L2_BUF_TYPE_VIDEO_CAPTURE);
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
