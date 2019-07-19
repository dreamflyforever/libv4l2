#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <sys/mman.h>

#include "libv4l2/libv4l2.h"
//#include "libv4l2/yuv2rgb.h"
#include "libv4l2/color.h"

#define FB_NAME "/dev/fb0"

int fb_open(void);
int fb_close();
void fb_show_img(unsigned int width, unsigned int height, unsigned char* rgb32);

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
    unsigned char* buf;

    if(argc != 2)
    {
        printf("Usage:%s </dev/videox>\n", argv[0]);
        return -1;
    }

    memset(&cap, 0, sizeof(cap));
    buf = malloc(width*height*4);

    initLut();
    fb_open();

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

    while(1)
    {
        ret = v4l2_poll(fd);
        if(ret < 0)
            goto err;
    
        v4l2_buf_unit = v4l2_dqbuf(fd, v4l2_buf);
        if(!v4l2_buf_unit)
            goto err;

        YUYV2RGB32(v4l2_buf_unit->start, buf, width, height);
        fb_show_img(width, height, buf);
        
        ret = v4l2_qbuf(fd, v4l2_buf_unit);
        if(ret < 0)
            goto err;
    }
    
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
    fb_close();

    freeLut();
    free(buf);
    
    return 0;

err:
    perror("err");

    return -1;
}

static unsigned int* fb_addr;
static int fb_fd;
static int fb_width;
static int fb_height;

int fb_open(void)
{
    int ret = -1;
	
    struct fb_fix_screeninfo finfo;
    struct fb_var_screeninfo vinfo;
 
    memset(&finfo, 0, sizeof(finfo));
    memset(&vinfo, 0, sizeof(vinfo));
 
    /* 第1步：打开设备 */
    fb_fd = open(FB_NAME, O_RDWR);
    if (fb_fd < 0)
    {
        perror("open");
        return -1;
    }
 
    /* 第2步：获取设备的硬件信息，如屏幕分辨率，颜色位深等信息 */
    ret = ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo);
    if (ret < 0)
    {
        perror("ioctl");
        return -1;
    }
 
    ret = ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);
    if (ret < 0)
    {
        perror("ioctl");
        return -1;
    }

    fb_width = vinfo.xres_virtual;
    fb_height = vinfo.yres_virtual;
    
    /* 第3步：进行mmap，申请fb */
    unsigned long len = vinfo.xres_virtual * vinfo.yres_virtual *     \
                                                    vinfo.bits_per_pixel / 8;
 
    fb_addr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
 
    if (NULL == fb_addr)
    {
        perror("mmap");
        return -1;
    }
}

int fb_close()
{
    close(fb_fd);
}

void inline fb_put_pixel(unsigned int x, unsigned int y, unsigned int color)
{
    *(fb_addr + y*fb_width + x) = color;
}

void fb_show_img(unsigned int width, unsigned int height, unsigned char* rgb32)
{
    int m_width;
    int m_height;
    int x, y;
    unsigned int* prgb32 = (unsigned int*)rgb32;
    int color;

    m_width = width < fb_width ? width : fb_width;
    m_height = height < fb_height ? height : fb_height;

    for(y = 0; y < m_height; ++y)
    {
        for(x = 0; x < m_width; ++x)
        {
            color = *(prgb32+y*width+x);
            fb_put_pixel(x, y, color);
        }
    }
}
