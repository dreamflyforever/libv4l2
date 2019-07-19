#include "widget.h"
#include "ui_widget.h"
#include "color.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <QDebug>
#include <QHBoxLayout>

#define VIDEO_DEV "/dev/video0"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    timer(new QTimer(this)),
    label(new QLabel(this)),
    width(640),
    height(480),
    camStatus(false)
{
    ui->setupUi(this);
    this->resize(width, height);

    image = new QImage(width, height, QImage::Format_RGB32);

    cameraInit();
    connect(timer, &QTimer::timeout, this, &Widget::repaintImage);
    connect(ui->pushButton_camCtrl, &QPushButton::clicked, this, &Widget::btnCtrlClicked);
}

Widget::~Widget()
{
    delete ui;
    delete image;
    cameraEXit();
}

void Widget::repaintImage()
{
    //static int color = 0;
    int ret;
    struct v4l2_buf_unit* v4l2_buf_unit;
    unsigned char *bits=image->bits();

    ret = v4l2_poll(fd);
    if(ret < 0)
        return;

    v4l2_buf_unit = v4l2_dqbuf(fd, v4l2_buf);
    if(!v4l2_buf_unit)
        return;

    YUYV2RGB32((uint8_t*)v4l2_buf_unit->start, bits, width, height);

    QPixmap pixmap = QPixmap::fromImage(*image);
    pixmap = pixmap.scaled(ui->label->size());
    ui->label->setAutoFillBackground(true);

    QPalette palette;
    palette.setBrush(ui->label->backgroundRole(), QBrush(pixmap));
    ui->label->setPalette(palette);
    ui->label->repaint();

    ret = v4l2_qbuf(fd, v4l2_buf_unit);
    if(ret < 0)
        return;
}

void Widget::btnCtrlClicked()
{
    if(camStatus == false)
    {
        ui->pushButton_camCtrl->setText("关闭设备");
        camStatus = true;
        timer->start(66);
    }
    else
    {
        ui->pushButton_camCtrl->setText("打开设备");
        camStatus = false;
        timer->stop();
    }
}

int Widget::cameraInit()
{
    int ret;
    char name[100];
    int nr_bufs = 4;
    struct v4l2_capability cap;

    memset(&cap, 0, sizeof(cap));

    fd = v4l2_open(VIDEO_DEV, O_RDWR);
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

err:
    return -1;
}

int Widget::cameraEXit()
{
    int ret;

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

    return 0;

err:
    return -1;
}
